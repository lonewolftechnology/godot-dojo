# Godot Dojo

Godot Dojo is a cross-platform extension for [Godot Engine](https://godotengine.org/), providing seamless integration with Dojo-based backends and blockchain entities, powered by Rust and C++.  
It enables advanced networking, subscriptions to blockchain events, and a set of utilities for interacting with decentralized data models directly from Godot.
During the development of this project, the dojo project used for the demo can be found [here](https://github.com/dannroda/dojo-starter)

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
- Python (required for SCons)
- `pkg-config` and `libdbus-1-dev` (on Linux)

**Dependencies:**
- Submodules:
    - [`external/dojo.c`](https://github.com/dojoengine/dojo.c)
    - [`external/godot-cpp`](https://github.com/godotengine/godot-cpp)

---

**Important note for Windows users:**  
The provided Makefile **is not compatible with Windows**.  
To build on any platform (Linux, Windows, MacOS, WebAssembly) **always use SCons** (`scons ...`) as described in the build instructions below. SCons is cross-platform and the recommended way to build this project.

---

**Step-by-step:**

1. **Clone the repository:**
    ```bash
    git clone --recurse-submodules https://github.com/lonewolftechnology/godot-dojo
    cd godot-dojo
    ```

2. **Verify and install dependencies:**  
   Make sure all prerequisites mentioned above are installed on your system.

3. **Build for your platform:**  
   You can build either in **release** (`template_release`, optimized) or **debug** (`template_debug`, with debug symbols) mode.  
   By default, `template_release` is used if not specified.

    - **Linux:**
      - Debug:
        ```bash
        scons platform=linux target=template_debug
        ```
      - Release:
        ```bash
        scons platform=linux target=template_release
        ```
    - **Windows:**
      - Debug:
        ```bash
        scons platform=windows target=template_debug
        ```
      - Release:
        ```bash
        scons platform=windows target=template_release
        ```
    - **MacOS (Intel):**
      - Debug:
        ```bash
        scons platform=macos arch=x86_64 target=template_debug
        ```
      - Release:
        ```bash
        scons platform=macos arch=x86_64 target=template_release
        ```
    - **MacOS (Apple Silicon):**
      - Debug:
        ```bash
        scons platform=macos arch=arm64 target=template_debug
        ```
      - Release:
        ```bash
        scons platform=macos arch=arm64 target=template_release
        ```
    - **WebAssembly:** (EXPERIMENTAL)
      - Debug:
        ```bash
        scons platform=web target=template_debug
        ```
      - Release:
        ```bash
        scons platform=web target=template_release
        ```

   The compiled libraries will be output to the `bin/` directory.

---

## Running the Demo

1. **Export or build the addon/library using the steps above.**
2. **Open the project in Godot:**
    - Launch Godot Engine.
    - Open the `demo` folder as a project.
3. **Run the demo scene:**
    - Open the `Demo` scene under the `game` folder.
    - Press the **Play** button.

You should see basic interaction with Dojo backends or a simulated demo, depending on your build and configuration.

---

## Troubleshooting

- If you see errors about missing dependencies, ensure all prerequisites are correctly installed.
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
