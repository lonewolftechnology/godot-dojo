# Godot Dojo

Godot Dojo is a cross-platform extension for [Godot Engine](https://godotengine.org/), providing seamless integration with Dojo-based backends and blockchain entities, powered by Rust and C++.  
It enables advanced networking, subscriptions to blockchain events, and a set of utilities for interacting with decentralized data models directly from Godot.

---

## Features

- **Multi-platform:** Build for Linux, Windows, MacOS, and Web (WASM).
- **Blockchain subscriptions:** Create and manage event/entity subscriptions from Godot.
- **Rust & C++ core:** High-performance native integration.
- **Pluggable demo:** Includes a simple demo scene showcasing basic usage.
- 📄 **See also:** [Project State & Support Matrix](project-state.md)

---

## Build Instructions

**Prerequisites:**
- [Godot Engine 4.x](https://godotengine.org/)
- [Rust](https://www.rust-lang.org/tools/install)
- [SCons](https://scons.org/pages/download.html)
- A C++17 compatible compiler (e.g., GCC, Clang, MSVC)
- Python (for SCons)
- `pkg-config` and `libdbus-1-dev` (on Linux)

**Dependencies:**
- Submodules:
    - [`external/dojo.c`](https://github.com/dojoengine/dojo.c) (Rust)
    - [`external/godot-cpp`](https://github.com/godotengine/godot-cpp)

**Step-by-step:**

1. **Clone the repository:**
    ```bash
    git clone --recurse-submodules https://github.com/lonewolftechnology/godot-dojo
    cd godot-dojo
    ```

2. **Check and install dependencies:**
    ```bash
    make check-deps
    ```

    - If anything is missing, follow the on-screen instructions.

3. **Build for your platform:**
    - **Linux:**
      ```bash
      make linux
      ```
    - **Windows:**
      ```bash
      make windows
      ```
    - **MacOS (Intel):**
      ```bash
      make macos-x64
      ```
    - **MacOS (Apple Silicon):**
      ```bash
      make macos-arm64
      ```
    - **WebAssembly:**
      ```bash
      make web
      ```

   The compiled libraries will be output in the `bin/` directory.

---

## Running the Demo

1. **Export or build the addon/library using the steps above.**
2. **Open the project in Godot:**
    - Launch Godot Engine.
    - Open the `godot-dojo` folder as a project.
3. **Run the demo scene:**
    - Open the `Demo` scene under `game` folder.
    - Press the **Play** button.

You should see basic interaction with Dojo backends or a simulated demo, depending on your build and configuration.

---

## Troubleshooting

- If you see errors about missing dependencies, run `make check-deps` and follow the provided suggestions.
- For Rust-related errors:  
  Ensure your Rust toolchain is installed and up to date (`rustup update`).

---

## License

This project is licensed under MIT. See [LICENSE](LICENSE) for details.

---

## Credits

- [Dojo Engine](https://github.com/dojoengine)
- [Godot Engine](https://godotengine.org/)
- All contributors.
