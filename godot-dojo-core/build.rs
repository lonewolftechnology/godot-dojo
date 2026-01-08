use std::path::PathBuf;
use std::env;
use std::process::Command;
use std::fs;
use cargo_metadata::MetadataCommand;
use build_print::{println, *};

/// Finds the root directory of a dependency using `cargo metadata`.
fn find_dependency_dir(dep_name: &str) -> Result<PathBuf, String> {
    let metadata = MetadataCommand::new()
        .exec()
        .map_err(|e| format!("Failed to execute `cargo metadata`: {}", e))?;

    let pkg = metadata.packages.iter()
        .find(|p| p.name == dep_name)
        .ok_or_else(|| format!("Could not find package '{}' in metadata.", dep_name))?;

    Ok(pkg.manifest_path.as_std_path().parent().unwrap().to_path_buf())
}

/// Manages path finding and information printing for a uniffi dependency.
fn handle_uniffi_dependency(dep_name: &str, udl_relative_path: &str, output_sub_dir: &str) {
    custom_println!("PROCESS", green, "dependency: {}", dep_name);

    // 1. Find the dependency's binary
    let bindgen_var = format!("CARGO_BIN_FILE_{}_{}", dep_name.to_uppercase().replace('-', "_"), "uniffi-bindgen-cpp");
    let bindgen_path = env::var(&bindgen_var)
        .unwrap_or_else(|_| panic!("Could not find environment variable '{}'. Make sure the binary artifact is configured in Cargo.toml.", bindgen_var));
    note!("Binary: {}", bindgen_path);

    // 2. Find the dependency's source directory
    let dep_dir = find_dependency_dir(dep_name)
        .unwrap_or_else(|e| panic!("Could not find the directory for dependency '{}': {}", dep_name, e));
    note!("Source directory: {}", dep_dir.display());

    // 3. Build the path to the UDL inside the dependency
    let source_udl_path = dep_dir.join(udl_relative_path);
    note!("UDL path: {}", source_udl_path.display());

    // 4. Tell Cargo to re-run if the original UDL changes
    println!("cargo:rerun-if-changed={}", source_udl_path.display());

    // 5. Command to generate the bindings
    let output_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap())
        .parent().unwrap() // Go up one directory
        .join("bindings") // And use the "bindings" folder
        .join(output_sub_dir);
    note!("Output directory: {}", output_dir.display());
    
    // Ensure the output directory exists before generating files
    fs::create_dir_all(&output_dir)
        .unwrap_or_else(|e| panic!("Failed to create output directory {}: {}", output_dir.display(), e));


    let output = Command::new(&bindgen_path)
        .arg(&source_udl_path)
        .arg(&output_dir)
        .output() // Use .output() to capture stdout and stderr
        .unwrap_or_else(|e| panic!("Failed to launch uniffi-bindgen-cpp command for {}: {}", dep_name, e));

    if !output.status.success() {
        // If the command failed, show the error and panic.
        let stderr = String::from_utf8_lossy(&output.stderr);
        error!("uniffi-bindgen-cpp for '{}' failed:\n{}", dep_name, stderr);
        panic!();
    }
    
    // For controller.hpp, inject the std::endian polyfill for C++17 compilers.
    if output_sub_dir == "controller" {
        let hpp_path = output_dir.join("controller.hpp");
        note!("Attempting to patch file with std::endian polyfill: {}", hpp_path.display());

        if hpp_path.exists() {
            let mut hpp_content = fs::read_to_string(&hpp_path)
                .expect("Failed to read generated controller.hpp for patching.");

            let polyfill = r#"

// Polyfill for std::endian (C++20) when using C++17 or older.
// UniFFI-generated code requires this.
#if __cplusplus < 202002L
#include <cstdint>
namespace std {
    enum class endian {
#if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        little = 0,
        big = 1,
        native = little
#elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
        little = 1,
        big = 0,
        native = big
#else
#  error "Cannot determine endianness"
#endif
    };
}
#endif

"#;
            let insertion_hook = "namespace controller {";
            if let Some(pos) = hpp_content.find(insertion_hook) {
                hpp_content.insert_str(pos, polyfill);
                fs::write(&hpp_path, hpp_content)
                    .expect("Failed to write patched controller.hpp");
                custom_println!("PATCHED", green, "Injected std::endian polyfill into {}", hpp_path.display());
            } else {
                warn!("Could not find insertion point for std::endian polyfill in {}.", hpp_path.display());
            }
        } else {
            warn!("controller.hpp not found for patching at {}", hpp_path.display());
        }
    }

    custom_println!("DONE", green, "of {}\n", dep_name);
}

fn main(){
    if env::var("SKIP_BINDINGS_GENERATION").as_deref() == Ok("1") {
        info!("Skipping bindings generation because SKIP_BINDINGS_GENERATION is set to 1.");
        return;
    };

    let manifest_dir = env::var("CARGO_MANIFEST_DIR").unwrap();
    note!("Source directory of 'godot-dojo-core': {}", manifest_dir);

    handle_uniffi_dependency("controller-uniffi", "src/controller.udl", "controller");
    handle_uniffi_dependency("dojo-uniffi", "src/dojo.udl", "dojo");

    // --- Copy dojo.h from dojo-c ---
    custom_println!("PROCESS", green, "dependency: dojo-c");

    // 1. Find the source directory of the dojo-c dependency.
    let dojo_c_dir = find_dependency_dir("dojo-c")
        .expect("Could not find the directory for dependency 'dojo-c'");
    // The `dojo-c` crate is inside a workspace. The `bindings` folder is at the root of the repository.
    // So we need to go up from the crate's directory to the repository root.
    let source_header_path = dojo_c_dir.parent().unwrap().parent().unwrap() // Go up to the repo root
        .join("bindings").join("c").join("dojo.h");
    note!("Source header: {}", source_header_path.display());

    // 2. Define the destination directory.
    let dest_dir = PathBuf::from(&manifest_dir).parent().unwrap().join("bindings").join("dojo");
    let dest_header_path = dest_dir.join("dojo.h");
    note!("Destination header: {}", dest_header_path.display());

    // 3. Tell Cargo to re-run if the original header changes.
    println!("cargo:rerun-if-changed={}", source_header_path.display());

    // 4. Copy the file.
    fs::copy(&source_header_path, &dest_header_path)
        .unwrap_or_else(|e| panic!("Failed to copy dojo.h: {}", e));
    info!("Copied dojo.h successfully\n");


}