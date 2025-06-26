# ============================================================================
# Godot Dojo Cross-Platform Makefile
# ============================================================================

.PHONY: help clean build debug release all check-deps install-deps
.PHONY: linux windows macos web
.PHONY: linux-x64 windows-x64 macos-x64 macos-arm64 web-wasm32

# Configuration
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
SCONS := scons -j$(NPROC) -Q

# Host detection
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)
UNAME_M := $(shell uname -m 2>/dev/null || echo x86_64)

# Detect if running on Windows (various environments)
ifneq ($(OS),Windows_NT)
    # Unix-like systems
    HOST_OS := $(shell echo $(UNAME_S) | tr '[:upper:]' '[:lower:]')
    HOST_ARCH := $(UNAME_M)
else
    # Windows
    HOST_OS := windows
    HOST_ARCH := $(PROCESSOR_ARCHITECTURE)
    ifeq ($(HOST_ARCH),AMD64)
        HOST_ARCH := x86_64
    endif
endif

# Additional Windows detection methods
ifeq ($(findstring mingw,$(HOST_OS)),mingw)
    HOST_OS := windows
endif
ifeq ($(findstring msys,$(HOST_OS)),msys)
    HOST_OS := windows
endif
ifeq ($(findstring cygwin,$(HOST_OS)),cygwin)
    HOST_OS := windows
endif

# Map host architecture
ifeq ($(HOST_ARCH),x86_64)
    HOST_ARCH_MAPPED := x64
else ifeq ($(HOST_ARCH),aarch64)
    HOST_ARCH_MAPPED := arm64
else ifeq ($(HOST_ARCH),arm64)
    HOST_ARCH_MAPPED := arm64
else ifeq ($(HOST_ARCH),amd64)
    HOST_ARCH_MAPPED := x64
else
    HOST_ARCH_MAPPED := x64
endif

# Map host OS
ifeq ($(HOST_OS),linux)
    HOST_PLATFORM := linux
else ifeq ($(HOST_OS),darwin)
    HOST_PLATFORM := macos
else ifeq ($(HOST_OS),windows)
    HOST_PLATFORM := windows
else
    HOST_PLATFORM := linux
endif

# Combined host target
HOST_TARGET := $(HOST_PLATFORM)-$(HOST_ARCH_MAPPED)

# Colors (with Windows compatibility)
ifeq ($(HOST_PLATFORM),windows)
    B := 
    G := 
    Y := 
    R := 
    X := 
else
    B := $(shell printf '\033[94m')
    G := $(shell printf '\033[92m')
    Y := $(shell printf '\033[93m')
    R := $(shell printf '\033[91m')
    X := $(shell printf '\033[0m')
endif

# Cross-compilation tools detection
CC_PREFIX_linux_x64 := x86_64-linux-gnu-
CC_PREFIX_windows_x64 := x86_64-w64-mingw32-
CC_PREFIX_macos_x64 := x86_64-apple-darwin-
CC_PREFIX_macos_arm64 := aarch64-apple-darwin-

# ============================================================================
# DEPENDENCY CHECKING
# ============================================================================

check-deps:
	@echo "$(B)🔍 Checking dependencies...$(X)"
	@$(MAKE) --no-print-directory _check-system
	@$(MAKE) --no-print-directory _check-rust
	@$(MAKE) --no-print-directory _check-scons
	@$(MAKE) --no-print-directory _check-dirs
	@$(MAKE) --no-print-directory _check-os-deps
	@echo "$(G)✅ All dependencies ready$(X)"

_check-system:
	@echo "$(Y)  Detected system: $(UNAME_S) $(UNAME_M) -> $(HOST_TARGET)$(X)"
	@echo "$(Y)  Build environment: $(shell $(MAKE) --version 2>/dev/null | head -1)$(X)"

_check-rust:
	@if ! command -v cargo >/dev/null 2>&1; then \
		echo "$(R)❌ Cargo not found$(X)"; \
		echo "$(Y)💡 Install Rust: curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh$(X)"; \
		exit 1; \
	fi
	@echo "$(G)  ✅ Cargo: $(shell cargo --version 2>/dev/null)$(X)"
	@if ! command -v rustup >/dev/null 2>&1; then \
		echo "$(R)❌ Rustup not found$(X)"; \
		exit 1; \
	fi
	@echo "$(G)  ✅ Rustup: $(shell rustup --version 2>/dev/null | head -1)$(X)"

