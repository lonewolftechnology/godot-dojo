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
    from SCons.Script import GetOption, Return, Exit, File, SConscript  # type: ignore
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

# Colors
G, B, R, Y, X = '\033[92m', '\033[94m', '\033[91m', '\033[1;33m', '\033[0m'

# Check if running on Windows to avoid encoding issues
is_windows = host_platform.system().lower() == "windows"
if is_windows:
    # Use ASCII alternatives on Windows
    rocket = ">"
    broom = "-"
    check = "+"
    package = "#"
    clipboard = "="
    party = "!"
    cross = "x"
else:
    # Use emojis on other platforms
    rocket = "🚀"
    broom = "🧹"
    check = "✅"
    package = "📦"
    clipboard = "📋"
    party = "🎉"
    cross = "❌"

print(f"{B}{rocket} Building godot-dojo{X}")

# Cleanup
if GetOption('clean'):
    print(f"{Y}{broom} Starting cleanup process...{X}")

    # Limpiar proyecto Rust dojo.c
    print(f"{B}  -> Cleaning Rust project: external/dojo.c{X}")
    try:
        subprocess.run(["cargo", "clean"], cwd="external/dojo.c", check=True, capture_output=False, text=True)
        print(f"{G}     {check} dojo.c cleanup complete.{X}")
    except subprocess.CalledProcessError as e:
        print(f"{R}     {cross} dojo.c cleanup failed: {e.stderr}{X}")

    # Limpiar proyecto Rust controller.c
    print(f"{B}  -> Cleaning Rust project: external/controller.c{X}")
    try:
        subprocess.run(["cargo", "clean"], cwd="external/controller.c", check=True, capture_output=False, text=True)
        print(f"{G}     {check} controller.c cleanup complete.{X}")
    except subprocess.CalledProcessError as e:
        print(f"{R}     {cross} controller.c cleanup failed: {e.stderr}{X}")

    # Eliminar directorio del addon
    print(f"{B}  -> Deleting addon directory: demo/addons/godot-dojo{X}")
    shutil.rmtree("demo/addons/godot-dojo", ignore_errors=True)
    print(f"{G}     {check} Addon directory deleted.{X}")

    # Eliminar directorio de distribución
    print(f"{B}  -> Deleting distribution directory: demo/dist{X}")
    shutil.rmtree("demo/dist", ignore_errors=True)
    print(f"{G}     {check} Distribution directory deleted.{X}")

    # Limpiar submódulo godot-cpp
    print(f"{B}  -> Cleaning godot-cpp submodule...{X}")
    subprocess.run(["scons", "-C", "external/godot-cpp", "--clean"], check=False, capture_output=False)
    print(f"{G}     {check} godot-cpp cleanup complete.{X}")

    print(f"\n{G}{check} Cleanup process finished.{X}")
    Return()

# Setup
os.makedirs("demo/addons/godot-dojo", exist_ok=True)

# Initialize the godot-cpp build environment
env = SConscript("external/godot-cpp/SConstruct")
platform, arch, target = env["platform"], env["arch"], env.get("target", "template_debug")

build_info = f"{platform} ({arch}) - {target}"
if env.get("precision") == "double":
    build_info += " (double precision)"

print(f"{B}Building: {build_info}{X}")

# Compile Rust
# Se usa más adelante, pero se importa aquí para que esté disponible
import re
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
    ("ios", "arm64"): "aarch64-apple-ios",
}
# Use CARGO_BUILD_TARGET from environment if set, otherwise use the target from the platform/arch
rust_target = os.environ.get("CARGO_BUILD_TARGET", targets.get((platform, arch), "x86_64-unknown-linux-gnu"))

# Override for macOS x86_64 to ensure it uses the correct target
if platform == "macos" and arch == "x86_64":
    rust_target = "x86_64-apple-darwin"
elif platform == "macos" and arch == "arm64":
    rust_target = "aarch64-apple-darwin"

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
        subprocess.run(["rustup", "target", "add", rt], check=False, cwd="external/dojo.c", capture_output=False)
        subprocess.run(["rustup", "target", "add", rt], check=False, cwd="external/controller.c", capture_output=False)

