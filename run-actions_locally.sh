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

echo "🚀 Running GitHub Actions locally with act..."
echo "📊 Cache setup:"
echo "  - SCons: $SCONS_CACHE_DIR"
echo "  - Cargo: $CARGO_HOME"
echo "  - Results: $RESULTS_DIR"
echo ""

# Verificar si act soporta bind
ACT_VERSION=$(act --version 2>/dev/null || echo "unknown")
echo "🔍 Act version: $ACT_VERSION"

# Ejecutar act con configuración básica
if act --help 2>/dev/null | grep -q "bind"; then
    echo "✅ Using bind support"
    act -j build-linux \
        --bind \
        --artifact-server-path ${RESULTS_DIR} \
        "$@"
else
    echo "⚠️  Basic mode (no bind support)"
    act -j build-linux \
        --artifact-server-path ${RESULTS_DIR} \
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