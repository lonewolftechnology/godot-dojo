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
    
    // For controller.hpp and dojo.hpp, inject the std::endian polyfill for C++17 compilers.
    if output_sub_dir == "controller" || output_sub_dir == "dojo" {
        let hpp_filename = format!("{}.hpp", output_sub_dir);
        let hpp_path = output_dir.join(&hpp_filename);

        if hpp_path.exists() {
            note!("Attempting to patch file: {}", hpp_path.display());
            let mut content = fs::read_to_string(&hpp_path)
                .unwrap_or_else(|_| panic!("Failed to read generated {} for patching.", hpp_filename));

            let polyfill = r#"

// Polyfill for std::endian (C++20) when using C++17 or older.
// UniFFI-generated code requires this.
#if __cplusplus < 202002L
#ifndef GODOT_DOJO_STD_ENDIAN_POLYFILL
#define GODOT_DOJO_STD_ENDIAN_POLYFILL
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
#endif

"#;
            let insertion_hook = format!("namespace {} {{", output_sub_dir);
            if let Some(pos) = content.find(&insertion_hook) {
                content.insert_str(pos, polyfill);
                custom_println!("PATCHED", green, "Injected std::endian polyfill into {}", hpp_path.display());
            } else {
                warn!("Could not find insertion point for std::endian polyfill in {}.", hpp_path.display());
            }

            // For dojo.hpp, also patch the vtable initializers for callback interfaces.
            // The generated code sometimes places .uniffi_free last, which is incorrect. It must be first.
            if output_sub_dir == "dojo" {
                let vtable_start_hook = "static inline UniffiVTableCallbackInterface";
                let vtable_end_hook = "};";
                let free_member = ".uniffi_free = reinterpret_cast<void *>(&uniffi_free)";

                let mut replacements = Vec::new();
                for (start, _) in content.match_indices(vtable_start_hook).collect::<Vec<_>>() {
                    if let Some(end) = content[start..].find(vtable_end_hook) {
                        let vtable_block = &content[start..start + end + vtable_end_hook.len()];

                        if vtable_block.contains(free_member) {
                            let init_start = vtable_block.find('{').unwrap() + 1;
                            let init_end = vtable_block.rfind('}').unwrap();
                            let init_block = &vtable_block[init_start..init_end];

                            let mut members: Vec<&str> = init_block.split(',').map(|s| s.trim()).filter(|s| !s.is_empty()).collect();
                            if let Some(pos) = members.iter().position(|&m| m.contains(".uniffi_free")) {
                                let free = members.remove(pos);
                                members.insert(0, free);

                                let corrected_init = format!("\n            {}\n        ", members.join(",\n            "));
                                let final_block = vtable_block.replace(init_block, &corrected_init);
                                replacements.push((vtable_block.to_string(), final_block));
                            }
                        }
                    }
                }

                if !replacements.is_empty() {
                    for (old, new) in replacements {
                        content = content.replace(&old, &new);
                    }
                    custom_println!("PATCHED", green, "Corrected vtable order in {}", hpp_path.display());
                }
            }

            // Write all accumulated changes to the file.
            fs::write(&hpp_path, content).unwrap_or_else(|_| panic!("Failed to write patched {}", hpp_filename));
        } else {
            warn!("{} not found for patching at {}", hpp_filename, hpp_path.display());
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
}