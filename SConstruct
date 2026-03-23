#!/usr/bin/env python
import os
import sys
import re

from methods import print_error

def patch_dojo_header_vtable(content):
    # Logic from original SConstruct to patch vtable initialization order
    vtable_start_hook = "static inline UniffiVTableCallbackInterface"
    vtable_end_hook = "};"
    free_member = ".uniffi_free = reinterpret_cast<void *>(&uniffi_free)"

    replacements = []
    current_pos = 0

    while True:
        start = content.find(vtable_start_hook, current_pos)
        if start == -1:
            break

        end_offset = content[start:].find(vtable_end_hook)
        if end_offset == -1:
            break

        end = start + end_offset + len(vtable_end_hook)
        vtable_block = content[start:end]

        if free_member in vtable_block:
            brace_start = vtable_block.find('{')
            brace_end = vtable_block.rfind('}')

            if brace_start != -1 and brace_end != -1:
                init_block = vtable_block[brace_start + 1:brace_end]
                members = [s.strip() for s in init_block.split(',') if s.strip()]

                # Buscar índice del miembro que contiene .uniffi_free
                pos = next((i for i, m in enumerate(members) if ".uniffi_free" in m), -1)

                if pos > 0:
                    free_mem = members.pop(pos)
                    members.insert(0, free_mem)

                    corrected_init = "\n            " + ",\n            ".join(members) + "\n        "
                    final_block = vtable_block.replace(init_block, corrected_init)
                    replacements.append((vtable_block, final_block))

        current_pos = end

    for old, new in replacements:
        content = content.replace(old, new)

    return content

def patch_uniffi_files():
    files = [
        "bindings/dojo/dojo_scaffolding.hpp",
        "bindings/dojo/dojo.hpp",
        "bindings/dojo/dojo.cpp",
        "bindings/controller/controller_scaffolding.hpp",
        "bindings/controller/controller.hpp",
        "bindings/controller/controller.cpp"
    ]

    for file_path in files:
        if not os.path.exists(file_path):
            continue

        with open(file_path, "r") as f:
            content = f.read()

        new_content = content

        if "scaffolding.hpp" in file_path:
            # Replace void * return types for uniffi functions
            new_content = re.sub(r'void \*\s+(uniffi_[a-z0-9_]+_fn_)', r'uint64_t \1', new_content)
            # Replace void * arguments named ptr, handle, owner
            new_content = re.sub(r'void \*\s+(ptr|handle|owner)', r'uint64_t \1', new_content)

        elif ".hpp" in file_path:
             # void *instance -> uint64_t instance
             new_content = new_content.replace("void *instance = nullptr;", "uint64_t instance = 0;")
             # ToriiClient(void *) -> ToriiClient(uint64_t)
             new_content = re.sub(r'([a-zA-Z0-9]+)\(void \*\)', r'\1(uint64_t)', new_content)
             # lift(void *) -> lift(uint64_t)
             new_content = new_content.replace("lift(void *)", "lift(uint64_t)")
             # void *lower -> uint64_t lower
             new_content = new_content.replace("void *lower", "uint64_t lower")
             # void *_uniffi_internal_clone_pointer -> uint64_t _uniffi_internal_clone_pointer
             new_content = new_content.replace("void *_uniffi_internal_clone_pointer", "uint64_t _uniffi_internal_clone_pointer")

             if "dojo.hpp" in file_path:
                 new_content = patch_dojo_header_vtable(new_content)

        elif ".cpp" in file_path:
             # Constructor definition
             new_content = re.sub(r'([a-zA-Z0-9]+)::\1\(void \*ptr\)', r'\1::\1(uint64_t ptr)', new_content)
             # _uniffi_internal_clone_pointer definition
             new_content = re.sub(r'void \*([a-zA-Z0-9]+)::_uniffi_internal_clone_pointer', r'uint64_t \1::_uniffi_internal_clone_pointer', new_content)
             # lift definition
             new_content = re.sub(r'std::shared_ptr<([a-zA-Z0-9]+)> FfiConverter\1::lift\(void \*ptr\)', r'std::shared_ptr<\1> FfiConverter\1::lift(uint64_t ptr)', new_content)
             # lift call inside read
             new_content = re.sub(r'FfiConverter([a-zA-Z0-9]+)::lift\(reinterpret_cast<void \*>\(ptr\)\)', r'FfiConverter\1::lift(ptr)', new_content)
             # lower definition
             new_content = re.sub(r'void \*FfiConverter([a-zA-Z0-9]+)::lower', r'uint64_t FfiConverter\1::lower', new_content)
             # lower implementation
             new_content = re.sub(r'return reinterpret_cast<([a-zA-Z0-9]+)\*>\(obj\.get\(\)\)->_uniffi_internal_clone_pointer\(\);', r'return obj->_uniffi_internal_clone_pointer();', new_content)
             # write implementation
             new_content = re.sub(r'stream << reinterpret_cast<std::uintptr_t>\(FfiConverter([a-zA-Z0-9]+)::lower\(obj\)\);', r'stream << FfiConverter\1::lower(obj);', new_content)
             # read implementation
             new_content = new_content.replace("std::uintptr_t ptr;", "uint64_t ptr;")
             # Fix copy constructor initialization with nullptr
             new_content = re.sub(r'([a-zA-Z0-9]+)::\1\(const \1 &other\) : instance\(nullptr\)', r'\1::\1(const \1 &other) : instance(0)', new_content)

        if new_content != content:
            with open(file_path, "w") as f:
                f.write(new_content)
            print(f"Patched {file_path}")