except subprocess.CalledProcessError as e:
    print(f"{R}{cross} Failed to check or install Rust target: {e}{X}")
    # Continue anyway, as cargo will show a more specific error if needed


def apply_patches():
    """Checks for and applies all .patch files found in the 'patches' directory."""
    patches_dir = 'patches'
    if not os.path.isdir(patches_dir):
        print(f"{B}Patch directory '{patches_dir}' not found, skipping.{X}")
        return

    patch_files = sorted(glob.glob(os.path.join(patches_dir, '*.patch')))
    if not patch_files:
        print(f"{B}No .patch files found in '{patches_dir}', skipping.{X}")
        return

    print(f"{Y}{clipboard} Applying patches from '{patches_dir}' directory...{X}")

    # Verificar si el comando 'patch' está disponible.
    if not shutil.which("patch"):
        print(f"{R}{cross} Error: The 'patch' command was not found in your system's PATH.{X}")
        print(f"{Y}This is required to apply a necessary fix to a dependency.{X}")
        print(f"{B}On Windows, the easiest way to get it is by installing 'Git for Windows':{X}")
        print(f"{G}https://git-scm.com/download/win{X}")
        print(f"{Y}Please install it and make sure its tools are added to your PATH, then try again.{X}")
        Exit(1) # Exit if patch command is not available.

    for patch_file in patch_files:
        print(f"{Y}Applying patch: {os.path.basename(patch_file)}...{X}")
        try:
            subprocess.run(['patch', '-p1', '-i', os.path.abspath(patch_file)], check=True)
            print(f"{G}{check} Patch {os.path.basename(patch_file)} applied successfully.{X}")
        except subprocess.CalledProcessError as e:
            print(f"{R}{cross} Failed to apply patch {os.path.basename(patch_file)}: {e}{X}")
            Exit(1)

def _compile_rust_library(lib_name, lib_path, is_release, cargo_flags=None, rustc_flags=None):
    """Compiles a Rust library, handling normal, universal, and web builds."""
    lib_version = _get_git_submodule_version(lib_path) or "unknown"
    print(f"{Y}{package} Compiling {lib_name} ({lib_version})...{X}")

    build_mode = "release" if is_release else "debug"
    base_cmd = ["cargo", "build", "--target", rust_target]

    # Prepare environment variables
    env_vars = os.environ.copy()
    current_rustflags = env_vars.get("RUSTFLAGS", "")
    if rustc_flags:
        current_rustflags += " " + " ".join(rustc_flags)
    env_vars["RUSTFLAGS"] = current_rustflags.strip()

    if is_release:
        base_cmd.append("--release")
    if cargo_flags:
        base_cmd.extend(cargo_flags)

    # Special handling for macOS universal builds
    if platform == "macos" and arch == "universal":
        print(f"{Y}Starting universal build for {lib_name}...{X}")
        rust_targets_for_lipo = targets.get((platform, arch))
        libs_to_lipo = []

        for rt in rust_targets_for_lipo:
            print(f"{Y}Compiling {lib_name} for target: {rt}...{X}")
            cargo_cmd = ["cargo", "build", "--target", rt]
            if is_release:
                cargo_cmd.append("--release")

            env_vars["MACOSX_DEPLOYMENT_TARGET"] = "14.0"
            rustflags = env_vars.get("RUSTFLAGS", "")
            if rustflags: rustflags += " "
            rustflags += "-C link-arg=-mmacosx-version-min=14.0"
            env_vars["RUSTFLAGS"] = rustflags

            subprocess.run(cargo_cmd, check=True, cwd=lib_path, env=env_vars)
            libs_to_lipo.append(f"{lib_path}/target/{rt}/{build_mode}/lib{lib_name}.a")

        print(f"{Y}Creating universal library for {lib_name} with lipo...{X}")
        universal_dir = f"{lib_path}/target/universal/{build_mode}"
        os.makedirs(universal_dir, exist_ok=True)
        universal_lib_path = f"{universal_dir}/lib{lib_name}.a"

        lipo_cmd = ["lipo", "-create"] + libs_to_lipo + ["-output", universal_lib_path]
        subprocess.run(lipo_cmd, check=True)
        print(f"{G}{check} Universal library for {lib_name} created at: {universal_lib_path}{X}")
        return # Handled

    # Standard build for other platforms
    if platform == "macos":
        print(f"{Y}Setting macOS deployment target to 14.0 for {lib_name}...{X}")
        env_vars["MACOSX_DEPLOYMENT_TARGET"] = "14.0"
        rustflags = env_vars.get("RUSTFLAGS", "")
        if rustflags: rustflags += " "
        rustflags += "-C link-arg=-mmacosx-version-min=14.0"
        env_vars["RUSTFLAGS"] = rustflags

    print(f"{Y}Running cargo command for {lib_name}: {' '.join(base_cmd)}{X}")
    subprocess.run(base_cmd, check=True, cwd=lib_path, env=env_vars)

