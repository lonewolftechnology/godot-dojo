# ============================================================================
# Godot Dojo Makefile - Versión Actualizada con Builds Separados
# ============================================================================

.PHONY: help build clean test format lint docker dev
.PHONY: debug release all-debug all-release
.PHONY: linux-debug linux-release windows-debug windows-release
.PHONY: macos-debug macos-release web-debug web-release
.PHONY: linux-x64-debug linux-x64-release linux-arm64-debug linux-arm64-release
.PHONY: windows-x64-debug windows-x64-release
.PHONY: macos-x64-debug macos-x64-release macos-arm64-debug macos-arm64-release
.PHONY: web-wasm32-debug web-wasm32-release

# Configuración básica
PROJECT_NAME := godot-dojo
NPROC := $(shell nproc 2>/dev/null || echo 4)
DEMO_BIN_DIR := demo/bin

# Configuración de targets
PLATFORMS := linux windows macos web
LINUX_ARCHS := x86_64 arm64
WINDOWS_ARCHS := x86_64
MACOS_ARCHS := x86_64 arm64
WEB_ARCHS := wasm32
TARGETS := template_debug template_release

# Colores
ifneq (,$(shell command -v tput))
	GREEN := $(shell tput setaf 2)
	YELLOW := $(shell tput setaf 3)
	RED := $(shell tput setaf 1)
	BLUE := $(shell tput setaf 4)
	CYAN := $(shell tput setaf 6)
	RESET := $(shell tput sgr0)
	BOLD := $(shell tput bold)
else
	GREEN := 
	YELLOW := 
	RED := 
	BLUE := 
	CYAN := 
	RESET := 
	BOLD := 
endif

# ============================================================================
# AYUDA
# ============================================================================

help:
	@echo "$(BLUE)$(BOLD)🚀 $(PROJECT_NAME) - Build System Avanzado$(RESET)"
	@echo ""
	@echo "$(GREEN)$(BOLD)📋 Comandos principales:$(RESET)"
	@echo "  make build         - Construir todo (debug + release, todas las plataformas)"
	@echo "  make debug         - Construir solo modo debug (todas las plataformas)"
	@echo "  make release       - Construir solo modo release (todas las plataformas)"
	@echo "  make clean         - Limpiar archivos compilados"
	@echo "  make test          - Ejecutar tests"
	@echo "  make format        - Formatear código"
	@echo "  make lint          - Linter de código"
	@echo ""
	@echo "$(GREEN)$(BOLD)🎯 Builds por plataforma:$(RESET)"
	@echo "  make linux-debug   - Linux debug (todas las arquitecturas)"
	@echo "  make linux-release - Linux release (todas las arquitecturas)"
	@echo "  make windows-debug - Windows debug"
	@echo "  make windows-release - Windows release"
	@echo "  make macos-debug   - macOS debug (x64 + ARM64)"
	@echo "  make macos-release - macOS release (x64 + ARM64)"
	@echo "  make web-debug     - Web debug"
	@echo "  make web-release   - Web release"
	@echo ""
	@echo "$(GREEN)$(BOLD)🏗️ Builds específicos por arquitectura:$(RESET)"
	@echo "  make linux-x64-debug      - Linux x86_64 debug"
	@echo "  make linux-x64-release    - Linux x86_64 release"
	@echo "  make linux-arm64-debug    - Linux ARM64 debug"
	@echo "  make linux-arm64-release  - Linux ARM64 release"
	@echo "  make windows-x64-debug    - Windows x86_64 debug"
	@echo "  make windows-x64-release  - Windows x86_64 release"
	@echo "  make macos-x64-debug      - macOS x86_64 debug"
	@echo "  make macos-x64-release    - macOS x86_64 release"
	@echo "  make macos-arm64-debug    - macOS ARM64 debug"
	@echo "  make macos-arm64-release  - macOS ARM64 release"
	@echo "  make web-wasm32-debug     - Web WASM32 debug"
	@echo "  make web-wasm32-release   - Web WASM32 release"
	@echo ""
	@echo "$(GREEN)$(BOLD)🔧 Utilidades:$(RESET)"
	@echo "  make dev           - Configurar entorno desarrollo"
	@echo "  make docker        - Construir imagen Docker"
	@echo "  make status        - Mostrar estado del proyecto"

# ============================================================================
# BUILDS PRINCIPALES
# ============================================================================

build: debug release
	@echo "$(GREEN)$(BOLD)✅ Build completo terminado$(RESET)"

debug: all-debug
	@echo "$(GREEN)$(BOLD)✅ Todos los builds debug completados$(RESET)"

release: all-release
	@echo "$(GREEN)$(BOLD)✅ Todos los builds release completados$(RESET)"

all-debug: linux-debug windows-debug macos-debug web-debug

all-release: linux-release windows-release macos-release web-release