libname = "godot-dojo"
projectdir = "demo"

localEnv = Environment(tools=["default"], PLATFORM="", ENV=os.environ)

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()



if not (os.path.isdir("external/godot-cpp") and os.listdir("external/godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("external/godot-cpp/SConstruct", {"env": env, "customs": customs})

if env["platform"] == "web":
    patch_uniffi_files()

targets = {
    ("windows", "x86_64"): "x86_64-pc-windows-gnu",
    ("linux", "x86_64"): "x86_64-unknown-linux-gnu",
    ("linux", "arm64"): "aarch64-unknown-linux-gnu",
    ("macos", "x86_64"): "x86_64-apple-darwin",
    ("macos", "arm64"): "aarch64-apple-darwin",
    ("macos", "universal"): ["x86_64-apple-darwin", "aarch64-apple-darwin"],
    ("web", "wasm32"): "wasm32-unknown-emscripten",
    ("android", "arm64"): "aarch64-linux-android",
    ("ios", "arm64"): "aarch64-apple-ios" if not env['ios_simulator'] else "aarch64-apple-ios-sim",
    ("ios", "x86_64"): "x86_64-apple-ios",
    ("ios", "universal"): ["aarch64-apple-ios", "x86_64-apple-ios", "aarch64-apple-ios-sim"],
}

rust_lib_path = f"godot-dojo-core/target/x86_64-unknown-linux-gnu/debug/libgodot_dojo_core.a"
rust_web_lib_path = f"godot-dojo-core/target/wasm32-unknown-emscripten/debug/libgodot_dojo_core.a"
# rust_web_lib_path = f"godot-dojo-core/target/wasm32-unknown-unknown/debug/libgodot_dojo_core.a"
# env.Append(CXXFLAGS=["-std=c++20"])
if env["platform"] == "web":
    env['ENV']['EMCC_DEBUG'] = '1'
    env['ENV']['EMCC_DEBUG_SAVE'] = '1'
    env.Append(SHLINKFLAGS=[
    "-sASSERTIONS=2",
    "-sSTACK_OVERFLOW_CHECK=0",
#     "-sSAFE_HEAP=1",
    "-sABORTING_MALLOC=0",
    "-g",
    "-gsource-map",
    "-semit-symbol-map",
    "--source-map-base http://localhost:8060/",
    "--profiling-funcs",
    "-fwasm-exceptions",
#     "-sWASM_BINDGEN",
    ])
    env.Append(LINKFLAGS=[
    "-fwasm-exceptions",
    "-sASSERTIONS=2"
    ])

    # Activar el soporte experimental de wasm-bindgen en el enlazado
#     env.Append(SHLINKFLAGS=['-sWASM_BINDGEN'])

#     env.Append(LIBS=[File(rust_web_lib_path)])
else:
    env.Append(LIBS=[File(rust_lib_path)])

# env.Append(CPPDEFINES=["BOOST_NO_EXCEPTIONS"])

env.Append(CPPPATH=["src/", "include/", "external/boost/include"])

web_excludes = [
    os.path.normpath("src/node/torii_client.cpp"),
    os.path.normpath("src/node/dojo_controller.cpp"),
    os.path.normpath("src/node/dojo_session_account.cpp"),
    os.path.normpath("src/ref_counted/dojo_utilities/callback.cpp"),
    os.path.normpath("src/ref_counted/dojo_utilities/callback_utils.cpp"),
    os.path.normpath("src/ref_counted/controller_utilities/dojo_owner.cpp"),
    os.path.normpath("src/tools/controller_helper.cpp"),
]

sources = []
for root, dirs, files in os.walk("src"):
    # Si la plataforma no es 'web', omitimos el directorio 'web' por completo
    if env["platform"] != "web" and "web" in dirs:
        dirs.remove("web")

    for file in files:
        if file.endswith(".cpp"):
            filepath = os.path.join(root, file)
            
            # Si es web, saltarnos los archivos originales que no son compatibles
            if env["platform"] == "web" and filepath in web_excludes:
                continue
                
            sources.append(filepath)


if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# DEBUG: Imprimir el entorno completo si estamos en web para verificar flags de Emscripten
if env["platform"] == "web":
    print(env.Dump())
    env.Append(CPPPATH=["include/web"])
else:
    sources.extend(["bindings/controller/controller.cpp", "bindings/dojo/dojo.cpp"])
    env.Append(CPPPATH=["bindings/"])

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
