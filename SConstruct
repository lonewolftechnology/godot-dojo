#!/usr/bin/env python
import os
import subprocess
import glob
import shutil
import platform as host_platform
import sys

# --- IDE helper imports for SCons (no effect during actual scons run) ---
try:
    from SCons.Script import AddOption, GetOption, Return, Exit, File, SConscript, Glob, Command  # type: ignore
except Exception:
    def GetOption(name: str):
        return None


    def Return():
        pass


    def Exit(code: int = 0):
        raise SystemExit(code)


    def File(path: str):
        return path
    
    def AddOption(*args, **kwargs):
        pass


    def SConscript(path: str):
        class _Env(dict):
            def Append(self, **kwargs):
                pass

            def AddPostAction(self, *args, **kwargs):
                pass

            def Default(self, *args, **kwargs):
                pass

            def SharedLibrary(self, *args, **kwargs):
                return args[0] if args else [None]
            
            def GodotCPPDocData(self, *args, **kwargs):
                return "doc_data.gen.cpp"
            
            def subst(self, text):
                return ""
            
            def ParseConfig(self, *args, **kwargs):
                pass

            def Clone(self, **kwargs):
                return self

        return _Env()
    
    def Glob(pattern):
        return []

    def Command(target, source, action):
        return None

# Import methods
from methods import (
    ANSI,
    rocket,
    broom,
    check,
    package,
    cross,
    party,
    apply_dojo_h_patch,
    detect_godot_min_requirement,
    copy_web_artifacts
)

AddOption("--merge-ios-xcframeworks", dest="merge_ios_xcframeworks", action="store_true", default=False, help="Merge iOS device and simulator XCFrameworks")

print(f"{ANSI.BLUE}{rocket} Building godot-dojo{ANSI.RESET}")

# Logic for the new merge command
if GetOption('merge_ios_xcframeworks'):
    env_merge = SConscript("external/godot-cpp/SConstruct")
    if env_merge.get("platform") != "ios":
        print(f"{ANSI.RED}{cross} The --merge-ios-xcframeworks option is only valid with 'platform=ios'.{ANSI.RESET}")
        Exit(1)

    print(f"{ANSI.YELLOW}{package} Merging iOS XCFrameworks...{ANSI.RESET}")
    
    target_merge = env_merge.get("target", "template_debug")
    build_mode_merge = "release" if target_merge == "template_release" else "debug"

    base_path = "demo/addons/godot-dojo/bin"
    device_fw_path = f"{base_path}/ios-arm64/{build_mode_merge}/godot-dojo.xcframework"
    sim_fw_path = f"{base_path}/ios-simulator/{build_mode_merge}/godot-dojo.xcframework"
    output_fw_path = f"{base_path}/ios/{build_mode_merge}/godot-dojo.xcframework"

    if not os.path.exists(device_fw_path) or not os.path.exists(sim_fw_path):
        print(f"{ANSI.RED}{cross} Cannot find required frameworks to merge.{ANSI.RESET}")
        print(f"Ensure the following exist by running the device and simulator builds first:")
        print(f"  - {device_fw_path}")
        print(f"  - {sim_fw_path}")
        Exit(1)

    # The paths to the actual dylib files inside the xcframeworks
    device_lib_path = f"{device_fw_path}/ios-arm64/godot-dojo.ios.{target_merge}.arm64.dylib"
    sim_lib_path = f"{sim_fw_path}/ios-arm64_x86_64-simulator/godot-dojo.ios.{target_merge}.universal.dylib"

    if not os.path.exists(device_lib_path) or not os.path.exists(sim_lib_path):
        print(f"{ANSI.RED}{cross} Cannot find required .dylib files inside the XCFrameworks.{ANSI.RESET}")
        print(f"  Expected device library: {device_lib_path}")
        print(f"  Expected simulator library: {sim_lib_path}")
        Exit(1)

    print(f"  Device input library: {device_lib_path}")
    print(f"  Simulator input library: {sim_lib_path}")
    print(f"  Output: {output_fw_path}")

    if os.path.exists(output_fw_path):
        shutil.rmtree(output_fw_path)

    os.makedirs(os.path.dirname(output_fw_path), exist_ok=True)

    cmd = ["xcodebuild", "-create-xcframework", 
           "-library", device_lib_path,
           "-library", sim_lib_path,
           "-output", output_fw_path]
    
    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True)
        print(f"{ANSI.GREEN}{check} Universal XCFramework created successfully at {output_fw_path}.{ANSI.RESET}")
    except subprocess.CalledProcessError as e:
        print(f"{ANSI.RED}{cross} XCFramework merging failed:{ANSI.RESET}")
        print(e.stdout)
        print(e.stderr)
        Exit(1)

    Return() # End the script after merging

