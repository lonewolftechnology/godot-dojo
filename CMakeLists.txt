cmake_minimum_required(VERSION 3.31.6)
project(godot-dojo LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

option(BUILD_WEB "Enable Web (WASM/WEB_ENABLED) build" OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.hpp include/*.h)

add_library(godot-dojo SHARED ${SOURCES} ${HEADERS})

if(BUILD_WEB)
    message(STATUS "Web build enabled, defining WEB_ENABLED preprocessor macro.")
    target_compile_definitions(godot-dojo PRIVATE WEB_ENABLED)
endif()

# Godot-cpp (use submodule headers directly to avoid building under _deps)
# Define a header-only interface target that points to the submodule directories.
add_library(godot-cpp INTERFACE)
add_library(godot::cpp ALIAS godot-cpp)

# Public include directories for godot-cpp headers and generated headers in the submodule.
target_include_directories(godot-cpp INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/godot-cpp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/godot-cpp/gen/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/godot-cpp/gdextension
)

# Dojo.c (use submodule directly; avoid FetchContent/_deps)
set(dojo_c_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/dojo.c)
# Place Rust build artifacts under the current CMake build dir to let CMake/Ninja manage rules.
set(dojo_c_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/dojo_c)

find_package(Rust REQUIRED)
# Ensure the binary directory exists at configure time to satisfy interface include dirs
file(MAKE_DIRECTORY ${dojo_c_BINARY_DIR})
add_rust_library(TARGET dojo_c SOURCE_DIRECTORY ${dojo_c_SOURCE_DIR} BINARY_DIRECTORY ${dojo_c_BINARY_DIR})

add_custom_target(patch_dojo_header
        COMMAND ${CMAKE_COMMAND} -E echo "=== Parchando dojo.hpp ==="
        COMMAND ${CMAKE_COMMAND} -DDOJO_HEADER_FILE=${dojo_c_SOURCE_DIR}/dojo.hpp -P "${CMAKE_SOURCE_DIR}/cmake/PatchDojoHpp.cmake"
        COMMENT "Parcheando dojo.hpp"
        VERBATIM
)

add_dependencies(patch_dojo_header dojo_c)
add_dependencies(godot-dojo patch_dojo_header)

configure_file(plugin_template.gdextension.in godot-dojo.gdextension)

target_include_directories(godot-dojo PRIVATE include src)
target_include_directories(godot-dojo PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/external/boost/include
)

target_link_libraries(godot-dojo PRIVATE godot::cpp dojo_c)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -ldbus-1")

# Arquitectura y nombres de salida
if (NOT DEFINED BITS)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(BITS 64)
    else ()
        set(BITS 32)
    endif ()
endif ()

set_target_properties(godot-dojo PROPERTIES
        PREFIX ""
        OUTPUT_NAME godot-dojo.$<LOWER_CASE:$<PLATFORM_ID>>.template_$<LOWER_CASE:$<CONFIG>>.x86_${BITS}
        ARCHIVE_OUTPUT_NAME godot-dojo.$<LOWER_CASE:$<PLATFORM_ID>>.template_$<LOWER_CASE:$<CONFIG>>.x86_${BITS}
)

# Limpieza y copias post build
file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/demo/addons/godot-dojo/)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/demo/addons/godot-dojo)

add_custom_command(TARGET godot-dojo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:godot-dojo> ${CMAKE_SOURCE_DIR}/demo/addons/godot-dojo/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/godot-dojo.gdextension ${CMAKE_SOURCE_DIR}/demo/addons/godot-dojo/
)