# ============================================================================
# BUILDS POR PLATAFORMA
# ============================================================================

linux-debug: linux-x64-debug linux-arm64-debug
	@echo "$(GREEN)✅ Linux debug builds completados$(RESET)"

linux-release: linux-x64-release linux-arm64-release
	@echo "$(GREEN)✅ Linux release builds completados$(RESET)"

windows-debug: windows-x64-debug
	@echo "$(GREEN)✅ Windows debug builds completados$(RESET)"

windows-release: windows-x64-release
	@echo "$(GREEN)✅ Windows release builds completados$(RESET)"

macos-debug: macos-x64-debug macos-arm64-debug
	@echo "$(GREEN)✅ macOS debug builds completados$(RESET)"

macos-release: macos-x64-release macos-arm64-release
	@echo "$(GREEN)✅ macOS release builds completados$(RESET)"

web-debug: web-wasm32-debug
	@echo "$(GREEN)✅ Web debug builds completados$(RESET)"

web-release: web-wasm32-release
	@echo "$(GREEN)✅ Web release builds completados$(RESET)"

# ============================================================================
# BUILDS ESPECÍFICOS POR ARQUITECTURA
# ============================================================================

# Linux builds
linux-x64-debug:
	@echo "$(BLUE)🐧 Building Linux x86_64 Debug...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=linux arch=x86_64 target=template_debug -j$(NPROC) -Q

linux-x64-release:
	@echo "$(BLUE)🐧 Building Linux x86_64 Release...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=linux arch=x86_64 target=template_release -j$(NPROC) -Q

linux-arm64-debug:
	@echo "$(BLUE)🐧 Building Linux ARM64 Debug...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=linux arch=arm64 target=template_debug -j$(NPROC) -Q

linux-arm64-release:
	@echo "$(BLUE)🐧 Building Linux ARM64 Release...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=linux arch=arm64 target=template_release -j$(NPROC) -Q

# Windows builds
windows-x64-debug:
	@echo "$(BLUE)🪟 Building Windows x86_64 Debug...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=windows arch=x86_64 target=template_debug use_mingw=yes -j$(NPROC) -Q

windows-x64-release:
	@echo "$(BLUE)🪟 Building Windows x86_64 Release...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=windows arch=x86_64 target=template_release use_mingw=yes -j$(NPROC) -Q

# macOS builds
macos-x64-debug:
	@echo "$(BLUE)🍎 Building macOS x86_64 Debug...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=macos arch=x86_64 target=template_debug -j$(NPROC) -Q

macos-x64-release:
	@echo "$(BLUE)🍎 Building macOS x86_64 Release...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=macos arch=x86_64 target=template_release -j$(NPROC) -Q

macos-arm64-debug:
	@echo "$(BLUE)🍎 Building macOS ARM64 Debug...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=macos arch=arm64 target=template_debug -j$(NPROC) -Q

macos-arm64-release:
	@echo "$(BLUE)🍎 Building macOS ARM64 Release...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@scons platform=macos arch=arm64 target=template_release -j$(NPROC) -Q

# Web builds
web-wasm32-debug:
	@echo "$(BLUE)🌐 Building Web WASM32 Debug...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@rustup target add wasm32-unknown-unknown 2>/dev/null || true
	@scons platform=web arch=wasm32 target=template_debug -j$(NPROC) -Q

web-wasm32-release:
	@echo "$(BLUE)🌐 Building Web WASM32 Release...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@rustup target add wasm32-unknown-unknown 2>/dev/null || true
	@scons platform=web arch=wasm32 target=template_release -j$(NPROC) -Q

# ============================================================================
# UTILIDADES
# ============================================================================