# Cleanup
if GetOption('clean'):
    print(f"{ANSI.YELLOW}{broom} Cleaning...{ANSI.RESET}")
    try:
        subprocess.run(["cargo", "clean"], cwd="external/dojo.c", check=True)
        shutil.rmtree("demo/addons/godot-dojo", ignore_errors=True)
        shutil.rmtree("demo/dist", ignore_errors=True)
        subprocess.run(["scons", "-C", "external/godot-cpp", "--clean"], check=False)
    except:
        pass
    print(f"{ANSI.GREEN}{check} Cleanup complete{ANSI.RESET}")
    Return()

# Setup
os.makedirs("demo/addons/godot-dojo", exist_ok=True)

# Initialize the godot-cpp build environment
env = SConscript("external/godot-cpp/SConstruct")
platform, arch, target = env["platform"], env["arch"], env.get("target", "template_debug")

build_info = f"{platform} ({arch}) - {target}"
if env.get("precision") == "double":
    build_info += " (double precision)"

print(f"{ANSI.BLUE}Building: {build_info}{ANSI.RESET}")

# --- Rust Compilation ---
print(f"{ANSI.YELLOW}{package} Compiling dojo.c...{ANSI.RESET}")

is_host_windows = host_platform.system().lower() == "windows"
use_mingw = env.get("use_mingw", False)
build_mode = "release" if target == "template_release" else "debug"

# Define Rust targets for each platform/architecture combination
rust_targets_map = {
    ("windows", "x86_64"): ["x86_64-pc-windows-gnu" if (not is_host_windows or use_mingw) else "x86_64-pc-windows-msvc"],
    ("linux", "x86_64"): ["x86_64-unknown-linux-gnu"],
    ("linux", "arm64"): ["aarch64-unknown-linux-gnu"],
    ("macos", "x86_64"): ["x86_64-apple-darwin"],
    ("macos", "arm64"): ["aarch64-apple-darwin"],
    ("macos", "universal"): ["x86_64-apple-darwin", "aarch64-apple-darwin"],
    ("web", "wasm32"): ["wasm32-unknown-unknown"],
    ("android", "arm64"): ["aarch64-linux-android"],
    ("ios", "arm64"): ["aarch64-apple-ios"],
    ("ios", "x86_64"): ["x86_64-apple-ios"],
    ("ios", "universal"): [],  # Placeholder, will be populated below
}

rust_targets_to_build = rust_targets_map.get((platform, arch))
if rust_targets_to_build is None:
    print(f"{ANSI.RED}{cross} Unsupported platform/architecture for Rust build: {platform}/{arch}{ANSI.RESET}")
    Exit(1)

is_ios_simulator = env.get("ios_simulator", False)

# Variable for the platform output path
platform_path_segment = platform
if platform == "ios":
    if is_ios_simulator:
        platform_path_segment = "ios-simulator"
    else:
        platform_path_segment = "ios-arm64"


if platform == "ios":
    if arch == "arm64":
        if is_ios_simulator:
            rust_targets_to_build = ["aarch64-apple-ios-sim"]
        # else: it's already ["aarch64-apple-ios"] from the map, which is correct for device.
    elif arch == "universal":
        if is_ios_simulator:
            rust_targets_to_build = ["x86_64-apple-ios", "aarch64-apple-ios-sim"]
        else:
            # If building for a physical device (not simulator) with 'universal',
            # it only makes sense to build for arm64. We'll override arch to reflect this.
            arch = "arm64"
            rust_targets_to_build = ["aarch64-apple-ios"]

# For universal builds, we define a "virtual" target name for directory purposes.
# For single-target builds, it's just the first (and only) item in the list.
if (platform == "macos" or platform == "ios") and arch == "universal":
    if platform == "macos":
        rust_target_folder_name = "universal-apple-darwin"
    else:  # ios
        if is_ios_simulator:
            rust_target_folder_name = "universal-apple-ios-simulator"
        else:
            rust_target_folder_name = "universal-apple-ios"
else:
    rust_target_folder_name = rust_targets_to_build[0]

