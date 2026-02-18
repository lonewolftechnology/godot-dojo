#!/usr/bin/env python
import os
import subprocess
import glob
import shutil
import platform as host_platform
import sys

# --- IDE helper imports for SCons (no effect during actual scons run) ---
# CLion/PyCharm may flag unresolved references because SCons injects these
# into the global namespace at runtime. Import them when available and
# provide safe fallbacks otherwise so IDEs stop warning.
try:
    from SCons.Script import GetOption, Return, Exit, File, SConscript, Glob, COMMAND_LINE_TARGETS  # type: ignore
except Exception:
    def GetOption(name: str):
        return None


    def Return():
        pass


    def Exit(code: int = 0):
        raise SystemExit(code)


    def File(path: str):
        return path


    def SConscript(path: str):
        # Minimal shim to satisfy IDEs; never used during real scons runs
        class _Env(dict):
            def Append(self, **kwargs):
                pass

            def AddPostAction(self, *args, **kwargs):
                pass

            def Default(self, *args, **kwargs):
                pass

            def SharedLibrary(self, *args, **kwargs):
                return args[0] if args else None

        return _Env()

    def Glob(pattern: str):
        return glob.glob(pattern)

    COMMAND_LINE_TARGETS = sys.argv

# Colors and Emojis
G, B, R, Y, X = '\033[92m', '\033[94m', '\033[91m', '\033[1;33m', '\033[0m'
rocket, broom, check, package, clipboard, party, cross = "🚀", "🧹", "✅", "📦", "📋", "🎉", "❌"

is_windows = host_platform.system().lower() == "windows"
if is_windows:
    try:
        import ctypes
        kernel32 = ctypes.windll.kernel32
        h_stdout = kernel32.GetStdHandle(-11)  # STD_OUTPUT_HANDLE
        mode = ctypes.c_ulong()
        # It's important to check the return value of GetConsoleMode.
        if kernel32.GetConsoleMode(h_stdout, ctypes.byref(mode)) == 0:
            raise ctypes.WinError()
        mode.value |= 0x0004  # ENABLE_VIRTUAL_TERMINAL_PROCESSING
        # Also check the return value of SetConsoleMode.
        if kernel32.SetConsoleMode(h_stdout, mode) == 0:
            raise ctypes.WinError()
    except Exception:
        # Fallback to no colors and ASCII symbols if ANSI support fails
        G, B, R, Y, X = '', '', '', '', ''
        rocket, broom, check, package, clipboard, party, cross = ">", "-", "+", "#", "=", "!", "x"

print(f"{B}{rocket} Building godot-dojo{X}")

# Cleanup
if GetOption('clean'):
    print(f"{Y}{broom} Starting cleanup process...{X}")

    # Limpiar proyecto Rust godot-dojo-core
    print(f"{B}  -> Cleaning Rust project: godot-dojo-core{X}")
    try:
        subprocess.run(["cargo", "clean"], cwd="godot-dojo-core", check=True, capture_output=False, text=True)
        print(f"{G}     {check} godot-dojo-core cleanup complete.{X}")
    except subprocess.CalledProcessError as e:
        print(f"{R}     {cross} godot-dojo-core cleanup failed: {e.stderr}{X}")

    # Eliminar directorio del addon
    print(f"{B}  -> Deleting addon directory: demo/addons/godot-dojo{X}")
    shutil.rmtree("demo/addons/godot-dojo", ignore_errors=True)
    print(f"{G}     {check} Addon directory deleted.{X}")

    # Eliminar assets del addon
    print(f"{B}  -> Deleting addon assets directory: demo/addons/godot-dojo/assets{X}")
    shutil.rmtree("demo/addons/godot-dojo/assets", ignore_errors=True)
    print(f"{G}     {check} Addon assets directory deleted.{X}")

    # Eliminar directorio de distribución
    print(f"{B}  -> Deleting distribution directory: demo/dist{X}")
    shutil.rmtree("demo/dist", ignore_errors=True)
    print(f"{G}     {check} Distribution directory deleted.{X}")

    # Eliminar xcframework
    print(f"{B}  -> Deleting XCFramework directory: demo/dist/godot-dojo.xcframework{X}")
    shutil.rmtree("demo/dist/godot-dojo.xcframework", ignore_errors=True)
    print(f"{G}     {check} XCFramework directory deleted.{X}")

    # Limpiar submódulo godot-cpp
    print(f"{B}  -> Cleaning godot-cpp submodule...{X}")
    subprocess.run(["scons", "-C", "external/godot-cpp", "--clean"], check=False, capture_output=False)
    print(f"{G}     {check} godot-cpp cleanup complete.{X}")

    # Eliminar directorio de objetos intermedios
    print(f"{B}  -> Deleting intermediate object directory: build/obj{X}")
    shutil.rmtree("build/obj", ignore_errors=True)
    print(f"{G}     {check} Intermediate object directory deleted.{X}")

    print(f"\n{G}{check} Cleanup process finished.{X}")
    Return()