clean:
	@echo "$(YELLOW)🧹 Limpiando archivos compilados...$(RESET)"
	@rm -rf $(DEMO_BIN_DIR)/* 2>/dev/null || true
	@scons --clean 2>/dev/null || true
	@if [ -d "external/dojo.c" ]; then \
		echo "$(YELLOW)🧹 Limpiando Rust...$(RESET)"; \
		cd external/dojo.c && cargo clean; \
	fi
	@if [ -d "external/godot-cpp" ]; then \
		echo "$(YELLOW)🧹 Limpiando Godot C++...$(RESET)"; \
		scons -C external/godot-cpp --clean 2>/dev/null || true; \
	fi
	@echo "$(GREEN)✅ Limpieza completada$(RESET)"

test:
	@echo "$(BLUE)🧪 Ejecutando tests...$(RESET)"
	@if [ -d "external/dojo.c" ]; then \
		cd external/dojo.c && cargo test --all-targets; \
	else \
		echo "$(YELLOW)⚠️ Directorio dojo.c no encontrado$(RESET)"; \
	fi

format:
	@echo "$(BLUE)🎨 Formateando código...$(RESET)"
	@if [ -d "external/dojo.c" ]; then \
		echo "$(CYAN)  📝 Formateando Rust...$(RESET)"; \
		cd external/dojo.c && cargo fmt; \
	fi
	@if command -v clang-format >/dev/null 2>&1; then \
		echo "$(CYAN)  📝 Formateando C++...$(RESET)"; \
		find src/ include/ -name "*.cpp" -o -name "*.hpp" -o -name "*.h" 2>/dev/null | xargs clang-format -i; \
	else \
		echo "$(YELLOW)⚠️ clang-format no está instalado$(RESET)"; \
	fi
	@echo "$(GREEN)✅ Formateo completado$(RESET)"

lint:
	@echo "$(BLUE)🔍 Ejecutando linter...$(RESET)"
	@if [ -d "external/dojo.c" ]; then \
		echo "$(CYAN)  🔍 Linting Rust...$(RESET)"; \
		cd external/dojo.c && cargo clippy --all-targets -- -D warnings; \
	fi
	@if command -v cppcheck >/dev/null 2>&1; then \
		echo "$(CYAN)  🔍 Linting C++...$(RESET)"; \
		cppcheck --enable=all --std=c++17 src/ include/ 2>/dev/null || true; \
	fi

dev:
	@echo "$(BLUE)🛠️ Configurando entorno desarrollo...$(RESET)"
	@mkdir -p $(DEMO_BIN_DIR)
	@echo "$(CYAN)  🦀 Configurando targets Rust...$(RESET)"
	@rustup target add x86_64-unknown-linux-gnu 2>/dev/null || true
	@rustup target add aarch64-unknown-linux-gnu 2>/dev/null || true
	@rustup target add x86_64-pc-windows-gnu 2>/dev/null || true
	@rustup target add x86_64-apple-darwin 2>/dev/null || true
	@rustup target add aarch64-apple-darwin 2>/dev/null || true
	@rustup target add wasm32-unknown-unknown 2>/dev/null || true
	@rustup component add rustfmt clippy 2>/dev/null || true
	@echo "$(GREEN)✅ Entorno configurado$(RESET)"

docker:
	@echo "$(BLUE)🐳 Construyendo imagen Docker...$(RESET)"
	@if command -v docker >/dev/null 2>&1; then \
		docker build -t $(PROJECT_NAME) .; \
		echo "$(GREEN)✅ Imagen Docker creada: $(PROJECT_NAME)$(RESET)"; \
	else \
		echo "$(RED)❌ Docker no está instalado$(RESET)"; \
	fi

status:
	@echo "$(BLUE)$(BOLD)📊 Estado del Proyecto $(PROJECT_NAME)$(RESET)"
	@echo ""
	@echo "$(CYAN)📁 Estructura de directorios:$(RESET)"
	@ls -la $(DEMO_BIN_DIR)/ 2>/dev/null || echo "  $(YELLOW)⚠️ Directorio bin vacío o no existe$(RESET)"
	@echo ""
	@echo "$(CYAN)🦀 Información Rust:$(RESET)"
	@if [ -d "external/dojo.c" ]; then \
		cd external/dojo.c && cargo --version 2>/dev/null || echo "  $(RED)❌ Cargo no disponible$(RESET)"; \
		cd external/dojo.c && rustup show active-toolchain 2>/dev/null || echo "  $(RED)❌ Rustup no disponible$(RESET)"; \
	else \
		echo "  $(YELLOW)⚠️ Directorio dojo.c no encontrado$(RESET)"; \
	fi
	@echo ""
	@echo "$(CYAN)🔧 Herramientas disponibles:$(RESET)"
	@command -v scons >/dev/null 2>&1 && echo "  ✅ SCons" || echo "  ❌ SCons"
	@command -v cargo >/dev/null 2>&1 && echo "  ✅ Cargo" || echo "  ❌ Cargo"
	@command -v clang-format >/dev/null 2>&1 && echo "  ✅ clang-format" || echo "  ❌ clang-format"
	@command -v cppcheck >/dev/null 2>&1 && echo "  ✅ cppcheck" || echo "  ❌ cppcheck"
	@command -v docker >/dev/null 2>&1 && echo "  ✅ Docker" || echo "  ❌ Docker"

# ============================================================================
# CONFIGURACIÓN AVANZADA
# ============================================================================

# Usar bash y configuración estricta
SHELL := /bin/bash
MAKEFLAGS += --no-builtin-rules --no-builtin-variables

# Configuración de paralelismo por defecto
.DEFAULT_GOAL := help

# Prevenir eliminación de archivos intermedios
.SECONDARY:

# Variables de debugging
ifdef VERBOSE
	SCONS_FLAGS += verbose=yes
endif

ifdef DEBUG_MAKE
	OLD_SHELL := $(SHELL)
	SHELL = $(warning Building $@$(if $<, (from $<))$(if $?, ($? newer)))$(OLD_SHELL)
endif