def _regenerate_uniffi_bindings():
    """
    Regenerates the UniFFI C++ bindings for controller.c by running the
    dedicated Rust binary for this task.
    """
    print(f"{Y}{clipboard} Regenerating UniFFI bindings for controller.c...{X}")

    # Check if uniffi-bindgen-cpp is installed. If not, install it from the specified repo.
    if not shutil.which("uniffi-bindgen-cpp"):
        print(f"{Y}Command 'uniffi-bindgen-cpp' not found. Installing from git...{X}")
        install_cmd = [
            "cargo", "install", "uniffi-bindgen-cpp",
            "--git", "https://github.com/Larkooo/uniffi-bindgen-cpp",
            "--branch", "update-0.30"
        ]
        try:
            subprocess.run(install_cmd, check=True)
            print(f"{G}{check} 'uniffi-bindgen-cpp' installed successfully.{X}")
        except subprocess.CalledProcessError as e:
            print(f"{R}{cross} Failed to install 'uniffi-bindgen-cpp': {e}{X}")
            Exit(1)

    bridge_crate_path = "external/controller.c/crates/bridge"
    try:
        cmd = ["cargo", "run", "--bin", "uniffi-bindgen-cpp"]
        # We use capture_output to show stdout/stderr only on failure.
        subprocess.run(cmd, cwd=bridge_crate_path, check=True, capture_output=False, text=True)
        print(f"{G}{check} UniFFI C++ bindings regenerated successfully.{X}")
    except subprocess.CalledProcessError as e:
        print(f"{R}{cross} Failed to regenerate UniFFI bindings:{X}")
        print(f"{R}STDOUT:\n{e.stdout}{X}")
        print(f"{R}STDERR:\n{e.stderr}{X}")
        Exit(1)

is_release_build = target == "template_release"