_check-scons:
	@if ! command -v scons >/dev/null 2>&1; then \
		echo "$(R)❌ SCons not found$(X)"; \
		echo "$(Y)💡 Install SCons: pip install scons$(X)"; \
		exit 1; \
	fi
	@echo "$(G)  ✅ SCons: $(shell scons --version 2>/dev/null | grep "SCons:" | cut -d' ' -f2 | sed 's/,//')$(X)"

_check-dirs:
	@if [ ! -d "external/dojo.c" ]; then \
		echo "$(R)❌ Directory external/dojo.c not found$(X)"; \
		exit 1; \
	fi
	@echo "$(G)  ✅ dojo.c directory found$(X)"
	@if [ ! -d "external/godot-cpp" ]; then \
		echo "$(R)❌ Directory external/godot-cpp not found$(X)"; \
		exit 1; \
	fi
	@echo "$(G)  ✅ godot-cpp directory found$(X)"
	@if [ ! -f "external/dojo.c/Cargo.toml" ]; then \
		echo "$(R)❌ Cargo.toml not found in dojo.c$(X)"; \
		exit 1; \
	fi
	@echo "$(G)  ✅ Cargo.toml found$(X)"

_check-os-deps:
	@echo "$(Y)  Checking OS-specific dependencies...$(X)"
ifeq ($(HOST_PLATFORM),linux)
	@$(MAKE) --no-print-directory _check-linux-deps
else ifeq ($(HOST_PLATFORM),macos)
	@$(MAKE) --no-print-directory _check-macos-deps
else ifeq ($(HOST_PLATFORM),windows)
	@$(MAKE) --no-print-directory _check-windows-deps
endif

_check-linux-deps:
	@echo "$(Y)    Linux dependencies:$(X)"
	@if ! pkg-config --exists dbus-1 2>/dev/null; then \
		echo "$(R)    ❌ libdbus-1-dev not found$(X)"; \
		echo "$(Y)    💡 Install: sudo apt-get install libdbus-1-dev$(X)"; \
	else \
		echo "$(G)    ✅ libdbus-1-dev$(X)"; \
	fi
	@if ! command -v gcc >/dev/null 2>&1; then \
		echo "$(R)    ❌ GCC not found$(X)"; \
		echo "$(Y)    💡 Install: sudo apt-get install build-essential$(X)"; \
	else \
		echo "$(G)    ✅ GCC: $(shell gcc --version 2>/dev/null | head -1)$(X)"; \
	fi
	@if ! command -v protoc >/dev/null 2>&1; then \
		echo "$(R)    ❌ protoc (Protocol Buffers compiler) not found$(X)"; \
		echo "$(Y)    💡 Install: sudo apt-get install protobuf-compiler libprotobuf-dev$(X)"; \
	else \
		echo "$(G)    ✅ protoc: $(shell protoc --version 2>/dev/null)$(X)"; \
	fi
	@if ! pkg-config --exists protobuf 2>/dev/null; then \
		echo "$(R)    ❌ libprotobuf-dev not found$(X)"; \
		echo "$(Y)    💡 Install: sudo apt-get install libprotobuf-dev$(X)"; \
	else \
		echo "$(G)    ✅ libprotobuf-dev$(X)"; \
	fi

_check-macos-deps:
	@echo "$(Y)    macOS dependencies:$(X)"
	@if ! command -v clang >/dev/null 2>&1; then \
		echo "$(R)    ❌ Clang not found$(X)"; \
		echo "$(Y)    💡 Install Xcode Command Line Tools: xcode-select --install$(X)"; \
	else \
		echo "$(G)    ✅ Clang: $(shell clang --version 2>/dev/null | head -1)$(X)"; \
	fi
	@if ! command -v pkg-config >/dev/null 2>&1; then \
		echo "$(R)    ❌ pkg-config not found$(X)"; \
		echo "$(Y)    💡 Install: brew install pkg-config$(X)"; \
	else \
		echo "$(G)    ✅ pkg-config$(X)"; \
	fi

