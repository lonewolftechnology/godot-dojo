import os
import sys

def get_rust_target(env):
    platform = env["platform"]
    arch = env["arch"]
    
    if platform == "ios" and env.get('ios_simulator'):
        return "aarch64-apple-ios-sim" if arch == "arm64" else "x86_64-apple-ios"
        
    targets = {
        ("windows", "x86_64"): "x86_64-pc-windows-gnu",
        ("linux", "x86_64"): "x86_64-unknown-linux-gnu",
        ("linux", "arm64"): "aarch64-unknown-linux-gnu",
        ("macos", "x86_64"): "x86_64-apple-darwin",
        ("macos", "arm64"): "aarch64-apple-darwin",
        ("web", "wasm32"): "wasm32-unknown-emscripten",
        ("android", "arm64"): "aarch64-linux-android",
        ("ios", "arm64"): "aarch64-apple-ios",
        ("ios", "x86_64"): "x86_64-apple-ios",
    }
    return targets.get((platform, arch), "")

def compile_rust_immediately(env):
    if env.GetOption("clean"):
        return
        
    is_release = env.get("target") in ["template_release"]
    platform = env["platform"]
    arch = env["arch"]
    
    def run_cargo(rust_target):
        cargo_args = ["cargo", "build"]
        if is_release:
            cargo_args.append("--release")
        if rust_target:
            cargo_args.extend(["--target", rust_target])
        cmd = " ".join(cargo_args)
        print(f"Compiling Rust library strictly first: {cmd}")
        ret = os.system(f"cd godot-dojo-core && {cmd}")
        if ret != 0:
            sys.stderr.write(f"\033[91mError: Failed to compile Rust (exit code {ret})\033[0m\n")
            sys.exit(ret)

    if platform == "web":
        if env.get("use_wasm_pack"):
            profile = "--release" if is_release else "--dev"
            cmd = f"wasm-pack build {profile} --target web"
            print(f"Compiling Rust library strictly first with wasm-pack: {cmd}")
            ret = os.system(f"cd godot-dojo-core && {cmd}")
            if ret != 0:
                sys.stderr.write(f"\033[91mError: Failed to compile Rust (exit code {ret})\033[0m\n")
                sys.exit(ret)
            return
        else:
            run_cargo("wasm32-unknown-emscripten")
            return

    if arch == "universal":
        targets = []
        if platform == "macos":
            targets = ["x86_64-apple-darwin", "aarch64-apple-darwin"]
        elif platform == "ios":
            targets = ["aarch64-apple-ios", "x86_64-apple-ios"]
        
        for t in targets:
            run_cargo(t)
        
        profile_dir = "release" if is_release else "debug"
        out_dir = f"godot-dojo-core/target/universal/{profile_dir}"
        os.makedirs(out_dir, exist_ok=True)
        in_libs = [f"godot-dojo-core/target/{t}/{profile_dir}/libgodot_dojo_core.a" for t in targets]
        lipo_cmd = f"lipo -create {' '.join(in_libs)} -output {out_dir}/libgodot_dojo_core.a"
        print(f"Creating universal binary: {lipo_cmd}")
        ret = os.system(lipo_cmd)
        if ret != 0:
            sys.stderr.write(f"\033[91mError: Failed to create universal binary (exit code {ret})\033[0m\n")
            sys.exit(ret)
    else:
        rust_target = get_rust_target(env)
        run_cargo(rust_target)

def exists(env):
    return True

def generate(env):
    compile_rust_immediately(env)
    
    platform = env["platform"]
    is_release = env.get("target") in ["template_release"]
    profile_dir = "release" if is_release else "debug"
    
    if platform == "web" and env.get("use_wasm_pack"):
        env["RUST_LIB_PATH"] = ""
    else:
        ext = ".a"
        if platform == "windows":
            ext = ".a" 
            
        if env["arch"] == "universal":
            env["RUST_LIB_PATH"] = f"godot-dojo-core/target/universal/{profile_dir}/libgodot_dojo_core{ext}"
        else:
            rust_target = get_rust_target(env)
            if rust_target:
                env["RUST_LIB_PATH"] = f"godot-dojo-core/target/{rust_target}/{profile_dir}/libgodot_dojo_core{ext}"
            else:
                env["RUST_LIB_PATH"] = f"godot-dojo-core/target/{profile_dir}/libgodot_dojo_core{ext}"