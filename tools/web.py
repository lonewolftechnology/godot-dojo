import os
import re

def patch_dojo_header_vtable(content):
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
            new_content = re.sub(r'void \*\s+(uniffi_[a-z0-9_]+_fn_)', r'uint64_t \1', new_content)
            new_content = re.sub(r'void \*\s+(ptr|handle|owner)', r'uint64_t \1', new_content)

        elif ".hpp" in file_path:
             new_content = new_content.replace("void *instance = nullptr;", "uint64_t instance = 0;")
             new_content = re.sub(r'([a-zA-Z0-9]+)\(void \*\)', r'\1(uint64_t)', new_content)
             new_content = new_content.replace("lift(void *)", "lift(uint64_t)")
             new_content = new_content.replace("void *lower", "uint64_t lower")
             new_content = new_content.replace("void *_uniffi_internal_clone_pointer", "uint64_t _uniffi_internal_clone_pointer")

             if "dojo.hpp" in file_path:
                 new_content = patch_dojo_header_vtable(new_content)

        elif ".cpp" in file_path:
             new_content = re.sub(r'([a-zA-Z0-9]+)::\1\(void \*ptr\)', r'\1::\1(uint64_t ptr)', new_content)
             new_content = re.sub(r'void \*([a-zA-Z0-9]+)::_uniffi_internal_clone_pointer', r'uint64_t \1::_uniffi_internal_clone_pointer', new_content)
             new_content = re.sub(r'std::shared_ptr<([a-zA-Z0-9]+)> FfiConverter\1::lift\(void \*ptr\)', r'std::shared_ptr<\1> FfiConverter\1::lift(uint64_t ptr)', new_content)
             new_content = re.sub(r'FfiConverter([a-zA-Z0-9]+)::lift\(reinterpret_cast<void \*>\(ptr\)\)', r'FfiConverter\1::lift(ptr)', new_content)
             new_content = re.sub(r'void \*FfiConverter([a-zA-Z0-9]+)::lower', r'uint64_t FfiConverter\1::lower', new_content)
             new_content = re.sub(r'return reinterpret_cast<([a-zA-Z0-9]+)\*>\(obj\.get\(\)\)->_uniffi_internal_clone_pointer\(\);', r'return obj->_uniffi_internal_clone_pointer();', new_content)
             new_content = re.sub(r'stream << reinterpret_cast<std::uintptr_t>\(FfiConverter([a-zA-Z0-9]+)::lower\(obj\)\);', r'stream << FfiConverter\1::lower(obj);', new_content)
             new_content = new_content.replace("std::uintptr_t ptr;", "uint64_t ptr;")
             new_content = re.sub(r'([a-zA-Z0-9]+)::\1\(const \1 &other\) : instance\(nullptr\)', r'\1::\1(const \1 &other) : instance(0)', new_content)

        if new_content != content:
            with open(file_path, "w") as f:
                f.write(new_content)
            print(f"Patched {file_path}")

def options(opts):
    pass

def exists(env):
    return True

def generate(env):
    patch_uniffi_files()

    env['ENV']['EMCC_DEBUG'] = '1'
    env['ENV']['EMCC_DEBUG_SAVE'] = '1'
    env.Append(SHLINKFLAGS=[
        "-sASSERTIONS=2",
        "-sSTACK_OVERFLOW_CHECK=0",
        "-sABORTING_MALLOC=0",
        "-g",
        "-gsource-map",
        "-semit-symbol-map",
        "--source-map-base http://localhost:8060/",
        "--profiling-funcs",
        # "-fwasm-exceptions",
    ])
    env.Append(LINKFLAGS=[
        # "-fwasm-exceptions",
        "-sASSERTIONS=2",
        # "-sWASM_BINDGEN"
    ])
    env.Append(CPPPATH=["web/include"])
    env.Append(CPPDEFINES=["WEB_ENABLED"])
    
    try:
        import common_compiler_flags
        common_compiler_flags.generate(env)
    except:
        pass