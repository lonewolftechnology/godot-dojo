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
env.Substfile('plugin_template.gdextension.in', SUBST_DICT = conf_dict)

# Gets the standard flags CC, CCX, etc.
env = SConscript("godot-cpp/SConstruct")

# Dojo.C
## Helper variables
if env['platform'] == "windows":
    rust_libname = "dojo_c"
else:
    rust_libname = "libdojo_c"
rust_libname += env['SHLIBSUFFIX']
rust_lib = 'dojo.c/target/{}/{}'.format(env["target"].replace("template_",""), rust_libname)

## Build rust
rust_extra_args = ""
# TODO: PLATFORM TARGETS
if env["target"] == "template_release":
    rust_extra_args += "--release"

#if env['platform'] == "macos":
#    if env['arch'] == "arm64":
#        rust_extra_args += "--target aarch64-apple-darwin"
#    else:
#        rust_extra_args += "--target x86_64-apple-darwin"

env.Execute(
    action="cargo build {}".format(rust_extra_args),
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

# tweak this if you want to use different folders, or more folders, to store your source code in.
if env['platform'] == "linux":
    env.Append(
        LINKFLAGS=["-ldbus-1"]
    )
    
env.Append(
    CPPPATH=[
        "src/",
        "include/",
        "dojo.c",
        "godot-cpp/include/godot_cpp",
        "godot-cpp/gen/include/godot_cpp"
    ]
)

if env['platform'] == "windows":
    env.Append(
        LIBS=[File("./" + rust_lib.replace(".dll", ".lib"))]
    )
    env.Append(LINKFLAGS=['/NODEFAULTLIB:MSVCRT'])
else:
    env.Append(
        LIBS=[File("./" + rust_lib.replace(".so", ".a"))]
    )


sources = Glob("src/*.cpp")
sources += Glob("src/types/*.cpp")

addon_dir = "demo/bin/"

if env.get("is_msvc", False):
    env['CXXFLAGS'].remove("/std:c++17")
    env.Append(CXXFLAGS=["/std:c++20"])


if env["platform"] == "macos":
    library = env.SharedLibrary(
        addon_dir + "/libdojoc.{}.{}.framework/dojoc.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios": # No lo borro por las dudas si surge que hay que hacer ios
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
