cmake_minimum_required(VERSION 3.31.6)
project(godot-dojo LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.hpp include/*.h)

add_library(godot-dojo SHARED ${SOURCES} ${HEADERS})

# Godot-cpp
include(FetchContent)
FetchContent_Declare(
    godot-cpp
    GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
    GIT_TAG godot-4.2.2-stable
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(godot-cpp)
list(APPEND CMAKE_MODULE_PATH "${godot-cpp_SOURCE_DIR}/cmake")

# Dojo.c
FetchContent_Declare(
    dojo_c
    GIT_REPOSITORY https://github.com/dojoengine/dojo.c.git
    GIT_TAG v1.6.0-alpha.0
)
FetchContent_MakeAvailable(dojo_c)

find_package(Rust REQUIRED)
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
    ${godot-cpp_SOURCE_DIR}/gdextension
    ${godot-cpp_SOURCE_DIR}/include/godot_cpp
    ${godot-cpp_SOURCE_DIR}/src
    ${godot-cpp_BINARY_DIR}/gen/src
    ${godot-cpp_BINARY_DIR}/gen/include/godot_cpp
    ${dojo_c_SOURCE_DIR}
)

target_link_libraries(godot-dojo PRIVATE godot::cpp dojo_c)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -ldbus-1")

# Arquitectura y nombres de salida
if (NOT DEFINED BITS)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(BITS 64)
    else()
        set(BITS 32)
    endif()
endif()

set_target_properties(godot-dojo PROPERTIES
    PREFIX ""
    OUTPUT_NAME godot-dojo.$<LOWER_CASE:$<PLATFORM_ID>>.template_$<LOWER_CASE:$<CONFIG>>.x86_${BITS}
    ARCHIVE_OUTPUT_NAME godot-dojo.$<LOWER_CASE:$<PLATFORM_ID>>.template_$<LOWER_CASE:$<CONFIG>>.x86_${BITS}
)

# Limpieza y copias post build
file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/demo/bin/)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/demo/bin)

add_custom_command(TARGET godot-dojo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:godot-dojo> ${CMAKE_SOURCE_DIR}/demo/bin/
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/godot-dojo.gdextension ${CMAKE_SOURCE_DIR}/demo/bin/
)