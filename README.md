# Godot Dojo


**Godot Dojo** is a cross-platform GDExtension for the [Godot Engine](https://godotengine.org/), providing seamless integration with [Dojo](https://dojoengine.org/)-based backends and blockchain entities. Powered by Rust and C++, it enables advanced networking, subscriptions to blockchain events, and a set of utilities for interacting with decentralized data models directly from Godot.

> Note:
> The demo project for this extension can be found in the [dojo-starter-godot-project](./dojo-starter-godot-project) directory. A zip file of the starter project is also included with each [release](https://github.com/lonewolftechnology/godot-dojo/releases).

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

-   **Multi-platform:** Build for Linux (x86_64), Windows (x86_64), macOS (x86_64, arm64), Android (arm64), and iOS (universal).
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
    -   [`external/godot-cpp`](https://github.com/godotengine/godot-cpp)
-   **Core Crate:**
    -   The `godot-dojo-core` crate combines `dojo.c` and `controller.c` into a single library to prevent duplicate symbols when linking.

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
    git clone --recurse-submodules https
    cd godot-dojo
    ```

2.  **Verify and install dependencies:**
    Make sure all prerequisites mentioned above are installed on your system.

3.  **Build for your platform:**
    
    ### Build Targets
    There are three build targets available:
    -   `target=editor`: **Required for use within the Godot Editor.** This build is necessary for the GDExtension to be recognized and used during development.
    -   `target=template_debug`: For exported games with debug symbols. Use this for testing and debugging your exported game.
    -   `target=template_release`: For final, optimized game exports.
    
    By default, `scons` will use `target=template_debug` if no target is specified.

    <details>
    <summary><strong>Build Commands</strong></summary>

    -   **Linux:**
        -   Editor:
            ```bash
            scons platform=linux target=editor
            ```
        -   Debug Export:
            ```bash
            scons platform=linux target=template_debug
            ```
        -   Release Export:
            ```bash
            scons platform=linux target=template_release
            ```
    -   **Windows:**
        -   Editor:
            ```bash
            scons platform=windows target=editor
            ```
        -   Debug Export:
            ```bash
            scons platform=windows target=template_debug
            ```
        -   Release Export:
            ```bash
            scons platform=windows target=template_release
            ```
    -   **MacOS (Intel):**
        -   Editor:
            ```bash
            scons platform=macos arch=x86_64 target=editor
            ```
        -   Debug Export:
            ```bash
            scons platform=macos arch=x86_64 target=template_debug
            ```
        -   Release Export:
            ```bash
            scons platform=macos arch=x86_64 target=template_release
            ```
    -   **MacOS (Apple Silicon):**
        -   Editor:
            ```bash
            scons platform=macos arch=arm64 target=editor
            ```
        -   Debug Export:
            ```bash
            scons platform=macos arch=arm64 target=template_debug
            ```
        -   Release Export:
            ```bash
            scons platform=macos arch=arm64 target=template_release
            ```
    -   **Android:** (No editor build for Android)
        -   Debug Export:
            ```bash
            scons platform=android arch=arm64 target=template_debug
            ```
        -   Release Export:
            ```bash
            scons platform=android arch=arm64 target=template_release
            ```
    -   **iOS:** (No editor build for iOS)
        -   Debug Export:
            ```bash
            scons platform=ios arch=universal target=template_debug
            ```
        -   Release Export:
            ```bash
            scons platform=ios arch=universal target=template_release
            ```
    </details>

    The compiled libraries will be output to the `demo/addons/godot-dojo/` directory.

---

## 🚀 Running the Demo

> **Note:** The `dojo-starter-godot-project` uses **Godot 4.5**.

1.  **Get the addon:**
    - **Option A (Recommended): Download from a release:** Download the `dojo-starter-godot-project.zip` from the [latest release](https://github.com/lonewolftechnology/godot-dojo/releases). The addon is already included.
    - **Option B: Build from source:** Follow the [Build Instructions](#️-build-instructions) above to build for `target=editor`. The compiled addon will be located in the `demo/addons/godot-dojo` directory. Then, copy the `godot-dojo` directory into the `dojo-starter-godot-project/addons` directory.
    - **Option C: Manual download:** Download the `godot-dojo-*.zip` from the [latest release](https://github.com/lonewolftechnology/godot-dojo/releases), extract it, and copy the `godot-dojo` directory into the `dojo-starter-godot-project/addons` directory.

2.  **Open the project in Godot:**
    -   Launch Godot Engine.
    -   Open the `dojo-starter-godot-project` folder as a project.
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
-   [Cartridge Controller](https://github.com/cartridge-gg/controller.c)
-   All contributors.
