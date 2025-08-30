#!/usr/bin/env python
import os
import subprocess
import glob
import shutil
import platform as host_platform
import sys

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
        subprocess.run(["scons", "-C", "external/godot-cpp", "--clean"], check=False)
    except:
        pass
    print(f"{G}{check} Cleanup complete{X}")
    Return()

# Setup
os.makedirs("demo/addons/godot-dojo", exist_ok=True)
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
    cmd.insert(1, "+nightly")
    cmd.extend(["-Z", "build-std=std,panic_abort"])
    env_vars = os.environ.copy()

    rustflags = "-C target-feature=+atomics,+bulk-memory,+mutable-globals"
    rustflags += " -C relocation-model=pic"
    rustflags += " --print=native-static-libs"
    # rustflags += " -C panic=abort"
    # rustflags += " -C opt-level=z"

    env_vars["RUSTFLAGS"] = rustflags

    subprocess.run(cmd, check=True, cwd="external/dojo.c", env=env_vars)
else:
    env_vars = os.environ.copy()
    # Explicitly set RUSTUP_TOOLCHAIN to ensure the correct toolchain is used
    if platform == "macos" and arch == "x86_64":
        # Set macOS deployment target to 14.0 to ensure compatibility
        print(f"{Y}Setting macOS deployment target to 14.0...{X}")
        env_vars["MACOSX_DEPLOYMENT_TARGET"] = "14.0"

        # Add deployment target to RUSTFLAGS
        rustflags = env_vars.get("RUSTFLAGS", "")
        if rustflags:
            rustflags += " "
        rustflags += "-C link-arg=-mmacosx-version-min=14.0"
        env_vars["RUSTFLAGS"] = rustflags
    elif platform == "macos" and arch == "arm64":
        # Also set for arm64 builds
        print(f"{Y}Setting macOS deployment target to 14.0...{X}")
        env_vars["MACOSX_DEPLOYMENT_TARGET"] = "14.0"

        # Add deployment target to RUSTFLAGS
        rustflags = env_vars.get("RUSTFLAGS", "")
        if rustflags:
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
    env.Append(LINKFLAGS=['-ldbus-1'])
    env.Append(
        CXXFLAGS=["-std=c++17"]
    )
elif platform == "macos":
    # Set macOS deployment target for C++ compilation
    print(f"{Y}Setting macOS deployment target for C++ compilation to 14.0...{X}")
    env['ENV']['MACOSX_DEPLOYMENT_TARGET'] = '14.0'
    env.Append(CCFLAGS=['-mmacosx-version-min=14.0'])
    env.Append(LINKFLAGS=['-mmacosx-version-min=14.0'])

# Link Rust libraries
build_mode = "release" if target == "template_release" else "debug"
rust_lib_dir = f"external/dojo.c/target/{rust_target}/{build_mode}"

if platform == "windows":
    rust_lib = ""
    target_dll_name = f"dojo_c.dll"
    target_dll_path = f"demo/addons/godot-dojo/{target_dll_name}"

    if use_mingw:
        rust_lib = f"{rust_lib_dir}/dojo_c.dll"

    elif is_host_windows:
        rust_lib = f"{rust_lib_dir}/dojo_c.dll.lib"
        env.Append(LINKFLAGS=['/NODEFAULTLIB:MSVCRT'])

    if os.path.exists(rust_lib):
        dll_dest = f"{rust_lib_dir}/{target_dll_name}"
        shutil.copy2(dll_dest, target_dll_path)
        print(f"{G}{clipboard} Copied {dll_dest} -> {target_dll_path}{X}")
        # rust_lib = target_dll_path
    env.Append(LIBS=[File(rust_lib)])

elif platform in ["linux", "macos", "web", "android"]:
    rust_lib = f"{rust_lib_dir}/libdojo_c.a"
    env.Append(LIBS=[File(rust_lib)])
else:
    print(f"{R}{cross} Rust library not found for platform {platform}{X}")
    Exit(1)

if not os.path.exists(rust_lib):
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

# Generate .gdextension
with open("plugin_template.gdextension.in", 'r') as f:
    template = f.read()

gdext = template.replace("${PROJECT_NAME}", "godot-dojo")
gdext = gdext.replace("${ENTRY_POINT}", "dojoc_library_init")
gdext = gdext.replace("${GODOT_MIN_REQUIREMENT}", "4.2")

with open("demo/addons/godot-dojo/godot-dojo.gdextension", 'w') as f:
    f.write(gdext)


def build_complete_callback(target, source, env):
    print(f"{G}{party} Build complete!{X}")
    return None


env.AddPostAction(library, build_complete_callback)

env.Default(library)