# The web build is special and has its own flow
if platform == "web":
    rust_target = rust_targets_to_build[0]
    cmd = ["cargo", "build", "--target", rust_target]
    if build_mode == "release":
        cmd.append("--release")

    print(f"{ANSI.YELLOW}Building dojo.c for wasm-bindgen...{ANSI.RESET}")
    bindgen_env = os.environ.copy()
    bindgen_env["RUSTFLAGS"] = "-C target-feature=+atomics,+bulk-memory,+mutable-globals"
    subprocess.run(cmd, check=True, cwd="external/dojo.c", env=bindgen_env)

    print(f"{ANSI.YELLOW}Running wasm-bindgen...{ANSI.RESET}")
    wasm_input_path = f"external/dojo.c/target/{rust_target}/{build_mode}/dojo_c.wasm"
    out_dir = "demo/addons/godot-dojo/"
    os.makedirs(out_dir, exist_ok=True)
    out_name = f"dojo_c_{build_mode}"

    bindgen_cmd = ["wasm-bindgen", wasm_input_path, "--out-dir", out_dir, "--out-name", out_name, "--target", "web", "--no-typescript"]
    try:
        subprocess.run(bindgen_cmd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"{ANSI.RED}{cross} wasm-bindgen failed: {e}{ANSI.RESET}")
        Exit(1)
else:
    # Standard compilation for native platforms (macOS, Linux, Windows, etc.)
    cargo_cmd_base = ["cargo", "build"]
    if build_mode == "release":
        cargo_cmd_base.append("--release")

    # When cross-compiling (especially for iOS), godot-cpp's SCons setup might
    # put the compiler and its arguments together in the 'CC' variable (e.g., "clang -isysroot ...").
    # Cargo interprets this entire string as the linker path, causing an error.
    # We need to separate the compiler from its arguments and pass them correctly.
    cc_from_env = env.get("CC")
    if cc_from_env and ' ' in cc_from_env:
        cc_parts = cc_from_env.split(' ', 1)
        compiler_path = cc_parts[0] # e.g., 'clang'
        # Arguments from CC are typically for the linker
        linker_args_from_cc = [f"-C link-arg={arg}" for arg in cc_parts[1].split(' ') if arg]
    else:
        compiler_path = cc_from_env
        linker_args_from_cc = []

    # Also get flags from CCFLAGS, which godot-cpp uses for version-min flags.
    # These are compiler flags, but for clang they also apply to linking.
    ccflags_from_env = env.get("CCFLAGS", [])
    linker_args_from_ccflags = [f"-C link-arg={flag}" for flag in ccflags_from_env if flag.startswith("-m")]
    linker_args = linker_args_from_cc + linker_args_from_ccflags

    env_vars = os.environ.copy()
    if compiler_path:
        env_vars['CC'] = compiler_path
    if linker_args:
        existing_rustflags = env_vars.get("RUSTFLAGS", "")
        env_vars['RUSTFLAGS'] = ' '.join(linker_args) + ' ' + existing_rustflags

    if platform == "macos":
        env_vars["MACOSX_DEPLOYMENT_TARGET"] = "14.0"
        rustflags = env_vars.get("RUSTFLAGS", "")
        env_vars["RUSTFLAGS"] = f"{rustflags} -C link-arg=-mmacosx-version-min=14.0".lstrip()

    libs_to_lipo = []
    try:
        for current_rust_target in rust_targets_to_build:
            print(f"{ANSI.YELLOW}Building for Rust target: {current_rust_target}...{ANSI.RESET}")
            
            # Ensure Rust target is installed
            try:
                result = subprocess.run(["rustup", "target", "list", "--installed"], capture_output=True, text=True, check=True)
                if current_rust_target not in result.stdout:
                    # On some systems (like fresh macOS installs), rustup might not be in the default PATH
                    # for subprocesses, even if it's in the shell's PATH. We can add the cargo bin dir to the PATH.
                    cargo_home = os.path.expanduser("~/.cargo/bin")
                    env_vars["PATH"] = f"{cargo_home}{os.pathsep}{env_vars['PATH']}"
                    print(f"{ANSI.YELLOW}Installing Rust target {current_rust_target}...{ANSI.RESET}")
                    subprocess.run(["rustup", "target", "add", current_rust_target], check=True)
                    subprocess.run(["rustup", "target", "add", current_rust_target], check=True, cwd="external/dojo.c")
            except subprocess.CalledProcessError as e:
                print(f"{ANSI.RED}{cross} Failed to check or install Rust target: {e}{ANSI.RESET}")

            cmd = cargo_cmd_base + ["--target", current_rust_target]

            # Explicitly tell cargo which linker to use for the target, separating it from linker arguments.
            if compiler_path:
                env_vars[f"CARGO_TARGET_{current_rust_target.upper().replace('-', '_')}_LINKER"] = compiler_path

            subprocess.run(cmd, check=True, cwd="external/dojo.c", env=env_vars)
            
            if (platform == "macos" or platform == "ios") and arch == "universal":
                libs_to_lipo.append(f"external/dojo.c/target/{current_rust_target}/{build_mode}/libdojo_c.a")
    finally:
        # Patch dojo.h after all builds are complete
        apply_dojo_h_patch(Exit)

    # After loop, if it was a universal build, run lipo to create the fat library
    if (platform == "macos" or platform == "ios") and arch == "universal":
        print(f"{ANSI.YELLOW}Creating universal library with lipo...{ANSI.RESET}")
        universal_dir = f"external/dojo.c/target/{rust_target_folder_name}/{build_mode}"
        os.makedirs(universal_dir, exist_ok=True)
        universal_lib_path = f"{universal_dir}/libdojo_c.a"
        
        lipo_cmd = ["lipo", "-create"] + libs_to_lipo + ["-output", universal_lib_path]
        subprocess.run(lipo_cmd, check=True)
        print(f"{ANSI.GREEN}{check} Universal library created at {universal_lib_path}{ANSI.RESET}")