# Environment variables for WebAssembly
if env["platform"] == "web":
    # For the web build, we need to do two things:
    # 1. Build the dojo.c library with wasm-bindgen to create the JS interface (only for dojo.c).
    #    This build must NOT use `-C relocation-model=pic`.
    # 2. Build the dojo.c and controller.c libraries as rlib with `-C relocation-model=pic`
    #    so scons can link it into the final GDExtension .wasm file.

    # Step 1: Build for wasm-bindgen
    print(f"{Y}Building dojo.c for wasm-bindgen (step 1/3)...{X}")
    cmd_bindgen = ["cargo", "build", "--target", rust_target]
    if is_release_build:
        cmd_bindgen.append("--release")

    bindgen_env = os.environ.copy()
    bindgen_env["RUSTFLAGS"] = "-C target-feature=+atomics,+bulk-memory,+mutable-globals"
    subprocess.run(cmd_bindgen, check=True, cwd="external/dojo.c", env=bindgen_env)

    # Apply patch immediately after Rust compilation
    # apply_patches() # Patches are applied later for web

    # Step 2: Run wasm-bindgen
    print(f"{Y}Running wasm-bindgen...{X}")
    build_mode = "release" if is_release_build else "debug"
    wasm_input_path = f"external/dojo.c/target/{rust_target}/{build_mode}/dojo_c.wasm"
    out_dir = "demo/addons/godot-dojo/"
    os.makedirs(out_dir, exist_ok=True)
    out_name = f"dojo_c_{build_mode}"

    bindgen_cmd = [
        "wasm-bindgen", wasm_input_path,
        "--out-dir", out_dir,
        "--out-name", out_name,
        "--target", "web",
        "--no-typescript"
    ]
    try:
        subprocess.run(bindgen_cmd, check=True)
        print(f"{G}{check} wasm-bindgen finished. Output in {out_dir}{X}")
    except subprocess.CalledProcessError as e:
        print(
            f"{R}{cross} wasm-bindgen failed. This might be due to incompatible Rust flags or wasm-bindgen version.{X}")
        print(f"{R}Error: {e}{X}")
        Exit(1)

    # Step 3: Build dojo.c and controller.c as rlib for SCons linking
    print(f"{Y}Building libraries as rlib for SCons linking...{X}")
    rlib_flags = ["-C", "relocation-model=pic", "-C", "target-feature=+atomics,+bulk-memory,+mutable-globals"]
    _compile_rust_library("dojo_c", "external/dojo.c", is_release_build, rustc_flags=rlib_flags)
    _compile_rust_library("controller_uniffi", "external/controller.c", is_release_build, cargo_flags=["-p", "controller-uniffi"], rustc_flags=rlib_flags)
else:
    # Standard compilation for all non-web platforms
    _compile_rust_library("dojo_c", "external/dojo.c", is_release_build)
    _compile_rust_library("controller_uniffi", "external/controller.c", is_release_build, cargo_flags=["-p", "controller-uniffi"])

apply_patches()

_regenerate_uniffi_bindings()

# Configure library
if platform != "android":
    # Android build requires lib prefix
    env['SHLIBPREFIX'] = ''
prefix = env.subst('$SHLIBPREFIX')
env.Append(CPPPATH=["src/", "include/", "external/dojo.c", "external/controller.c/bindings/cpp", "external/boost/include"])
if platform == "macos":
    # Set macOS deployment target for C++ compilation
    print(f"{Y}Setting macOS deployment target for C++ compilation to 14.0...{X}")
    env['ENV']['MACOSX_DEPLOYMENT_TARGET'] = '14.0'
    env.Append(CCFLAGS=['-mmacosx-version-min=14.0'])
    env.Append(LINKFLAGS=['-mmacosx-version-min=14.0'])

# Enable C++ exceptions for try/catch blocks
if env["platform"] == "windows" and not env.get("use_mingw"):
    # For MSVC
    env.Append(CXXFLAGS=['/EHsc', '/std:c++20'])
else:
    # For GCC/Clang
    env.Append(CXXFLAGS=['-fexceptions', '-std=c++20', '-Wno-template-id-cdtor'])

# List to hold the targets for the Android plugin files.
android_plugin_targets = []

# --- Android Plugin Setup ---
# This section runs only when building for Android. It prepares the Kotlin
# plugin files and places them in the structure Godot's build system expects.
if platform == "android":
    print(f"{Y}{package} Building Android plugin with Gradle...{X}")
    gradlew_cmd = "gradlew.bat" if is_host_windows else "./gradlew"
    try:
        # Execute the gradle build command in the 'android' directory.
        # We run both assembleDebug and assembleRelease to get both AARs.
        subprocess.run([gradlew_cmd, ":plugin:assembleDebug", ":plugin:assembleRelease"], cwd="android", check=True)
        print(f"{G}{check} Android plugin built successfully.{X}")

        # Copy the generated AAR files to the correct addon location
        print(f"{Y}{clipboard} Copying Android plugin artifacts...{X}")
        plugin_name = "GodotDojoAndroidPlugin" # Must match pluginName in build.gradle.kts
        aar_source_dir = f"android/plugin/build/outputs/aar/"
        
        # Copy Debug AAR
        debug_aar_src = f"{aar_source_dir}/{plugin_name}-debug.aar"
        debug_aar_dest_dir = "demo/addons/godot-dojo/bin/android/debug"
        os.makedirs(debug_aar_dest_dir, exist_ok=True)
        shutil.copy(debug_aar_src, debug_aar_dest_dir)

        # Copy Release AAR
        release_aar_src = f"{aar_source_dir}/{plugin_name}-release.aar"
        release_aar_dest_dir = "demo/addons/godot-dojo/bin/android/release"
        os.makedirs(release_aar_dest_dir, exist_ok=True)
        shutil.copy(release_aar_src, release_aar_dest_dir)

        print(f"{G}{check} Android artifacts copied to demo/addons/godot-dojo/bin/android/{X}")
    except subprocess.CalledProcessError as e:
        print(f"{R}{cross} Gradle build failed: {e}{X}")
        Exit(1)