# Setup
os.makedirs("demo/addons/godot-dojo", exist_ok=True)

# Initialize the godot-cpp build environment
env = SConscript("external/godot-cpp/SConstruct")

platform, arch, target = env["platform"], env["arch"], env.get("target", "template_debug")

# Validate that 'assemble-ios' is used with 'platform=ios'
if "assemble-ios" in COMMAND_LINE_TARGETS and platform != "ios":
    print(f"{R}{cross} The 'assemble-ios' target requires 'platform=ios'. Please run the command as 'scons platform=ios assemble-ios'.{X}")
    Exit(1)


build_info = f"{platform} ({arch}) - {target}"
precision = env.get("precision", "single")
if precision == "double":
    build_info += " (double precision)"

print(f"{B}Building: {build_info}{X}")

# Compile Rust
# Se usa más adelante, pero se importa aquí para que esté disponible
import re
def get_git_version():
    try:
        return subprocess.check_output(
            ["git", "describe", "--tags", "--always", "--dirty"],
            stderr=subprocess.DEVNULL,
            text=True
        ).strip()
    except Exception:
        return "0.0.0-unknown"

def _get_git_submodule_version(submodule_path):
    """
    Detects the version of a git submodule by trying to find a tag, then a branch name,
    then the branch from .gitmodules, and finally the short commit hash.
    Returns the found version string or None if all methods fail.
    """
    repo_path = os.path.join(os.getcwd(), submodule_path)
    version_source = None
    try:
        # Prefer the latest reachable tag.
        version_source = subprocess.check_output(
            ["git", "describe", "--tags", "--abbrev=0"], cwd=repo_path, stderr=subprocess.DEVNULL, text=True
        ).strip()
    except Exception:
        try:
            # Fallback to branch name if no tag is found.
            version_source = subprocess.check_output(
                ["git", "symbolic-ref", "-q", "--short", "HEAD"], cwd=repo_path, stderr=subprocess.DEVNULL, text=True
            ).strip()
        except Exception:
            # As a last resort, try reading the .gitmodules declared branch.
            try:
                with open(os.path.join(os.getcwd(), ".gitmodules"), "r") as gm:
                    gm_text = gm.read()
                    m = re.search(rf'\[submodule "{submodule_path}"\][\s\S]*?branch\s*=\s*([^\n\r]+)', gm_text)
                    if m:
                        version_source = m.group(1).strip()
            except Exception:
                # Final fallback to short commit hash.
                try:
                    version_source = subprocess.check_output(
                        ["git", "rev-parse", "--short", "HEAD"], cwd=repo_path, stderr=subprocess.DEVNULL, text=True
                    ).strip()
                except Exception:
                    return None # All methods failed
    return version_source


def _detect_godot_min_requirement():
    version_source = _get_git_submodule_version("external/godot-cpp")
    if version_source:
        m = re.search(r"(\d+)\.(\d+)", version_source)
        if m:
            return f"{m.group(1)}.{m.group(2)}"
    # Default fallback if detection fails.
    return "4.2"