# --- Library Linking ---

if platform != "android":
    env['SHLIBPREFIX'] = ''
prefix = env.subst('$SHLIBPREFIX')
env.Append(CPPPATH=["src/", "include/", "external/dojo.c", "external/boost/include"])

if platform in ["macos", "ios"]:
    print(f"{ANSI.YELLOW}Applying Apple platform specific flags...{ANSI.RESET}")
    env.Append(LINKFLAGS=['-framework', 'Security', '-framework', 'CoreFoundation'])

    if platform == "macos":
        env['ENV']['MACOSX_DEPLOYMENT_TARGET'] = '14.0'
        env.Append(CCFLAGS=['-mmacosx-version-min=14.0'])
        env.Append(LINKFLAGS=['-mmacosx-version-min=14.0'])
    elif platform == "ios":
        # Get the iOS deployment target from the godot-cpp environment and set it for C++ builds.
        # The Rust build gets this from the `env_vars` setup earlier.
        ios_min_version = env.get("ios_min_version", "12.0")
        env['ENV']['IPHONEOS_DEPLOYMENT_TARGET'] = ios_min_version


if platform == "android":
    print(f"{ANSI.YELLOW}{package} Building Android plugin with Gradle...{ANSI.RESET}")
    gradlew_cmd = "gradlew.bat" if is_host_windows else "./gradlew"
    try:
        subprocess.run([gradlew_cmd, ":plugin:assemble"], cwd="android", check=True)
        print(f"{ANSI.GREEN}{check} Android plugin built successfully.{ANSI.RESET}")
    except subprocess.CalledProcessError as e:
        print(f"{ANSI.RED}{cross} Gradle build failed: {e}{ANSI.RESET}")
        Exit(1)

# Link Rust libraries
rust_lib_dir = f"external/dojo.c/target/{rust_target_folder_name}/{build_mode}"
rust_lib = ""

if platform == "windows":
    if use_mingw:
        rust_lib = f"{rust_lib_dir}/libdojo_c.a"
    else:
        rust_lib = f"{rust_lib_dir}/dojo_c.lib"
        env.Append(LINKFLAGS=['/NODEFAULTLIB:MSVCRT'])
    env.Append(LIBS=[File(rust_lib)])
    env.Append(LIBS=['ws2_32', 'advapi32', 'ntdll'])
elif platform == "linux":
    rust_lib = f"{rust_lib_dir}/libdojo_c.a"
    env.Append(LIBS=[File(rust_lib)])
    try:
        env.ParseConfig('pkg-config --cflags --libs dbus-1')
    except Exception:
        env.Append(LIBS=['dbus-1'])
elif platform == "web":
    print(f"{ANSI.YELLOW}{package} Web export doesn't link to anything.{ANSI.RESET}")
    rust_lib = f"{rust_lib_dir}/libdojo_c.rlib" # This is not used for linking, but for existence check
else:
    rust_lib = f"{rust_lib_dir}/libdojo_c.a"
    env.Append(LIBS=[File(rust_lib)])

if platform != "web" and not os.path.exists(rust_lib):
    print(f"{ANSI.RED}{cross} Rust library not found: {rust_lib}{ANSI.RESET}")
    Exit(1)

sources = sorted(glob.glob("src/**/*.cpp", recursive=True))