_check-windows-deps:
	@echo "$(Y)    Windows dependencies:$(X)"
	@if ! command -v cl >/dev/null 2>&1 && ! command -v gcc >/dev/null 2>&1; then \
		echo "$(R)    ❌ No compiler found (MSVC or MinGW)$(X)"; \
		echo "$(Y)    💡 Install Visual Studio Build Tools or MinGW-w64$(X)"; \
	else \
		if command -v cl >/dev/null 2>&1; then \
			echo "$(G)    ✅ MSVC compiler found$(X)"; \
		fi; \
		if command -v gcc >/dev/null 2>&1; then \
			echo "$(G)    ✅ MinGW GCC: $(shell gcc --version 2>/dev/null | head -1)$(X)"; \
		fi; \
	fi

install-deps:
	@echo "$(B)📦 Installing dependencies...$(X)"
	@$(MAKE) --no-print-directory _install-rust-targets
	@$(MAKE) --no-print-directory _install-cross-tools
	@$(MAKE) --no-print-directory _install-os-deps
	@echo "$(G)✅ Dependencies installed$(X)"

_install-rust-targets:
	@echo "$(Y)🦀 Installing Rust targets...$(X)"
	@rustup target add x86_64-unknown-linux-gnu 2>/dev/null || true
	@rustup target add x86_64-pc-windows-gnu 2>/dev/null || true
	@rustup target add x86_64-pc-windows-msvc 2>/dev/null || true
	@rustup target add x86_64-apple-darwin 2>/dev/null || true
	@rustup target add aarch64-apple-darwin 2>/dev/null || true
	@rustup target add wasm32-unknown-unknown 2>/dev/null || true
	@rustup component add rustfmt clippy 2>/dev/null || true
	@echo "$(G)  ✅ Rust targets and components installed$(X)"

_install-cross-tools:
	@echo "$(Y)🔧 Installing cross-compilation tools...$(X)"
ifeq ($(HOST_PLATFORM),linux)
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "$(Y)  Installing cross-compilation toolchains (Ubuntu/Debian)...$(X)"; \
		sudo apt-get update >/dev/null 2>&1 || true; \
		sudo apt-get install -y gcc-mingw-w64-x86-64 >/dev/null 2>&1 || true; \
	elif command -v yum >/dev/null 2>&1; then \
		echo "$(Y)  Installing cross-compilation toolchains (RHEL/CentOS)...$(X)"; \
		sudo yum install -y mingw64-gcc >/dev/null 2>&1 || true; \
	elif command -v pacman >/dev/null 2>&1; then \
		echo "$(Y)  Installing cross-compilation toolchains (Arch)...$(X)"; \
		sudo pacman -S --noconfirm mingw-w64-gcc >/dev/null 2>&1 || true; \
	fi
else ifeq ($(HOST_PLATFORM),macos)
	@if command -v brew >/dev/null 2>&1; then \
		echo "$(Y)  Installing cross-compilation toolchains (macOS)...$(X)"; \
		brew install mingw-w64 >/dev/null 2>&1 || true; \
	fi
endif
	@echo "$(G)  ✅ Cross-compilation tools installed$(X)"

_install-os-deps:
	@echo "$(Y)📦 Installing OS-specific dependencies...$(X)"
ifeq ($(HOST_PLATFORM),linux)
	@if command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get install -y build-essential pkg-config libdbus-1-dev protobuf-compiler libprotobuf-dev >/dev/null 2>&1 || true; \
	elif command -v yum >/dev/null 2>&1; then \
		sudo yum install -y gcc-c++ pkgconfig dbus-devel protobuf-compiler protobuf-devel >/dev/null 2>&1 || true; \
	elif command -v pacman >/dev/null 2>&1; then \
		sudo pacman -S --noconfirm base-devel pkgconf dbus protobuf >/dev/null 2>&1 || true; \
	fi
else ifeq ($(HOST_PLATFORM),macos)
	@if command -v brew >/dev/null 2>&1; then \
		brew install pkg-config >/dev/null 2>&1 || true; \
	fi
else ifeq ($(HOST_PLATFORM),windows)
	@echo "$(Y)  Windows: Please ensure you have Visual Studio Build Tools or MinGW-w64 installed$(X)"
endif
	@echo "$(G)  ✅ OS dependencies installed$(X)"

