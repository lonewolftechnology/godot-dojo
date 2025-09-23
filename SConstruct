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
    print(f"{Y}{broom} Cleaning...{X}")
    try:
        subprocess.run(["cargo", "clean"], cwd="external/dojo.c", check=True)
        shutil.rmtree("demo/addons/godot-dojo", ignore_errors=True)
        shutil.rmtree("demo/dist", ignore_errors=True)
        subprocess.run(["scons", "-C", "external/godot-cpp", "--clean"], check=False)
    except:
        pass
    print(f"{G}{check} Cleanup complete{X}")
    Return()

# Setup
os.makedirs("demo/addons/godot-dojo", exist_ok=True)

# Initialize the godot-cpp build environment
env = SConscript("external/godot-cpp/SConstruct")
platform, arch, target = env["platform"], env["arch"], env.get("target", "template_debug")

print(f"{B}Building: {platform} ({arch}) - {target}{X}")

# Compile Rust
print(f"{Y}{package} Compiling dojo.c...{X}")

# Detect host configuration and toolchain
is_host_windows = host_platform.system().lower() == "windows"
use_mingw = env.get("use_mingw", False)

targets = {
    ("windows", "x86_64"): "x86_64-pc-windows-gnu" if (not is_host_windows or use_mingw) else "x86_64-pc-windows-msvc",
    ("linux", "x86_64"): "x86_64-unknown-linux-gnu",
    ("linux", "arm64"): "aarch64-unknown-linux-gnu",
    ("macos", "x86_64"): "x86_64-apple-darwin",
    ("macos", "arm64"): "aarch64-apple-darwin",
    ("web", "wasm32"): "wasm32-unknown-unknown",
    ("android", "arm64"): "aarch64-linux-android"
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
    print(f"{Y}Checking if Rust target {rust_target} is installed...{X}")
    result = subprocess.run(["rustup", "target", "list", "--installed"], capture_output=True, text=True, check=True)
    installed_targets = result.stdout.splitlines()

    if rust_target not in installed_targets:
        print(f"{Y}Installing Rust target {rust_target}...{X}")
        # Run rustup target add in the main directory
        subprocess.run(["rustup", "target", "add", rust_target], check=True)
        # Also run rustup target add in the external/dojo.c directory
        print(f"{Y}Installing Rust target {rust_target} in external/dojo.c directory...{X}")
        subprocess.run(["rustup", "target", "add", rust_target], check=True, cwd="external/dojo.c")
        print(f"{G}{check} Rust target {rust_target} installed{X}")
    else:
        print(f"{G}{check} Rust target {rust_target} is already installed{X}")
        # Even if the target is already installed globally, ensure it's also installed in the external/dojo.c directory
        print(f"{Y}Ensuring Rust target {rust_target} is installed in external/dojo.c directory...{X}")
        subprocess.run(["rustup", "target", "add", rust_target], check=False, cwd="external/dojo.c")
except subprocess.CalledProcessError as e:
    print(f"{R}{cross} Failed to check or install Rust target: {e}{X}")
    # Continue anyway, as cargo will show a more specific error if needed

cmd = ["cargo", "build", "--target", rust_target]
if target == "template_release":
    cmd.append("--release")

# Environment variables for WebAssembly
if env["platform"] == "web":
    # For the web build, we need to do two things:
    # 1. Build the dojo.c library with wasm-bindgen to create the JS interface.
    #    This build must NOT use `-C relocation-model=pic`.
    # 2. Build the dojo.c library as an rlib with `-C relocation-model=pic`
    #    so scons can link it into the final GDExtension .wasm file.

    # Step 1: Build for wasm-bindgen
    print(f"{Y}Building dojo.c for wasm-bindgen...{X}")
    bindgen_env = os.environ.copy()
    bindgen_env["RUSTFLAGS"] = "-C target-feature=+atomics,+bulk-memory,+mutable-globals"
    subprocess.run(cmd, check=True, cwd="external/dojo.c", env=bindgen_env)

    # Step 2: Run wasm-bindgen
    print(f"{Y}Running wasm-bindgen...{X}")
    build_mode = "release" if target == "template_release" else "debug"
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
        print(f"{R}{cross} wasm-bindgen failed. This might be due to incompatible Rust flags or wasm-bindgen version.{X}")
        print(f"{R}Error: {e}{X}")
        Exit(1)
else:
    env_vars = os.environ.copy()
    # Explicitly set RUSTUP_TOOLCHAIN to ensure the correct toolchain is used
    if platform == "macos":
        # Set macOS deployment target to 14.0 to ensure compatibility
        print(f"{Y}Setting macOS deployment target to 14.0...{X}")
        env_vars["MACOSX_DEPLOYMENT_TARGET"] = "14.0"

        # Add deployment target to RUSTFLAGS
        rustflags = env_vars.get("RUSTFLAGS", "")
        if rustflags: # Add a space if RUSTFLAGS is not empty
            rustflags += " "
        rustflags += "-C link-arg=-mmacosx-version-min=14.0"
        env_vars["RUSTFLAGS"] = rustflags

    # Print the command and environment for debugging
    print(f"{Y}Running cargo command: {' '.join(cmd)}{X}")
    print(f"{Y}With target: {rust_target}{X}")
    if platform == "macos":
        print(f"{Y}With MACOSX_DEPLOYMENT_TARGET: {env_vars.get('MACOSX_DEPLOYMENT_TARGET', 'not set')}{X}")
        print(f"{Y}With RUSTFLAGS: {env_vars.get('RUSTFLAGS', 'not set')}{X}")

    subprocess.run(cmd, check=True, cwd="external/dojo.c", env=env_vars)

# Configure library
if platform != "android":
    # Android build requires lib prefix
    env['SHLIBPREFIX'] = ''
prefix = env.subst('$SHLIBPREFIX')
env.Append(CPPPATH=["src/", "include/", "external/dojo.c", "external/boost/include"])

if platform == "linux":
    # Use pkg-config to add proper include/lib flags for dbus-1 and ensure correct link order
    try:
        env.ParseConfig('pkg-config --cflags --libs dbus-1')
    except Exception:
        # Fallback: at least attempt to link dbus-1 if pkg-config is unavailable
        env.Append(LIBS=['dbus-1'])
elif platform == "macos":
    # Set macOS deployment target for C++ compilation
    print(f"{Y}Setting macOS deployment target for C++ compilation to 14.0...{X}")
    env['ENV']['MACOSX_DEPLOYMENT_TARGET'] = '14.0'
    env.Append(CCFLAGS=['-mmacosx-version-min=14.0'])
    env.Append(LINKFLAGS=['-mmacosx-version-min=14.0'])

# List to hold the targets for the Android plugin files.
android_plugin_targets = []

# --- Android Plugin Setup ---
# This section runs only when building for Android. It prepares the Kotlin
# plugin files and places them in the structure Godot's build system expects.
if platform == "android":
    print(f"{Y}{package} Preparing Android plugin files...{X}")
    addon_dir = "demo/addons/godot-dojo"
    # The .gdap file must be in the addon's root directory.
    # The rest of the plugin files (sources, manifest) go into an 'android' subdirectory.
    android_plugin_source_dir = f"{addon_dir}/android"
    android_src_root = f"{android_plugin_source_dir}/src/main"

    # 1. Copy the .gdap configuration file
    # We assume a file named 'GodotDojoBridge.gdap' exists in 'android/'
    android_plugin_targets.extend(env.Install(addon_dir, "android/GodotDojoBridge.gdap"))

    # 2. Copy the AndroidManifest.xml
    android_plugin_targets.extend(env.Install(android_src_root, "android/src/main/AndroidManifest.xml"))

    # 3. Copy the Kotlin source file into the correct package structure
    kotlin_src_dir = f"{android_src_root}/java/org/godotengine/godot"
    android_plugin_targets.extend(env.Install(kotlin_src_dir, "android/GodotDojoBridge.kt"))

    print(f"{G}{check} Android plugin files copied to {addon_dir}{X}")


# Link Rust libraries
build_mode = "release" if target == "template_release" else "debug"
rust_lib_dir = f"external/dojo.c/target/{rust_target}/{build_mode}"

if platform == "windows":
    if use_mingw:
        # For MinGW, we link against the static .a library
        rust_lib = f"{rust_lib_dir}/libdojo_c.a"
    elif is_host_windows:
        # For MSVC, we link against the static .lib library
        rust_lib = f"{rust_lib_dir}/dojo_c.lib"
        env.Append(LINKFLAGS=['/NODEFAULTLIB:MSVCRT'])
    else:
        rust_lib = "" # Should not happen
    env.Append(LIBS=[File(rust_lib)]) if rust_lib else None
elif platform == "web":
    print(f"{Y}{clipboard} Web export doesn't link to anything.{X}")
#     rust_lib = f"{rust_lib_dir}/libdojo_c.rlib"
#     env.Append(LIBS=[File(rust_lib)])
elif platform in ["linux", "macos", "android"]:
    rust_lib = f"{rust_lib_dir}/libdojo_c.a"
    env.Append(LIBS=[File(rust_lib)])
else:
    print(f"{R}{cross} Rust library not found for platform {platform}{X}")
    Exit(1)

if platform != "web" and not os.path.exists(rust_lib):
    print(f"{R}{cross} Rust library not found: {rust_lib}{X}")
    Exit(1)

sources = sorted(glob.glob("src/**/*.cpp", recursive=True))

# Create library
suffix_map = {
    "linux": f".linux.{target}.{arch}.so",
    "windows": f".windows.{target}.{arch}.dll",
    "macos": f".macos.{target}.{arch}.dylib",
    "web": f".web.{target}.wasm32.wasm"
}

lib_name = f"demo/addons/godot-dojo/{prefix}godot-dojo{suffix_map.get(platform, f'.{platform}.{target}.{arch}.so')}"
library = env.SharedLibrary(target=lib_name, source=sources)

# Make the main library target depend on the Android plugin files being copied.
if android_plugin_targets:
    env.Depends(library, android_plugin_targets)

# Generate .gdextension
with open("plugin_template.gdextension.in", 'r') as f:
    template = f.read()

gdext = template.replace("${PROJECT_NAME}", "godot-dojo")
gdext = gdext.replace("${ENTRY_POINT}", "dojoc_library_init")

# Determine Godot minimum requirement from the godot-cpp submodule's tag/branch.
# We extract the first two numeric components (e.g., 4.2 from 4.2.2 or godot-4.3-stable).
import re

def _detect_godot_min_requirement():
    repo_path = os.path.join(os.getcwd(), "external", "godot-cpp")
    version_source = None
    try:
        # Prefer the latest reachable tag.
        tag = subprocess.check_output([
            "git", "-C", repo_path, "describe", "--tags", "--abbrev=0"
        ], stderr=subprocess.DEVNULL, text=True).strip()
        version_source = tag
    except Exception:
        try:
            # Fallback to branch name if no tag is found.
            branch = subprocess.check_output([
                "git", "-C", repo_path, "symbolic-ref", "-q", "--short", "HEAD"
            ], stderr=subprocess.DEVNULL, text=True).strip()
            version_source = branch
        except Exception:
            pass

    # As a last resort, try reading the .gitmodules declared branch.
    if not version_source:
        try:
            with open(os.path.join(os.getcwd(), ".gitmodules"), "r") as gm:
                gm_text = gm.read()
                # Look for the external/godot-cpp section and capture a version-like pattern.
                # Example: branch = godot-4.3-stable
                m = re.search(r"\[submodule \"external/godot-cpp\"\][\s\S]*?branch\s*=\s*([^\n\r]+)", gm_text)
                if m:
                    version_source = m.group(1).strip()
        except Exception:
            pass

    if version_source:
        m = re.search(r"(\d+)\.(\d+)", version_source)
        if m:
            return f"{m.group(1)}.{m.group(2)}"

    # Default fallback if detection fails.
    return "4.2"

_godot_min = _detect_godot_min_requirement()
print(f"Using GODOT_MIN_REQUIREMENT={_godot_min} derived from godot-cpp git ref.")

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