# Link Rust libraries
build_mode = "release" if target == "template_release" else "debug"

# For macOS universal, rust_target was modified, so we handle it specially
rust_lib_target_dir = "universal" if platform == "macos" and arch == "universal" else rust_target

rust_lib_dir_dojo = f"external/dojo.c/target/{rust_lib_target_dir}/{build_mode}"
rust_lib_dir_controller = f"external/controller.c/target/{rust_lib_target_dir}/{build_mode}"

rust_lib_dojo = ""
rust_lib_controller = ""

if platform == "windows":
    if use_mingw:
        # For MinGW, we link against the static .a library
        rust_lib_dojo = f"{rust_lib_dir_dojo}/libdojo_c.a"
        rust_lib_controller = f"{rust_lib_dir_controller}/libcontroller_uniffi.a"
    elif is_host_windows:
        # For MSVC, we link against the static .lib library
        rust_lib_dojo = f"{rust_lib_dir_dojo}/dojo_c.lib"
        rust_lib_controller = f"{rust_lib_dir_controller}/controller_uniffi.lib"
        env.Append(LINKFLAGS=['/NODEFAULTLIB:MSVCRT'])
    env.Append(LIBS=['ws2_32', 'advapi32', 'ntdll'])
elif platform == "linux":
    rust_lib_dojo = f"{rust_lib_dir_dojo}/libdojo_c.a"
    rust_lib_controller = f"{rust_lib_dir_controller}/libcontroller_uniffi.a"
    # Use pkg-config to add proper include/lib flags for dbus-1 and ensure correct link order.
    # It's important to add this *after* our own library which depends on it.
    try:
        env.ParseConfig('pkg-config --cflags --libs dbus-1')
    except Exception:
        env.Append(LIBS=['dbus-1'])
elif platform == "web":
    print(f"{Y}{clipboard} Web export links to rlib files.{X}")
    rust_lib_dojo = f"{rust_lib_dir_dojo}/libdojo_c.rlib"
    rust_lib_controller = f"{rust_lib_dir_controller}/libcontroller_uniffi.rlib"
else:
    # Default for other platforms like macos (non-universal), android, ios
    rust_lib_dojo = f"{rust_lib_dir_dojo}/libdojo_c.a"
    rust_lib_controller = f"{rust_lib_dir_controller}/libcontroller_uniffi.a"

if rust_lib_dojo and rust_lib_controller:
    # Link our libraries first, then the system libraries they depend on.
    env.Append(LIBS=[File(rust_lib_dojo), File(rust_lib_controller)])
else:
    print(f"{R}{cross} Could not determine Rust library paths for platform {platform}{X}")
    Exit(1)

if platform != "web" and (not os.path.exists(rust_lib_dojo) or not os.path.exists(rust_lib_controller)):
    if not os.path.exists(rust_lib_dojo): print(f"{R}{cross} Rust library not found: {rust_lib_dojo}{X}")
    if not os.path.exists(rust_lib_controller): print(f"{R}{cross} Rust library not found: {rust_lib_controller}{X}")
    Exit(1)

