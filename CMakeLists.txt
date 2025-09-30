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

if(ANDROID_ENABLED)
    message(STATUS "Android build enabled, defining ANDROID_ENABLED preprocessor macro.")
    target_compile_definitions(godot-dojo PRIVATE ANDROID_ENABLED)
endif()

if(DEBUG_ENABLED)
    message(STATUS "Debug build enabled, defining DEBUG_ENABLED preprocessor macro.")
    target_compile_definitions(godot-dojo PRIVATE DEBUG_ENABLED)
endif()

if(TOOLS_ENABLED)
    message(STATUS "Tools build enabled, defining TOOLS_ENABLED preprocessor macro.")
    target_compile_definitions(godot-dojo PRIVATE TOOLS_ENABLED)
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

# Boost (header-only)
add_library(boost INTERFACE)
add_library(Boost::boost ALIAS boost)
target_include_directories(boost INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/boost/include)

# Dojo.c (use submodule directly; avoid FetchContent/_deps)
set(dojo_c_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/dojo.c)
# Place Rust build artifacts under the current CMake build dir to let CMake/Ninja manage rules.
set(dojo_c_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/dojo_c)

find_package(Rust REQUIRED)
# Ensure the binary directory exists at configure time to satisfy interface include dirs
file(MAKE_DIRECTORY ${dojo_c_BINARY_DIR})

# Apply patches to dojo.c submodule before it's built
find_program(PATCH_EXECUTABLE patch)
if(PATCH_EXECUTABLE)
    file(GLOB PATCH_FILES "${CMAKE_CURRENT_SOURCE_DIR}/patches/*.patch")
    if(PATCH_FILES)
        add_custom_target(apply_dojo_c_patches
            COMMAND ${CMAKE_COMMAND} -E echo "Resetting and patching dojo.c submodule..."
            COMMAND ${CMAKE_COMMAND} -E chdir ${dojo_c_SOURCE_DIR}
                git reset --hard
            COMMAND ${CMAKE_COMMAND} -E chdir ${dojo_c_SOURCE_DIR}
                git apply ${PATCH_FILES}
            COMMENT "Resetting dojo.c and applying patches..."
            # Este objetivo se ejecutará cada vez que se construya, asegurando que los parches estén aplicados.
            # El 'git reset --hard' asegura un estado limpio antes de aplicar los parches.
        )
    endif()
else()
    message(WARNING "patch command not found. Cannot apply patches to dojo.c")
endif()

add_rust_library(TARGET dojo_c SOURCE_DIRECTORY ${dojo_c_SOURCE_DIR} BINARY_DIRECTORY ${dojo_c_BINARY_DIR} NO_DEFAULT_FEATURES)
if(TARGET apply_dojo_c_patches)
    add_dependencies(dojo_c apply_dojo_c_patches)
endif()

configure_file(plugin_template.gdextension.in godot-dojo.gdextension)

target_include_directories(godot-dojo PRIVATE include src)

# Link core libraries
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Find and link libdbus-1 properly via pkg-config on Linux
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(DBUS IMPORTED_TARGET dbus-1)
        if(DBUS_FOUND)
            message(STATUS "Found dbus-1 via pkg-config: ${DBUS_VERSION}")
            target_link_libraries(godot-dojo PRIVATE PkgConfig::DBUS)
        endif()
    endif()
endif()

# Always link to our C++/Rust parts
target_link_libraries(godot-dojo PRIVATE godot::cpp dojo_c Boost::boost)

# Avoid forcing -ldbus-1 or static stdlib via global CXX flags; manage via target_link_libraries instead.
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -ldbus-1")

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