# Godot Dojo


**Godot Dojo** is a cross-platform GDExtension for the [Godot Engine](https://godotengine.org/), providing seamless integration with [Dojo](https://dojoengine.org/)-based backends and blockchain entities. Powered by Rust and C++, it enables advanced networking, subscriptions to blockchain events, and a set of utilities for interacting with decentralized data models directly from Godot.

> Note:
> During the development of this project, the dojo project used for the demo can be found [here](https://github.com/dannroda/dojo-starter).

---

## 📋 Table of Contents

- [✨ Features](#-features)
- [🛠️ Build Instructions](#️-build-instructions)
- [🚀 Running the Demo](#-running-the-demo)
- [❓ Troubleshooting](#-troubleshooting)
- [📜 License](#-license)
- [🙏 Credits](#-credits)

---

## ✨ Features

-   **Multi-platform:** Build for Linux, Windows, MacOS, and Web (WASM).
-   **Blockchain subscriptions:** Create and manage event/entity subscriptions from Godot.
-   **Rust & C++ core:** High-performance native integration.
-   **Pluggable demo:** Includes a simple demo scene showcasing basic usage.
-   📄 **See also:** [Project State & Support Matrix](project-state.md)

---

## 🛠️ Build Instructions

<details>
<summary><strong>Prerequisites</strong></summary>

-   [Godot Engine 4.x](https://godotengine.org/)
-   [Rust](https://www.rust-lang.org/tools/install)
-   [SCons](https://scons.org/pages/download.html)
-   A C++17 compatible compiler (e.g., GCC, Clang, MSVC)
-   Python (required for SCons)
-   `pkg-config` and `libdbus-1-dev` (on Linux)

</details>

<details>
<summary><strong>Dependencies</strong></summary>

-   **Submodules:**
    -   [`external/dojo.c`](https://github.com/dojoengine/dojo.c)
    -   [`external/godot-cpp`](https://github.com/godotengine/godot-cpp)

</details>

> Important:
> We are currently building against Godot 4.3, but 4.2.2 should be supported for a while.
> If you want to build against 4.2.2, you need to checkout the `godot-4.2.2` branch of the `godot-cpp` submodule. Please submit an issue or PR if you find any problems.

> Warning:
> The provided Makefile **is not compatible with Windows**.
> To build on any platform (Linux, Windows, MacOS, WebAssembly) **always use SCons** (`scons ...`) as described in the build instructions below. SCons is cross-platform and the recommended way to build this project.

### Step-by-step

1.  **Clone the repository:**
    ```bash
    git clone --recurse-submodules https://github.com/lonewolftechnology/godot-dojo
    cd godot-dojo
    ```

2.  **Verify and install dependencies:**
    Make sure all prerequisites mentioned above are installed on your system.

3.  **Build for your platform:**
    You can build either in **release** (`template_release`, optimized) or **debug** (`template_debug`, with debug symbols) mode. These names match Godot's export template names and refer to the final project artifacts that Godot will load, not the editor build. By default, `template_debug` is used if not specified.

    <details>
    <summary><strong>Build Commands</strong></summary>

    -   **Linux:**
        -   Debug:
            ```bash
            scons platform=linux target=template_debug
            ```
        -   Release:
            ```bash
            scons platform=linux target=template_release
            ```
    -   **Windows:**
        -   Debug:
            ```bash
            scons platform=windows target=template_debug
            ```
        -   Release:
            ```bash
            scons platform=windows target=template_release
            ```
    -   **MacOS (Intel):**
        -   Debug:
            ```bash
            scons platform=macos arch=x86_64 target=template_debug
            ```
        -   Release:
            ```bash
            scons platform=macos arch=x86_64 target=template_release
            ```
    -   **MacOS (Apple Silicon):**
        -   Debug:
            ```bash
            scons platform=macos arch=arm64 target=template_debug
            ```
        -   Release:
            ```bash
            scons platform=macos arch=arm64 target=template_release
            ```
    -   **WebAssembly:** (EXPERIMENTAL - UNFINISHED)
        -   Debug:
            ```bash
            scons platform=web target=template_debug
            ```
        -   Release:
            ```bash
            scons platform=web target=template_release
            ```
    </details>

    The compiled libraries will be output to the `demo/addons/godot-dojo/` directory.

---

## 🚀 Running the Demo

1.  **Export or build the addon/library using the steps above.**
2.  **Open the project in Godot:**
    -   Launch Godot Engine.
    -   Open the `demo` folder as a project.
3.  **Run the demo scene:**
    -   Open the `Demo` scene under the `game` folder.
    -   Press the **Play** button.

You should see basic interaction with Dojo backends or a simulated demo, depending on your build and configuration.

---

## ❓ Troubleshooting

-   If you see errors about missing dependencies, ensure all prerequisites are correctly installed.
-   For Rust-related errors:
    Ensure your Rust toolchain is installed and up to date (`rustup update`).

---

## 📜 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---

## 🙏 Credits

-   [Dojo Engine](https://github.com/dojoengine)
-   [Godot Engine](https://godotengine.org/)
-   All contributors.
