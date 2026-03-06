#[allow(unused_imports)]
use {
    controller_uniffi,
    dojo_uniffi,
};

// use std::panic;

// #[cfg(not(target_os = "emscripten"))]
// use ctor::ctor;

// Esta función se ejecuta automáticamente al cargar el Wasm, antes que cualquier otra cosa
// #[cfg(not(target_os = "emscripten"))]
// // #[ctor]
// fn init_panic_hook() {
//     register_panic_hook();
// }
//
// // Implementación manual para Emscripten (Wasm)
// #[cfg(target_os = "emscripten")]
// #[used]
// #[no_mangle]
// #[link_section = ".init_array"]
// pub static INIT_PANIC_HOOK: extern "C" fn() = register_panic_hook;
//
// #[no_mangle]
// extern "C" fn register_panic_hook() {
//     // Imprimir inmediatamente para confirmar que este código se ejecuta
//     eprintln!("RUST: Registering panic hook....");
//
//     panic::set_hook(Box::new(|info| {
//         eprintln!("RUST PANIC: {}", info);
//         // Forzar el vaciado del buffer por si el abort ocurre muy rápido
//         use std::io::Write;
//         let _ = std::io::stderr().flush();
//     }));
// }