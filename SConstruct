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
        subprocess.run(["rustup", "target", "add", rt], check=False, cwd="godot-dojo-core", capture_output=False)

except subprocess.CalledProcessError as e:
    print(f"{R}{cross} Failed to check or install Rust target: {e}{X}")
    # Continue anyway, as cargo will show a more specific error if needed

def apply_patches():
    # Checks for and applies all .patch files found in the 'patches' directory
    patches_dir = "patches"
    if not os.path.isdir(patches_dir):
        print(f"{B}Patch directory '{patches_dir}' not found, skipping.{X}")
        return

    patch_files = sorted(glob.glob(os.path.join(patches_dir, '*.patch')))
    if not patch_files:
        print(f"{B}No .patch files found in '{patches_dir}', skipping.{X}")
        return

    print(f"{Y}{clipboard} Applying patches from '{patches_dir}' directory...{X}")

    # Check if 'patch' is available.
    if not shutil.which("patch"):
        print(f"{R}{cross} Error: The 'patch' command was not found in your system's PATH.{X}")
        print(f"{Y}This is required to apply a necessary fix to a dependency.{X}")
        print(f"{B}On Windows, the easiest way to get it is by installing 'Git for Windows':{X}")
        print(f"{G}https://git-scm.com/download/win{X}")
        print(f"{Y}Please install it and make sure its tools are added to your PATH, then try again.{X}")
        Exit(1) # Exit if patch command is not available.

    for patch_file in patch_files:
        print(f"{Y}Applying patch: {os.path.basename(patch_file)}...{X}")
        result = subprocess.run(
            ['patch', '-p1', '-N', '--dry-run', '-i', os.path.abspath(patch_file)],
            capture_output=True, text=True
        )

        already_applied = (
            "previously applied" in result.stdout or
            "hunk ignored" in result.stdout or
            "hunk ignored" in result.stderr
        )

        if already_applied:
            print(f"{G}{check} Patch {os.path.basename(patch_file)} already applied, skipping.{X}")
            continue

        if result.returncode == 0:
            apply_result = subprocess.run(['patch', '-p1', '-N', '-i', os.path.abspath(patch_file)], check=False, capture_output=True, text=True)
            print(f"{G}{check} Patch {os.path.basename(patch_file)} applied successfully.{X}")
        else:
            print(f"{R}{cross} Failed to apply patch {os.path.basename(patch_file)}.{X}")
            if result.stdout:
                print(f"{B}--- stdout ---\n{result.stdout}{X}")
            if result.stderr:
                print(f"{R}--- stderr ---\n{result.stderr}{X}")
            Exit(1)

def _compile_rust_library(lib_name, lib_path, is_release, cargo_flags=None, rustc_flags=None):
    """Compiles a Rust library, handling normal, universal, and web builds."""
    lib_version = _get_git_submodule_version(lib_path) or "unknown"
    print(f"{Y}{package} Compiling {lib_name} ({lib_version})...{X}")

    build_mode = "release" if is_release else "debug"
    base_cmd = ["cargo", "build"]

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

    # Always specify the target unless it's a universal macOS build (handled separately)
    if not (platform == "macos" and arch == "universal"):
        base_cmd.extend(["--target", rust_target])

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

# Determine if we should use the release build of the Rust crate.
# This is true if the SCons target is 'template_release' OR if we force it via an environment variable.
force_rust_release = os.environ.get("FORCE_RUST_RELEASE", "0") == "1"
is_release_build = target == "template_release" or force_rust_release

_compile_rust_library("godot_dojo_core", "godot-dojo-core", is_release_build)

apply_patches()
# Configure library
env['SHLIBPREFIX'] = ''
prefix = env.subst('$SHLIBPREFIX')
env.Append(CPPPATH=["src/", "include/", "bindings/", "external/boost/include"])
if platform == "macos":
    # Set macOS deployment target for C++ compilation
    print(f"{Y}Setting macOS deployment target for C++ compilation to 14.0...{X}")
    env['ENV']['MACOSX_DEPLOYMENT_TARGET'] = '14.0'
    env.Append(CCFLAGS=['-mmacosx-version-min=14.0'])
    env.Append(LINKFLAGS=['-mmacosx-version-min=14.0'])

# Enable C++ exceptions for try/catch blocks
if env["platform"] == "windows" and not env.get("use_mingw"):
    # For MSVC
    # Use /std:c++17 to avoid issues with std::uniform_int_distribution
    # and char types in C++20.
    env.Append(CXXFLAGS=['/EHsc', '/std:c++17'])
