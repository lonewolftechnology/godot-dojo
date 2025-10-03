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

build_info = f"{platform} ({arch}) - {target}"
if env.get("precision") == "double":
    build_info += " (double precision)"

print(f"{B}Building: {build_info}{X}")

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
        subprocess.run(["rustup", "target", "add", rt], check=False, cwd="external/dojo.c")

except subprocess.CalledProcessError as e:
    print(f"{R}{cross} Failed to check or install Rust target: {e}{X}")
    # Continue anyway, as cargo will show a more specific error if needed

cmd = ["cargo", "build", "--target", rust_target]

# Special handling for macOS universal builds
if platform == "macos" and arch == "universal":
    print(f"{Y}Starting universal build for macOS...{X}")
    build_mode = "release" if target == "template_release" else "debug"
    rust_targets = targets.get((platform, arch))
    libs_to_lipo = []

    for rt in rust_targets:
        print(f"{Y}Compiling for target: {rt}...{X}")
        cargo_cmd = ["cargo", "build", "--target", rt]
        if build_mode == "release":
            cargo_cmd.append("--release")

        env_vars = os.environ.copy()
        env_vars["MACOSX_DEPLOYMENT_TARGET"] = "14.0"
        rustflags = env_vars.get("RUSTFLAGS", "")
        if rustflags:
            rustflags += " "
        rustflags += "-C link-arg=-mmacosx-version-min=14.0"
        env_vars["RUSTFLAGS"] = rustflags

        subprocess.run(cargo_cmd, check=True, cwd="external/dojo.c", env=env_vars)
        libs_to_lipo.append(f"external/dojo.c/target/{rt}/{build_mode}/libdojo_c.a")

    print(f"{Y}Creating universal library with lipo...{X}")
    universal_dir = f"external/dojo.c/target/universal/{build_mode}"
    os.makedirs(universal_dir, exist_ok=True)
    universal_lib_path = f"{universal_dir}/libdojo_c.a"

    lipo_cmd = ["lipo", "-create"] + libs_to_lipo + ["-output", universal_lib_path]
    subprocess.run(lipo_cmd, check=True)

    print(f"{G}{check} Universal library created at: {universal_lib_path}{X}")
    # Set rust_target to 'universal' to adjust library path later
    rust_target = "universal"
    cmd = [] # Command already executed


def apply_dojo_h_patch():
    """Applies a patch to dojo.h to fix an incomplete type issue."""
    print(f"{Y}{clipboard} Applying workaround patch to dojo.h...{X}")
    # Check if the 'patch' command is available.
    if not shutil.which("patch"):
        print(f"{R}{cross} Error: The 'patch' command was not found in your system's PATH.{X}")
        print(f"{Y}This is required to apply a necessary fix to a dependency.{X}")
        print(f"{B}On Windows, the easiest way to get it is by installing 'Git for Windows':{X}")
        print(f"{G}https://git-scm.com/download/win{X}")
        print(f"{Y}Please install it and make sure its tools are added to your PATH, then try again.{X}")
        Exit(1)

    patch_file = 'patches/fix_dojo_c_incomplete_type.patch'
    dojo_c_dir = 'external/dojo.c'
    target_to_patch = f'{dojo_c_dir}/dojo.h'

    try:
        subprocess.run(['patch', '-p1', '-d', dojo_c_dir, '-i', os.path.abspath(patch_file)], check=True)
        print(f"{G}{check} Patch applied successfully to {target_to_patch}.{X}")
    except Exception as e:
        print(f"{R}{cross} Failed to apply patch: {e}{X}")
        Exit(1)


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

    # Apply patch immediately after Rust compilation
    apply_dojo_h_patch()

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
        print(
            f"{R}{cross} wasm-bindgen failed. This might be due to incompatible Rust flags or wasm-bindgen version.{X}")
        print(f"{R}Error: {e}{X}")
        Exit(1)
else:
    if cmd: # Only run if not a universal macOS build (which has its own logic)
        env_vars = os.environ.copy()
        # Explicitly set RUSTUP_TOOLCHAIN to ensure the correct toolchain is used
        if platform == "macos":
            # Set macOS deployment target to 14.0 to ensure compatibility
            print(f"{Y}Setting macOS deployment target to 14.0...{X}")
            env_vars["MACOSX_DEPLOYMENT_TARGET"] = "14.0"

            # Add deployment target to RUSTFLAGS
            rustflags = env_vars.get("RUSTFLAGS", "")
            if rustflags:  # Add a space if RUSTFLAGS is not empty
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
    # Apply patch immediately after Rust compilation
    apply_dojo_h_patch()


# Configure library
if platform != "android":
    # Android build requires lib prefix
    env['SHLIBPREFIX'] = ''
prefix = env.subst('$SHLIBPREFIX')
env.Append(CPPPATH=["src/", "include/", "external/dojo.c", "external/boost/include"])
if platform == "macos":
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
    print(f"{Y}{package} Building Android plugin with Gradle...{X}")
    gradlew_cmd = "gradlew.bat" if is_host_windows else "./gradlew"
    try:
        # Execute the gradle build command in the 'android' directory
        subprocess.run([gradlew_cmd, ":plugin:assemble"], cwd="android", check=True)
        print(f"{G}{check} Android plugin built successfully.{X}")
    except subprocess.CalledProcessError as e:
        print(f"{R}{cross} Gradle build failed: {e}{X}")
        Exit(1)


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
        rust_lib = ""  # Should not happen

    # Link our library first, then the system libraries it depends on.
    env.Append(LIBS=[File(rust_lib)])
    env.Append(LIBS=['ws2_32', 'advapi32', 'ntdll'])
elif platform == "linux":
    rust_lib = f"{rust_lib_dir}/libdojo_c.a"
    env.Append(LIBS=[File(rust_lib)])
    # Use pkg-config to add proper include/lib flags for dbus-1 and ensure correct link order.
    # It's important to add this *after* our own library which depends on it.
    try:
        env.ParseConfig('pkg-config --cflags --libs dbus-1')
    except Exception:
        env.Append(LIBS=['dbus-1'])
elif platform == "web":
    print(f"{Y}{clipboard} Web export doesn't link to anything.{X}")
#     rust_lib = f"{rust_lib_dir}/libdojo_c.rlib"
#     env.Append(LIBS=[File(rust_lib)])
elif platform in ["linux", "macos", "android", "ios"]:
    rust_lib = f"{rust_lib_dir}/libdojo_c.a"
    env.Append(LIBS=[File(rust_lib)])
else:
    print(f"{R}{cross} Rust library not found for platform {platform}{X}")
    Exit(1)

if platform != "web" and not os.path.exists(rust_lib):
    print(f"{R}{cross} Rust library not found: {rust_lib}{X}")
    Exit(1)

sources = sorted(glob.glob("src/**/*.cpp", recursive=True))

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