# Detect host configuration and toolchain
is_host_windows = host_platform.system().lower() == "windows"
use_mingw = env.get("use_mingw", False)

targets = {
    ("windows", "x86_64"): "x86_64-pc-windows-gnu" if (not is_host_windows or use_mingw) else "x86_64-pc-windows-msvc",
    ("linux", "x86_64"): "x86_64-unknown-linux-gnu",
    ("linux", "arm64"): "aarch64-unknown-linux-gnu",
    ("macos", "x86_64"): "x86_64-apple-darwin",
    ("macos", "arm64"): "aarch64-apple-darwin",
    ("macos", "universal"): ["x86_64-apple-darwin", "aarch64-apple-darwin"],
    ("web", "wasm32"): "wasm32-unknown-unknown",
    ("android", "arm64"): "aarch64-linux-android",
    ("ios", "arm64"): "aarch64-apple-ios" if not env['ios_simulator'] else "aarch64-apple-ios-sim",
    ("ios", "x86_64"): "x86_64-apple-ios",
    ("ios", "universal"): ["aarch64-apple-ios", "x86_64-apple-ios", "aarch64-apple-ios-sim"],
}
# Use CARGO_BUILD_TARGET from environment if set, otherwise use the target from the platform/arch
rust_target = os.environ.get("CARGO_BUILD_TARGET", targets.get((platform, arch), "x86_64-unknown-linux-gnu"))

# Ensure the Rust target is installed
try:
    targets_to_check = rust_target if isinstance(rust_target, list) else [rust_target]
    print(f"{Y}Checking if Rust targets {targets_to_check} are installed...{X}")
    result = subprocess.run(["rustup", "target", "list", "--installed"], capture_output=True, text=True, check=True)
    installed_targets = result.stdout.splitlines()

    all_installed = True
    for rt in targets_to_check:
        if rt not in installed_targets:
            all_installed = False
            print(f"{Y}Installing Rust target {rt}...{X}")
            subprocess.run(["rustup", "target", "add", rt], check=True)
            print(f"{G}{check} Rust target {rt} installed{X}")

    if all_installed:
        print(f"{G}{check} All required Rust targets are already installed.{X}")

    # Ensure targets are also available in the submodule directory context
    for rt in targets_to_check:
        subprocess.run(["rustup", "target", "add", rt], check=False, cwd="godot-dojo-core", capture_output=False)

except subprocess.CalledProcessError as e:
    print(f"{R}{cross} Failed to check or install Rust target: {e}{X}")
    # Continue anyway, as cargo will show a more specific error if needed