else:
    # For GCC/Clang
    # Using C++17 for consistency and to avoid breaking changes from C++20.
    # -Wno-template-id-cdtor suppresses warnings from godot-cpp templates.
    env.Append(CXXFLAGS=['-fexceptions', '-std=c++17', '-Wno-template-id-cdtor'])

# Link Rust libraries
rust_build_mode = "release" if is_release_build else "debug"

# For macOS universal, rust_target was modified, so we handle it specially
rust_lib_target_dir = "universal" if platform == "macos" and arch == "universal" else rust_target

rust_lib_dir = f"godot-dojo-core/target/{rust_lib_target_dir}/{rust_build_mode}"

rust_lib = ""

if platform == "windows":
    if use_mingw:
        # For MinGW, we link against the static .a library
        rust_lib = f"{rust_lib_dir}/libgodot_dojo_core.a"
    elif is_host_windows:
        # For MSVC, we link against the static .lib library
        rust_lib = f"{rust_lib_dir}/godot_dojo_core.lib"
        env.Append(LINKFLAGS=['/NODEFAULTLIB:MSVCRT'])
    env.Append(LIBS=['ws2_32', 'advapi32', 'ntdll'])
elif platform == "linux":
    rust_lib = f"{rust_lib_dir}/libgodot_dojo_core.a"
    # Use pkg-config to add proper include/lib flags for dbus-1 and ensure correct link order.
    # It's important to add this *after* our own library which depends on it.
    try:
        env.ParseConfig('pkg-config --cflags --libs dbus-1')
    except Exception: # Fallback if pkg-config is not available
        env.Append(LIBS=['dbus-1'])
else:
    # Default for other platforms like macos (non-universal), android, ios
    rust_lib = f"{rust_lib_dir}/libgodot_dojo_core.a"

if rust_lib:
    # For static libraries, we need to tell the linker to include all symbols,
    # as external C++ code (like the UniFFI bindings) will depend on them.
    # This solves "undefined symbol" errors for symbols inside the Rust static lib.
    if platform in ["linux", "macos", "ios"] or use_mingw:
        # For GCC/Clang based toolchains
        # Use --start-group and --end-group to correctly link symbols from the static Rust
        # library. This is more robust than --whole-archive, which can cause "multiple
        # definition" errors if the static library contains duplicate internal symbols
        # (e.g., from vendored dependencies like `ring`).
        env.Append(LINKFLAGS=["-Wl,--start-group", File(rust_lib), "-Wl,--end-group"])
    elif platform == "windows" and not use_mingw:
        # For MSVC
        env.Append(LINKFLAGS=[f"/WHOLEARCHIVE:{os.path.basename(rust_lib)}"])
        env.Append(LIBPATH=[os.path.dirname(rust_lib)])
else:
    print(f"{R}{cross} Could not determine Rust library paths for platform {platform}{X}")
    Exit(1)
if not os.path.exists(rust_lib):
    print(f"{R}{cross} Rust library not found: {rust_lib}{X}")
    Exit(1)

sources = sorted(glob.glob("src/**/*.cpp", recursive=True)) + [
    "bindings/controller/controller.cpp",
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
}

target_out_dir = target
if '_' in target:
    target_out_dir = target.split('_', 1)[1]

output_dir = f"demo/addons/godot-dojo/bin/{platform}/{target_out_dir}"
lib_name = f"{output_dir}/{prefix}godot-dojo{suffix_map.get(platform, f'.{platform}.{target}.{arch}.so')}"
library = env.SharedLibrary(target=lib_name, source=sources)
env.Alias(f"godot-dojo-{platform}-{target_out_dir}", library)

# Generate .gdextension
with open("plugin_template.gdextension.in", 'r') as f:
    template = f.read()

gdext = template.replace("${PROJECT_NAME}", "godot-dojo")
gdext = gdext.replace("${ENTRY_POINT}", "dojoc_library_init")


gdext = gdext.replace("${GODOT_MIN_REQUIREMENT}", _godot_min)

with open("demo/addons/godot-dojo/godot-dojo.gdextension", 'w') as f:
    f.write(gdext)


def build_complete_callback(target, source, env):
    print(f"{G}{party} Build complete!{X}")
    return None


env.AddPostAction(library, build_complete_callback)

env.Default(library)