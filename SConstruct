#!/usr/bin/env python
import os
import sys
import shutil

# Project Configurations
PROJECT_NAME = "dojoc-gdextension"
ENTRY_POINT = "dojoc_library_init"
GODOT_MIN_REQUIREMENT = "4.2"

# Generate .gdextension file
conf_dict = {
    r'${PROJECT_NAME}': PROJECT_NAME,
    r'${ENTRY_POINT}': ENTRY_POINT,
    r'${GODOT_MIN_REQUIREMENT}': GODOT_MIN_REQUIREMENT
}
env = Environment(tools=['textfile'])
env.Substfile('plugin_template.gdextension.in', SUBST_DICT=conf_dict)

# Gets the standard flags CC, CCX, etc.
env = SConscript("godot-cpp/SConstruct")

# Dojo.C
## Helper variables
rust_libname = "libdojo_c" + env['SHLIBSUFFIX']
rust_extra_args = ""
rust_lib = ""
rust_target = ""

# Platform specifics
if env['platform'] == "windows":
    # Con msvc, todo es dojo_c
    # Con mingw (linux, no probe en windows) es dojo_c para el dll
    # el resto es libdojo_c
    rust_libname = rust_libname[3:]

    if env.get("is_msvc", False):
        rust_target = "x86_64-pc-windows-msvc"
        rust_libname = rust_libname.replace(".dll", ".lib")
        env['CXXFLAGS'].remove("/std:c++17")
        env.Append(CXXFLAGS=["/std:c++20"])
        env.Append(
            LINKFLAGS=[
                '/NODEFAULTLIB:MSVCRT'
            ]
        )
    else:
        # rust_libname = rust_libname.replace(".dll", ".dll.a")
        rust_target = "x86_64-pc-windows-gnu"

elif env['platform'] == "linux":
    rust_target = "x86_64-unknown-linux-gnu"
    env.Append(
        LINKFLAGS=["-ldbus-1"],
    )

elif env['platform'] == "macos":
    if env['arch'] == "arm64":
        rust_target = "aarch64-apple-darwin"
    else:
        rust_target = "x86_64-apple-darwin"

rust_libname = rust_libname.replace(".so", ".a")

## Build rust
if env["target"] == "template_release":
    rust_extra_args += "--release"

rust_lib_path = 'dojo.c/target/{}/{}'.format(rust_target, env["target"].replace("template_", ""))
rust_lib = "{}/{}".format(rust_lib_path, rust_libname)

env.Execute(
    action="cargo build --target {} {}".format(rust_target, rust_extra_args),
    chdir="dojo.c"
)
os.chdir("..")
# Copy compiled library into base directory
# local_rust = env.Command(
#     target=rust_libname,
#     source=rust_lib,
#     action=Copy('$TARGET', '$SOURCE'))

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags
env.Append(
    CPPPATH=[
        "src/",
        "include/",
        "dojo.c",
        "godot-cpp/include/godot_cpp",
        "godot-cpp/gen/include/godot_cpp",
        "godot-cpp/gdextension"
    ],
    LIBS=[
        File("./" + rust_lib)
    ],
    LINKFLAGS=[
        "-static-libgcc",
        "-static-libstdc++"
    ]

)

sources = Glob("src/*.cpp")
sources += Glob("src/classes/*.cpp")
sources += Glob("src/variant/*.cpp")

addon_dir = "demo/bin/"

if env["platform"] == "macos":
    library = env.SharedLibrary(
        addon_dir + "/libdojoc.{}.{}.framework/dojoc.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":  # No lo borro por las dudas si surge que hay que hacer ios
    if env["ios_simulator"]:
        library = env.StaticLibrary(
            addon_dir + "/dojoc.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary(
            addon_dir + "/dojoc.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary(
        addon_dir + "dojoc{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

Default(library)