def _compile_rust_library(lib_name, lib_path, is_release, cargo_flags=None, rustc_flags=None):
    """Logic to compile a Rust library, handling normal, universal, and web builds."""
    print(f"{Y}{package} Compiling {lib_name}...{X}")

    build_mode = "release" if is_release else "debug"
    base_cmd = ["cargo", "build"]

    # Prepare environment variables
    env_vars = os.environ.copy()
    current_rustflags = env_vars.get("RUSTFLAGS", "")
    if rustc_flags:
        current_rustflags += " " + " ".join(rustc_flags)

    env_vars["RUSTFLAGS"] = current_rustflags.strip()
    
    # Use relative path for Cargo to avoid path duplication issues
    cargo_target_dir = "target"
    env_vars["CARGO_TARGET_DIR"] = cargo_target_dir
    
    # Path for SCons to find the artifacts (relative to root)
    scons_search_dir = f"{lib_path}/{cargo_target_dir}"

    if is_release:
        base_cmd.append("--release")

    if cargo_flags:
        base_cmd.extend(cargo_flags)

    # Always specify the target unless it's a universal macOS build (handled separately)
    if not (platform in ["macos", "ios"] and arch == "universal"):
        base_cmd.extend(["--target", rust_target])

    # Special handling for universal builds (macOS and iOS Simulator)
    if platform in ["macos", "ios"] and arch == "universal":
        print(f"{Y}Starting universal build for {lib_name} on {platform}...{X}")
        rust_targets_for_lipo = targets.get((platform, arch))
        libs_to_lipo = []

        for rt in rust_targets_for_lipo:
            print(f"{Y}Compiling {lib_name} for target: {rt}...{X}")
            cargo_cmd = ["cargo", "build", "--target", rt]
            if is_release:
                cargo_cmd.append("--release")

            if platform == "macos":
                deployment_target = os.environ.get("MACOSX_DEPLOYMENT_TARGET", "14.0")
                env_vars["MACOSX_DEPLOYMENT_TARGET"] = deployment_target
                rustflags = env_vars.get("RUSTFLAGS", "")
                if f"-mmacosx-version-min={deployment_target}" not in rustflags:
                    if rustflags:
                        rustflags += " "
                    rustflags += f"-C link-arg=-mmacosx-version-min={deployment_target}"
                env_vars["RUSTFLAGS"] = rustflags

            subprocess.run(cargo_cmd, check=True, cwd=lib_path, env=env_vars)
            libs_to_lipo.append(f"{scons_search_dir}/{rt}/{build_mode}/lib{lib_name}.a")

        print(f"{Y}Creating universal library for {lib_name} with lipo...{X}")
        universal_dir = f"{scons_search_dir}/universal/{build_mode}"
        os.makedirs(universal_dir, exist_ok=True)
        universal_lib_path = f"{universal_dir}/lib{lib_name}.a"

        lipo_cmd = ["lipo", "-create"] + libs_to_lipo + ["-output", universal_lib_path]
        subprocess.run(lipo_cmd, check=True)
        print(f"{G}{check} Universal library for {lib_name} created at: {universal_lib_path}{X}")
        return # Handled

    # Standard build for other platforms
    if platform == "macos":
        deployment_target = os.environ.get("MACOSX_DEPLOYMENT_TARGET", "14.0")
        print(f"{Y}Setting macOS deployment target to {deployment_target} for {lib_name}...{X}")
        env_vars['MACOSX_DEPLOYMENT_TARGET'] = deployment_target
        rustflags = env_vars.get("RUSTFLAGS", "")
        if f"-mmacosx-version-min={deployment_target}" not in rustflags:
            if rustflags:
                rustflags += " "
            rustflags += f"-C link-arg=-mmacosx-version-min={deployment_target}"
            env_vars["RUSTFLAGS"] = rustflags

    print(f"{Y}Running cargo command for {lib_name}: {' '.join(base_cmd)}{X}")
    subprocess.run(base_cmd, check=True, cwd=lib_path, env=env_vars)

# Determine if we should use the release build of the Rust crate.
# This is true if the SCons target is 'template_release' OR if we force it via an environment variable.
force_rust_release = os.environ.get("FORCE_RUST_RELEASE", "0") == "1"
is_release_build = target == "template_release" or force_rust_release

# By default, SCons compiles the Rust library.
# To prevent this (e.g., in CI where a separate 'cargo build' step is used),
# set the environment variable SKIP_RUST_BUILD=1.
if os.environ.get("SKIP_RUST_BUILD", "0") == "1":
    print(f"{Y}Skipping Rust library compilation from SCons because SKIP_RUST_BUILD is set.{X}")
else:
    _compile_rust_library('godot_dojo_core', 'godot-dojo-core', is_release_build)

# Configure library
if env["platform"] != "android":
    # For Android, the prefix is 'lib', which is standard. For other platforms,
    # we remove it to have a consistent naming scheme (e.g., 'godot-dojo.windows.dll').
    env['SHLIBPREFIX'] = ''
prefix = env.subst('$SHLIBPREFIX')

project_version = get_git_version()