sources = sorted(glob.glob("src/**/*.cpp", recursive=True)) + [
    "external/controller.c/bindings/cpp/controller.cpp",
]
_godot_min = _detect_godot_min_requirement()
_godot_tag = _get_git_submodule_version("external/godot-cpp")
print(f"{Y}{clipboard} Building with {_godot_tag}.{X}")
# Add documentation
if target in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# Create library
suffix_map = {
    "linux": f".linux.{target}.{arch}.so",
    "windows": f".windows.{target}.{arch}.dll",
    "macos": f".macos.{target}.{arch}.dylib",
    "ios": f".ios.{target}.{arch}.dylib",
    "web": f".web.{target}.wasm32.wasm"
}

output_dir = f"demo/addons/godot-dojo/bin/{platform}/{build_mode}"
lib_name = f"{output_dir}/{prefix}godot-dojo{suffix_map.get(platform, f'.{platform}.{target}.{arch}.so')}"
library = env.SharedLibrary(target=lib_name, source=sources)
env.Alias(f"godot-dojo-{platform}-{build_mode}", library)

# Generate .gdextension
with open("plugin_template.gdextension.in", 'r') as f:
    template = f.read()

gdext = template.replace("${PROJECT_NAME}", "godot-dojo")
gdext = gdext.replace("${ENTRY_POINT}", "dojoc_library_init")


gdext = gdext.replace("${GODOT_MIN_REQUIREMENT}", _godot_min)

with open("demo/addons/godot-dojo/godot-dojo.gdextension", 'w') as f:
    f.write(gdext)


def copy_web_artifacts(target, source, env):
    print(f"{Y}{clipboard} Preparing web export files...{X}")
    build_mode = "release" if env.get("target", "template_debug") == "template_release" else "debug"

    addon_dir = "demo/addons/godot-dojo"
    out_name = f"dojo_c_{build_mode}"

    js_path = f"{addon_dir}/{out_name}.js"
    wasm_bg_path = f"{addon_dir}/{out_name}_bg.wasm"
    wasm_final_path = f"{addon_dir}/{out_name}.wasm"

    # 1. Rename the wasm file (e.g., dojo_c_release_bg.wasm -> dojo_c_release.wasm)
    print(f"{Y}Renaming {os.path.basename(wasm_bg_path)} to {os.path.basename(wasm_final_path)}...{X}")
    shutil.move(wasm_bg_path, wasm_final_path)

    # 2. Patch the JS file to load the renamed wasm file
    print(f"{Y}Patching {os.path.basename(js_path)} to load the new wasm file...{X}")
    with open(js_path, 'r+') as f:
        content = f.read()
        # Replace the old wasm filename with the new one
        content = content.replace(f"'{out_name}_bg.wasm'", f"'{out_name}.wasm'")
        f.seek(0)
        f.write(content)
        f.truncate()

    print(f"{G}{check} Web artifacts prepared in {addon_dir}{X}")

    # Copy the custom HTML template to the addon directory
    template_html_src = "web/godot-template.html"
    shutil.copy(template_html_src, f"{addon_dir}/godot-template.html")
    print(f"{G}{check} Custom HTML template copied to {addon_dir}{X}")

    print(f"")
    print(f"{Y}********************************************************************************{X}")
    print(f"{Y}** {clipboard} ACTION REQUIRED: Configure Godot Web Export                        **{X}")
    print(f"{Y}********************************************************************************{X}")
    print(f"{B}To complete the web export, you MUST configure the Godot export preset:{X}")
    print(f"{B}1. Go to {G}Project -> Export...{X} and select the {G}Web{X} preset.{X}")
    print(f"{B}2. In the options, find {G}Shell -> Custom HTML Shell{X}.{X}")
    print(f"{B}3. Select the file: {G}res://addons/godot-dojo/godot-template.html{X}{X}")
    print(f"{Y}********************************************************************************{X}")


def build_complete_callback(target, source, env):
    print(f"{G}{party} Build complete!{X}")
    if env["platform"] == "web":
        copy_web_artifacts(target, source, env)
    return None


env.AddPostAction(library, build_complete_callback)

env.Default(library)