if target in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# --- Final library build ---

def build_complete_callback(target, source, env):
    print(f"{ANSI.GREEN}{party} Build complete!{ANSI.RESET}")
    if env["platform"] == "web":
        copy_web_artifacts(target, source, env)
    return None

# Check if we are building for Apple platforms and not for the editor
is_apple_xcframework_build = platform in ["macos", "ios"] and target != "editor"

if is_apple_xcframework_build:
    # For XCFramework builds, the dylib is an intermediate product.
    output_dir = f"demo/addons/godot-dojo/bin/{platform_path_segment}/{build_mode}/intermediate"
    os.makedirs(output_dir, exist_ok=True)
    dylib_suffix = f".{platform}.{target}.{arch}.dylib"
    dylib_path = f"{output_dir}/{prefix}godot-dojo{dylib_suffix}"
    
    built_dylib = env.SharedLibrary(target=dylib_path, source=sources)

    # Define the final XCFramework path
    xcframework_output_dir = f"demo/addons/godot-dojo/bin/{platform_path_segment}/{build_mode}"
    xcframework_path = f"{xcframework_output_dir}/godot-dojo.xcframework"

    def create_xcframework(target, source, env):
        source_lib = source[0].get_path()
        target_dir = target[0].get_path()

        # Ensure the output directory exists before creating the xcframework
        output_parent_dir = os.path.dirname(target_dir)
        os.makedirs(output_parent_dir, exist_ok=True)

        print(f"{ANSI.YELLOW}{package} Creating XCFramework at: {target_dir}{ANSI.RESET}")

        # The xcframework path itself must not exist for xcodebuild
        if os.path.exists(target_dir):
            shutil.rmtree(target_dir)

        # Collect header directories from CPPPATH
        header_dirs = env.get("CPPPATH", [])
        # We only want to include headers that are part of our project, not system headers.
        project_header_dirs = []
        for h in header_dirs:
            h_str = str(h)
            if os.path.isdir(h_str) and not h_str.startswith('/usr'):
                project_header_dirs.append(h_str)

        cmd = ["xcodebuild", "-create-xcframework", "-library", source_lib]
        for header_dir in project_header_dirs:
            cmd.extend(["-headers", header_dir])
        cmd.extend(["-output", target_dir])
        
        try:
            subprocess.run(cmd, check=True, capture_output=True, text=True)
            print(f"{ANSI.GREEN}{check} XCFramework created successfully.{ANSI.RESET}")
            
            intermediate_dir = os.path.dirname(source_lib)
            if os.path.isdir(intermediate_dir):
                print(f"{ANSI.YELLOW}{broom} Cleaning up intermediate directory: {intermediate_dir}{ANSI.RESET}")
                shutil.rmtree(intermediate_dir)

        except subprocess.CalledProcessError as e:
            print(f"{ANSI.RED}{cross} XCFramework creation failed:{ANSI.RESET}")
            print(e.stdout)
            print(e.stderr)
            Exit(1)

    # Create a command to build the XCFramework
    final_library = env.Command(xcframework_path, built_dylib, create_xcframework)

else:
    # Standard build for other platforms or editor builds
    output_dir = f"demo/addons/godot-dojo/bin/{platform_path_segment}/{build_mode}"
    os.makedirs(output_dir, exist_ok=True)
    suffix_map = {
        "linux": f".linux.{target}.{arch}.so",
        "windows": f".windows.{target}.{arch}.dll",
        "macos": f".macos.{target}.{arch}.dylib",
        "ios": f".ios.{target}.{arch}.dylib",
        "web": f".web.{target}.wasm32.wasm"
    }
    lib_name = f"{output_dir}/{prefix}godot-dojo{suffix_map.get(platform, f'.{platform}.{target}.{arch}.so')}"
    final_library = env.SharedLibrary(target=lib_name, source=sources)


env.Alias(f"godot-dojo-{platform}-{build_mode}", final_library)
env.AddPostAction(final_library, build_complete_callback)
env.Default(final_library)

# --- GDExtension file generation ---
with open("plugin_template.gdextension.in", 'r') as f:
    template = f.read()

gdext = template.replace("${PROJECT_NAME}", "godot-dojo")
gdext = gdext.replace("${ENTRY_POINT}", "dojoc_library_init")
_godot_min = detect_godot_min_requirement()
gdext = gdext.replace("${GODOT_MIN_REQUIREMENT}", _godot_min)

with open("demo/addons/godot-dojo/godot-dojo.gdextension", 'w') as f:
    f.write(gdext)