# Generate version header
version_header_path = "include/gen/version.gen.hpp"
version_header_content = f"""/* THIS FILE IS GENERATED BY SCONS. DO NOT EDIT. */
#ifndef GODOT_DOJO_VERSION_GEN_HPP
#define GODOT_DOJO_VERSION_GEN_HPP
#define GODOT_DOJO_VERSION "{project_version}"
#define GODOT_DOJO_PRECISION "{precision}"
#endif // GODOT_DOJO_VERSION_GEN_HPP
"""
os.makedirs(os.path.dirname(version_header_path), exist_ok=True)
if not os.path.exists(version_header_path) or open(version_header_path, "r").read() != version_header_content:
    print(f"{Y}{clipboard} Generating {version_header_path}...{X}")
    with open(version_header_path, "w") as f:
        f.write(version_header_content)

env.Append(CPPPATH=["src/", "include/", "bindings/", "external/boost/include"])
if platform == "macos":
    deployment_target = os.environ.get("MACOSX_DEPLOYMENT_TARGET", "14.0")
    # Set macOS deployment target for C++ compilation
    print(f"{Y}Setting macOS deployment target for C++ compilation to {deployment_target}...{X}")
    env['ENV']['MACOSX_DEPLOYMENT_TARGET'] = deployment_target
    env.Append(CCFLAGS=[f'-mmacosx-version-min={deployment_target}'])
    env.Append(LINKFLAGS=[f'-mmacosx-version-min={deployment_target}'])

# Enable C++ exceptions for try/catch blocks
if env["platform"] == "windows" and not env.get("use_mingw"):
    # For MSVC
    # Use /std:c++17 to avoid issues with std::uniform_int_distribution
    # and char types in C++20.
    env.Append(CXXFLAGS=['/EHsc', '/std:c++17'])
else:
    # For GCC/Clang
    # Using C++17 for consistency and to avoid breaking changes from C++20.
    cxx_flags = ['-fexceptions', '-std=c++17']
    # -Wno-template-id-cdtor suppresses warnings from godot-cpp templates with GCC, but is not supported by Clang.
    if env["platform"] not in ["macos", "android", "ios"]:
        cxx_flags.append('-Wno-template-id-cdtor')
    env.Append(CXXFLAGS=cxx_flags)

# Link Rust libraries
rust_build_mode = "release" if is_release_build else "debug"

# Determine target directory for Rust libraries
if platform in ["macos", "ios"] and arch == "universal":
    rust_lib_target_dir = "universal"
else:
    rust_lib_target_dir = rust_target

rust_lib_base_path = f"godot-dojo-core/target/{rust_lib_target_dir}/{rust_build_mode}"

# Determine library path and name based on platform
if platform == "windows":
    rust_lib_path = f"{rust_lib_base_path}/{'lib' if use_mingw else ''}godot_dojo_core.{'a' if use_mingw else 'lib'}"
else:
    rust_lib_path = f"{rust_lib_base_path}/libgodot_dojo_core.a"

# Check if the library exists before proceeding
if "assemble-ios" not in COMMAND_LINE_TARGETS and not os.path.exists(rust_lib_path):
    print(f"{R}{cross} Could not find Rust library at {rust_lib_path}. Please build the Rust crate first.{X}")
    Exit(1)

# Configure linking for each platform
if platform == "windows":
    if use_mingw:
        win_libs = ['-lws2_32', '-ladvapi32', '-lntdll']
        env.Append(_LIBFLAGS=['-Wl,--start-group', rust_lib_path] + win_libs + ['-Wl,--end-group'])
    else:  # MSVC
        # Sidestep the SCons duplicate library issue by using /OPT:NOREF.
        # This tells the linker to keep all symbols, achieving the same as /WHOLEARCHIVE
        # but without conflicting with SCons's automatic dependency management.
        rust_lib_dir = os.path.dirname(rust_lib_path)
        rust_lib_name = os.path.splitext(os.path.basename(rust_lib_path))[0].replace("lib", "", 1)

        env.Append(LIBPATH=[rust_lib_dir])
        env.Append(LIBS=[rust_lib_name])
        env.Append(LINKFLAGS=['/OPT:NOREF', '/NODEFAULTLIB:MSVCRT'])
        env.Append(LIBS=['ws2_32', 'advapi32', 'ntdll'])