# ============================================================================
# MAIN COMMANDS
# ============================================================================

help:
	@echo "$(B)🚀 Godot Dojo Cross-Platform Build System$(X)"
	@echo "$(Y)  Host detected: $(HOST_TARGET)$(X)"
	@echo ""
	@echo "$(G)Main commands:$(X)"
	@echo "  make build        - Build everything (debug + release) for host"
	@echo "  make debug        - Build debug for host ($(HOST_TARGET))"
	@echo "  make release      - Build release for host ($(HOST_TARGET))"
	@echo "  make clean        - Clean all build files"
	@echo ""
	@echo "$(G)Dependencies:$(X)"
	@echo "  make check-deps   - Check all dependencies"
	@echo "  make install-deps - Install missing dependencies"
	@echo ""
	@echo "$(G)Complete platform builds:$(X)"
	@echo "  make linux-all    - Linux x64 debug + release"
	@echo "  make windows-all  - Windows x64 debug + release"
	@echo "  make macos-all    - macOS (x64 + arm64) debug + release"
	@echo "  make web-all      - WebAssembly debug + release $(R)[EXPERIMENTAL - NON-FUNCTIONAL]$(X)"
	@echo ""
	@echo "$(G)Platform debug/release only:$(X)"
	@echo "  make linux-debug    make linux-release"
	@echo "  make windows-debug  make windows-release"  
	@echo "  make macos-debug    make macos-release"
	@echo "  make web-debug      make web-release $(R)[EXPERIMENTAL]$(X)"
	@echo ""
	@echo "$(G)Architecture-specific builds:$(X)"
	@echo "  make linux-x64-debug     make linux-x64-release"
	@echo "  make windows-x64-debug   make windows-x64-release"
	@echo "  make macos-x64-debug     make macos-x64-release"
	@echo "  make macos-arm64-debug   make macos-arm64-release"
	@echo "  make web-wasm32-debug    make web-wasm32-release $(R)[EXPERIMENTAL]$(X)"
	@echo ""
	@echo "$(G)Shortcuts:$(X)"
	@echo "  make linux-x64    - Linux x64 (debug + release)"
	@echo "  make macos-arm64  - macOS ARM64 (debug + release)"
	@echo "  make windows-x64  - Windows x64 (debug + release)"
	@echo ""
	@echo "$(R)⚠️  Web builds are experimental and currently non-functional$(X)"

build: check-deps debug release

# Debug/release commands build only for the host
debug: check-deps $(HOST_TARGET)-debug
	@echo "$(G)✅ Debug build completed for $(HOST_TARGET)$(X)"

release: check-deps $(HOST_TARGET)-release
	@echo "$(G)✅ Release build completed for $(HOST_TARGET)$(X)"

