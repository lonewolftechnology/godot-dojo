#!/usr/bin/env python
import os
import sys
from SCons.Variables import BoolVariable

from methods import print_error

libname = "godot-dojo"
projectdir = "demo/addons/godot-dojo"

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
opts.Add(BoolVariable("use_wasm_pack", "Compile Rust for web using wasm-pack instead of standard cargo with emscripten", False))
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("external/godot-cpp") and os.listdir("external/godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env["libname"] = libname
env["projectdir"] = projectdir

# Appending toolpath for common_compiler_flags
sys.path.append(os.path.abspath("external/godot-cpp/tools"))

# Apply godot-cpp configuration (This has to happen before our compilation to parse correct flags like platform/arch)
env = SConscript("external/godot-cpp/SConstruct", {"env": env, "customs": customs})

# Apply our custom tools
env.Tool("rust", toolpath=["tools"]) # THIS COMPILES RUST STRICTLY FIRST IMMEDIATELY
env.Tool("common", toolpath=["tools"])

# Platform-specific tools
platform = env["platform"]
if os.path.exists(f"tools/{platform}.py"):
    env.Tool(platform, toolpath=["tools"])

if env.get("RUST_LIB_PATH"):
    env.Append(LIBS=[File(env["RUST_LIB_PATH"])])

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
    for file in files:
        if file.endswith(".cpp"):
            filepath = os.path.join(root, file)
            
            if env["platform"] == "web" and filepath in web_excludes:
                continue
                
            sources.append(filepath)
sources.extend([
    "bindings/dojo/dojo.cpp",
    "bindings/controller/controller.cpp"
    ])

if env["platform"] == "web":
    env.Append(CPPPATH=["web/include", "."])

    for root, dirs, files in os.walk("web/src"):
        for file in files:
            if file.endswith(".cpp"):
                filepath = os.path.join(root, file)

                sources.append(filepath)

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

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