elif platform in ["linux", "android"]:
    env.Append(_LIBFLAGS=['-Wl,--start-group', rust_lib_path, '-Wl,--end-group'])
    if platform == "linux":
        try:
            env.ParseConfig('pkg-config --cflags --libs dbus-1')
        except Exception:
            env.Append(LIBS=['dbus-1'])
elif platform in ["macos", "ios"]:
    env.Append(_LIBFLAGS=[rust_lib_path])
    # The iana_time_zone crate (a dependency) requires the CoreFoundation framework on Apple platforms.
    env.Append(LINKFLAGS=['-framework', 'CoreFoundation'])
else:
    # Fallback for other platforms (like web) which might just need the library path.
    # Web builds, for example, don't link in the same way.
    print(f"{Y}Warning: Using default linking for platform {platform}. This may not be correct.{X}")
    env.Append(_LIBFLAGS=[rust_lib_path])

# --- Source File Management ---
# This is a common SCons pitfall: glob() finds files that were generated
# in a previous run, causing them to be added to the build twice.
# We fix this by explicitly filtering out known generated files from the glob result.
all_sources = glob.glob("src/**/*.cpp", recursive=True)
generated_doc_file = "src/gen/doc_data.gen.cpp"
sources = [s for s in all_sources if os.path.normpath(s) != os.path.normpath(generated_doc_file)]
sources = sorted(sources) + ["bindings/controller/controller.cpp", "bindings/dojo/dojo.cpp"]

# Define precision string for suffixes
precision_string = ".double" if precision == "double" else ""

_godot_min = _detect_godot_min_requirement()
_godot_tag = _get_git_submodule_version("external/godot-cpp")
print(f"{Y}{clipboard} Building with {_godot_tag}. Project Version: {project_version}{X}")

