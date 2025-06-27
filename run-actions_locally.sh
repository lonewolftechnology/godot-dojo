#!/bin/bash

# Configuración
CACHE_DIR=".cache"
RESULTS_DIR="./results"

# Crear directorios de cache
mkdir -p ${CACHE_DIR}/{scons-cache,cargo-registry,cargo-git}
mkdir -p ${RESULTS_DIR}

# Setup de variables de entorno para cache
export SCONS_CACHE_DIR=$(pwd)/.scons_cache
export CARGO_HOME=$(pwd)/${CACHE_DIR}/cargo

# Detectar arquitectura y configurar imágenes
HOST_ARCH=$(uname -m)
echo "🔍 Host Architecture: $HOST_ARCH"

if [[ "$HOST_ARCH" == "aarch64" || "$HOST_ARCH" == "arm64" ]]; then
    echo "🚀 Configuring for ARM64 host"
    
    # Habilitar emulación multi-arch si es necesario
    if ! docker run --rm --privileged multiarch/qemu-user-static --reset -p yes 2>/dev/null; then
        echo "⚠️  Could not setup multi-arch emulation (might not be needed)"
    fi
    
    # Ejecutar con configuración ARM64
    act -j build-all \
        --container-architecture linux/arm64 \
        -P ubuntu-latest=catthehacker/ubuntu:act-latest \
        --artifact-server-path ${RESULTS_DIR} \
        --env SCONS_CACHE_DIR="$SCONS_CACHE_DIR" \
        --env CARGO_HOME="$CARGO_HOME" \
        "$@"
else
    echo "🚀 Configuring for x86_64 host"
    act -j build-all \
        --artifact-server-path ${RESULTS_DIR} \
        --env SCONS_CACHE_DIR="$SCONS_CACHE_DIR" \
        --env CARGO_HOME="$CARGO_HOME" \
        "$@"
fi

echo ""
echo "✅ Build completed!"
echo "📁 Results: ls ${RESULTS_DIR}/"
ls ${RESULTS_DIR}/ 2>/dev/null || echo "  (no files yet)"

echo ""
echo "📊 Cache sizes:"
echo "  - SCons: $(du -sh .scons_cache 2>/dev/null | cut -f1 || echo '0B')"
echo "  - Cargo: $(du -sh ${CACHE_DIR}/cargo 2>/dev/null | cut -f1 || echo '0B')"