clean:
	@echo "$(Y)🧹 Cleaning...$(X)"
	@$(SCONS) --clean 2>/dev/null || true
	@rm -rf demo/bin/* 2>/dev/null || true
	@cd external/dojo.c && cargo clean 2>/dev/null || true
	@cd external/godot-cpp && $(SCONS) --clean 2>/dev/null || true
	@echo "$(G)✅ Clean complete$(X)"

# ============================================================================
# COMPLETE PLATFORM BUILDS (all architectures)
# ============================================================================

linux-all: check-deps linux-debug linux-release
linux-debug: check-deps linux-x64-debug
linux-release: check-deps linux-x64-release

windows-all: check-deps windows-debug windows-release
windows-debug: check-deps windows-x64-debug
windows-release: check-deps windows-x64-release

macos-all: check-deps macos-debug macos-release
macos-debug: check-deps macos-x64-debug macos-arm64-debug
macos-release: check-deps macos-x64-release macos-arm64-release

web-all: check-deps web-debug web-release
	@echo "$(R)⚠️  Web builds completed but are experimental and may not function correctly$(X)"

web-debug: check-deps web-wasm32-debug
	@echo "$(R)⚠️  Web debug build is experimental$(X)"

web-release: check-deps web-wasm32-release
	@echo "$(R)⚠️  Web release build is experimental$(X)"

# ============================================================================
# SPECIFIC ARCHITECTURE BUILDS
# ============================================================================

# Linux builds
linux-x64-debug: check-deps
	@echo "$(B)🐧 Building Linux x64 Debug$(X)"
	@$(SCONS) platform=linux arch=x86_64 target=template_debug

linux-x64-release: check-deps
	@echo "$(B)🐧 Building Linux x64 Release$(X)"
	@$(SCONS) platform=linux arch=x86_64 target=template_release

# Windows builds
windows-x64-debug: check-deps
	@echo "$(B)🪟 Building Windows x64 Debug$(X)"
	@$(SCONS) platform=windows arch=x86_64 target=template_debug

windows-x64-release: check-deps
	@echo "$(B)🪟 Building Windows x64 Release$(X)"
	@$(SCONS) platform=windows arch=x86_64 target=template_release

# macOS builds
macos-x64-debug: check-deps
	@echo "$(B)🍎 Building macOS x64 Debug$(X)"
	@$(SCONS) platform=macos arch=x86_64 target=template_debug

macos-x64-release: check-deps
	@echo "$(B)🍎 Building macOS x64 Release$(X)"
	@$(SCONS) platform=macos arch=x86_64 target=template_release

macos-arm64-debug: check-deps
	@echo "$(B)🍎 Building macOS ARM64 Debug$(X)"
	@$(SCONS) platform=macos arch=arm64 target=template_debug

macos-arm64-release: check-deps
	@echo "$(B)🍎 Building macOS ARM64 Release$(X)"
	@$(SCONS) platform=macos arch=arm64 target=template_release

# Web builds (experimental)
web-wasm32-debug: check-deps
	@echo "$(B)🌐 Building Web WASM32 Debug $(R)[EXPERIMENTAL - MAY NOT WORK]$(X)"
	@echo "$(Y)⚠️  Web compilation is experimental and currently non-functional$(X)"
	@rustup target add wasm32-unknown-unknown 2>/dev/null || true
	@$(SCONS) platform=web arch=wasm32 target=template_debug || echo "$(R)❌ Web build failed (expected - experimental feature)$(X)"

web-wasm32-release: check-deps
	@echo "$(B)🌐 Building Web WASM32 Release $(R)[EXPERIMENTAL - MAY NOT WORK]$(X)"
	@echo "$(Y)⚠️  Web compilation is experimental and currently non-functional$(X)"
	@rustup target add wasm32-unknown-unknown 2>/dev/null || true
	@$(SCONS) platform=web arch=wasm32 target=template_release || echo "$(R)❌ Web build failed (expected - experimental feature)$(X)"

# ============================================================================
# ARCHITECTURE SHORTCUTS (debug + release)
# ============================================================================

linux-x64: check-deps linux-x64-debug linux-x64-release
	@echo "$(G)✅ Linux x64 complete$(X)"

windows-x64: check-deps windows-x64-debug windows-x64-release
	@echo "$(G)✅ Windows x64 complete$(X)"

macos-x64: check-deps macos-x64-debug macos-x64-release
	@echo "$(G)✅ macOS x64 complete$(X)"

macos-arm64: check-deps macos-arm64-debug macos-arm64-release
	@echo "$(G)✅ macOS ARM64 complete$(X)"

web-wasm32: check-deps web-wasm32-debug web-wasm32-release
	@echo "$(R)⚠️  Web WASM32 complete (experimental - may not function)$(X)"

# ============================================================================
# SPECIAL TARGETS
# ============================================================================

all: build

ci-all: check-deps linux-all windows-all macos-all
	@echo "$(G)🎉 All stable platforms built successfully!$(X)"
	@echo "$(Y)📝 Note: Web builds excluded from CI (experimental)$(X)"

ci-all-experimental: check-deps linux-all windows-all macos-all web-all
	@echo "$(G)🎉 All platforms built (including experimental web)!$(X)"

list-targets:
	@echo "$(B)Available build targets:$(X)"
	@echo "$(G)Host-specific:$(X) debug, release, build"
	@echo "$(G)Linux:$(X) linux-x64-debug, linux-x64-release"
	@echo "$(G)Windows:$(X) windows-x64-debug, windows-x64-release"
	@echo "$(G)macOS:$(X) macos-x64-debug, macos-x64-release, macos-arm64-debug, macos-arm64-release"
	@echo "$(R)Web (experimental):$(X) web-wasm32-debug, web-wasm32-release"

.DEFAULT_GOAL := help