# Add documentation (the correct way)
if target in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData(generated_doc_file, source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# Create library
suffix_map = {
    "linux": f".linux.{target}{precision_string}.{arch}.so",
    "windows": f".windows.{target}{precision_string}.{arch}.dll",
    "macos": f".macos.{target}{precision_string}.{arch}.dylib",
    "ios": f".ios.{target}{precision_string}.{arch}.dylib" if not env['ios_simulator'] else f".ios.{target}{precision_string}.simulator.{arch}.dylib",
    "android": f".android.{target}{precision_string}.{arch}.so",
    "web": f".web.{target}{precision_string}.{arch}.wasm"
}

target_out_dir = target

if not target_out_dir == "editor":
    target_out_dir = target.split('_', 1)[1]  # "debug" or "release"

# --- VariantDir Logic ---
# Isolate C++ object files by precision and target to prevent conflicts.
# This removes the need to clean when switching between single/double.
obj_dir = f"build/obj/{target_out_dir}/{precision}"
env.VariantDir(f"{obj_dir}/src", "src", duplicate=0)
env.VariantDir(f"{obj_dir}/bindings", "bindings", duplicate=0)

final_sources = []
for s in sources:
    s_path = str(s)
    if s_path.startswith("src/"):
        final_sources.append(s_path.replace("src/", f"{obj_dir}/src/", 1))
    elif s_path.startswith("bindings/"):
        final_sources.append(s_path.replace("bindings/", f"{obj_dir}/bindings/", 1))
    else:
        final_sources.append(s)

if platform == "ios":
    output_dir = f"build/ios/{target_out_dir}"
else:
    output_dir = f"demo/addons/godot-dojo/bin/{platform}/{target_out_dir}"

# Ensure the output directory exists
os.makedirs(output_dir, exist_ok=True)

lib_name = f"{output_dir}/{prefix}godot-dojo{suffix_map.get(platform, f'.{platform}.{target}{precision_string}.{arch}.so')}"

final_objects = []

# Buscar la librería de godot-cpp para forzar el orden de compilación
godot_cpp_lib_node = None
for lib in env.get("LIBS", []):
    if "godot-cpp" in str(lib):
        # Si es un nodo (tiene get_path) lo usamos, si es string lo buscamos en LIBPATH
        if hasattr(lib, "get_path"):
            godot_cpp_lib_node = lib
        else:
            godot_cpp_lib_node = env.FindFile(str(lib), env.get("LIBPATH", []))
        break

for s in final_sources:
    objs = env.SharedObject(s)
    # Forzar recompilación si cambia la precisión (soluciona "ya existen los archivos")
    env.Depends(objs, env.Value(precision))
    # Depender de la librería de godot-cpp asegura que los headers generados estén listos (soluciona race condition)
    if godot_cpp_lib_node:
        env.Depends(objs, godot_cpp_lib_node)
    final_objects.extend(objs)

library = env.SharedLibrary(target=lib_name, source=final_objects)

env.Alias(f"godot-dojo-{platform}-{target_out_dir}", library)

# Generate .gdextension
with open("plugin_template.gdextension.in", 'r') as f:
    template = f.read()

gdext = template.replace("${PROJECT_NAME}", "godot-dojo")
gdext = gdext.replace("${ENTRY_POINT}", "godotdojo_library_init")


gdext = gdext.replace("${GODOT_MIN_REQUIREMENT}", _godot_min)
gdext = gdext.replace("${VERSION}", project_version)
gdext = gdext.replace("${PRECISION}", precision_string)

print(f"{Y}{clipboard} Generating demo/addons/godot-dojo/godot-dojo.gdextension...{X}")
with open("demo/addons/godot-dojo/godot-dojo.gdextension", 'w') as f:
    f.write(gdext)

def build_complete_callback(target, source, env):
    print(f"{G}{party} Build complete for {str(target[0])}!{X}")

    # Copiar assets
    assets_src = "assets"
    assets_dest = "demo/addons/godot-dojo/assets"
    print(f"{Y}{clipboard} Copying assets to {assets_dest}...{X}")
    if os.path.exists(assets_dest):
        shutil.rmtree(assets_dest)
    shutil.copytree(assets_src, assets_dest)
    print(f"{G}{check} Assets copied successfully.{X}")
    return None


env.AddPostAction(library, build_complete_callback)
env.Default(library)

# --- XCFramework Post-Action Logic ---
def create_xcframework_action(target, source, env):
    """
    Custom builder function to create an XCFramework using xcodebuild.
    'source' is a list of library nodes. This function will identify device and simulator
    libraries, merge simulator libraries into a universal binary if needed, and then
    create the XCFramework.
    """
    xcframework_path = str(target[0])
    source_paths = [str(s) for s in source if os.path.exists(str(s))]

    if not source_paths:
        print(f"{R}{cross} No libraries found to create XCFramework. Aborting.{X}")
        return 1

    device_libs = [p for p in source_paths if "simulator" not in p]
    simulator_libs = [p for p in source_paths if "simulator" in p]

    final_libs = device_libs

    # If we have multiple simulator libraries, merge them with lipo.
    if len(simulator_libs) > 1:
        print(f"{Y}Merging simulator libraries with lipo...{X}")
        # Define a path for the merged universal simulator library
        output_dir = os.path.dirname(simulator_libs[0])
        universal_sim_lib_path = os.path.join(output_dir, "godot-dojo.ios.universal.simulator.dylib")

        # Filter out any pre-existing universal library from the input list to avoid conflicts.
        lipo_inputs = [lib for lib in simulator_libs if "universal" not in os.path.basename(lib)]

        # Ensure the old universal lib is gone before creating a new one.
        if os.path.exists(universal_sim_lib_path):
            os.remove(universal_sim_lib_path)

        lipo_cmd = ["lipo", "-create", "-output", universal_sim_lib_path] + lipo_inputs
        subprocess.run(lipo_cmd, check=True)
        final_libs.append(universal_sim_lib_path)
        print(f"{G}{check} Universal simulator library created at: {universal_sim_lib_path}{X}")
    elif simulator_libs:
        final_libs.extend(simulator_libs)

    cmd = ["xcodebuild", "-create-xcframework"]
    print(f"{Y}{package} Creating XCFramework at {xcframework_path} from:{X}")
    for lib_path in final_libs:
        print(f"{Y}  -> Found library: {lib_path}{X}")
        cmd.extend(["-library", lib_path])

    cmd.extend(["-output", xcframework_path])

    # Ensure the destination directory exists
    os.makedirs(os.path.dirname(xcframework_path), exist_ok=True)

    try:
        subprocess.run(cmd, check=True, text=True, capture_output=False)
        print(f"{G}{check} XCFramework created successfully at: {xcframework_path}{X}")
    except subprocess.CalledProcessError as e:
        print(f"{R}{cross} Failed to create XCFramework.{X}")
        return e.returncode

# Define an alias to build the XCFramework.
# This only runs if `scons platform=ios xcframework` is invoked.
if platform == "ios":
    # We need to know the target ('template_debug' or 'template_release')
    target_out_dir = target.split('_', 1)[1]  # "debug" or "release"

    # Only define the 'assemble-ios' alias and its dependencies if it's actually requested.
    if "assemble-ios" in COMMAND_LINE_TARGETS:
        output_base = f"build/ios/{target_out_dir}"
        is_simulator = env.get('ios_simulator', False)
        # The output path is always the same. The action will handle deletion to force recreation.
        target_dir = f"demo/addons/godot-dojo/bin/ios/{target_out_dir}/godot-dojo{precision_string}.xcframework"
        # Always delete the existing XCFramework to force recreation.
        if os.path.exists(target_dir):
            print(f"{Y}{broom} Deleting existing XCFramework to ensure a clean build: {target_dir}{X}")
            shutil.rmtree(target_dir)

        # Dynamically determine which libraries to build based on command-line arguments.
        # This allows for building a single architecture (e.g., for testing) or all of them (for release).
        libs_to_package = []

        # If 'arch' is not specified or is 'universal', build all libraries.
        if arch == "universal":
            print(f"{Y}Gathering all architectures for iOS XCFramework...{X}")
            libs_to_package.append(f"{output_base}/godot-dojo.ios.{target}{precision_string}.arm64.dylib")
            libs_to_package.append(f"{output_base}/godot-dojo.ios.{target}{precision_string}.simulator.x86_64.dylib")
            libs_to_package.append(f"{output_base}/godot-dojo.ios.{target}{precision_string}.simulator.arm64.dylib")
        else:
            # Build only the specified architecture.
            if is_simulator:
                print(f"{Y}Gathering for iOS Simulator ({arch}) only...{X}")
                libs_to_package.append(f"{output_base}/godot-dojo.ios.{target}{precision_string}.simulator.{arch}.dylib")
            else:
                print(f"{Y}Gathering for iOS Device ({arch}) only...{X}")
                libs_to_package.append(f"{output_base}/godot-dojo.ios.{target}{precision_string}.{arch}.dylib")

        if not libs_to_package:
            print(f"{R}{cross} No valid iOS libraries to build for the specified configuration. Aborting.{X}")
            Exit(1)

        # The action will be called *after* the dependencies are built.
        # It receives the target directory and the list of source files.
        package_action = env.Action(
            lambda target, source, env: create_xcframework_action(target, source, env),
            "Packaging iOS libraries into XCFramework..."
        )

        # Create a target for the final XCFramework directory.
        # This target depends on the individual libraries being built.
        xcframework_package = env.Command(target_dir, libs_to_package, package_action)

        # Create an alias so we can call it from the command line.
        # This alias now depends on the final package.
        env.Alias("assemble-ios", xcframework_package)

        # When 'assemble-ios' is called, we don't want to build the default library for the host.
        # We clear the default target to prevent this.
        env.Default(None)
