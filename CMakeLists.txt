cmake_minimum_required(VERSION 3.16)
project(godot-dojo LANGUAGES CXX C VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # Ensure C++17 is used
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

option(BUILD_WEB "Enable Web (WASM/WEB_ENABLED) build" OFF)
option(DEBUG_ENABLED "Enable Debug (DEBUG_ENABLED) code blocks" OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp bindings/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.hpp bindings/*.hpp src/*.hpp)

# Filter out doc_data.gen.cpp unless in Debug/Tools mode (matches SConstruct logic)
if(NOT (DEBUG_ENABLED OR CMAKE_BUILD_TYPE MATCHES "Debug" OR TOOLS_ENABLED))
    list(FILTER SOURCES EXCLUDE REGEX "src/gen/doc_data\\.gen\\.cpp")
endif()

add_library(godot-dojo SHARED ${SOURCES} ${HEADERS})

# Enable C++ exceptions for try/catch blocks (cross-compiler compatible)
if(MSVC)
    # For MSVC on Windows
    target_compile_options(godot-dojo PRIVATE /EHsc)
else()
    # For GCC/Clang on Linux, macOS, MinGW, etc.
    target_compile_options(godot-dojo PRIVATE -fexceptions)
endif()

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
# Godot-cpp (Build properly via subdirectory)
add_subdirectory(external/godot-cpp)

# Boost (header-only)
add_library(boost INTERFACE)
add_library(Boost::boost ALIAS boost)
target_include_directories(boost INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/boost/include)

find_program(PATCH_EXECUTABLE patch)
if(NOT PATCH_EXECUTABLE)
    message(WARNING "patch command not found. Cannot apply patches to dojo.c")
endif()
configure_file(plugin_template.gdextension.in godot-dojo.gdextension)

target_include_directories(godot-dojo PRIVATE include src
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings
)

# --- Rust Integration (Matching SConstruct) ---
find_program(CARGO_EXECUTABLE cargo REQUIRED)
set(RUST_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/godot-dojo-core")
set(RUST_LIB_NAME "godot_dojo_core")

# Determine build mode
if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(CARGO_CMD_ARGS "build" "--release")
    set(RUST_TARGET_SUBDIR "release")
else()
    set(CARGO_CMD_ARGS "build")
    set(RUST_TARGET_SUBDIR "debug")
endif()

# Determine Rust target path (Simplified detection, ideally pass RUST_TARGET_TRIPLE to CMake)
if(DEFINED RUST_TARGET_TRIPLE)
    list(APPEND CARGO_CMD_ARGS "--target" "${RUST_TARGET_TRIPLE}")
    set(RUST_TARGET_DIR "${RUST_TARGET_TRIPLE}/${RUST_TARGET_SUBDIR}")
else()
    set(RUST_TARGET_DIR "${RUST_TARGET_SUBDIR}")
endif()

if(MSVC)
    set(RUST_LIB_FILENAME "${RUST_LIB_NAME}.lib")
else()
    set(RUST_LIB_FILENAME "${CMAKE_STATIC_LIBRARY_PREFIX}${RUST_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

set(RUST_LIB_PATH "${RUST_PROJECT_DIR}/target/${RUST_TARGET_DIR}/${RUST_LIB_FILENAME}")

# Custom target to build Rust
add_custom_target(cargo_build
        COMMAND ${CARGO_EXECUTABLE} ${CARGO_CMD_ARGS}
        WORKING_DIRECTORY ${RUST_PROJECT_DIR}
        COMMENT "Building Rust project (godot-dojo-core)..."
        VERBATIM
)
add_dependencies(godot-dojo cargo_build)

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
target_link_libraries(godot-dojo PRIVATE godot::cpp Boost::boost ${RUST_LIB_PATH})

# Platform specific linking for Rust dependencies (matches SConstruct)
if(WIN32)
    target_link_libraries(godot-dojo PRIVATE ws2_32 advapi32 ntdll userenv bcrypt)
    if(MSVC)
        target_link_options(godot-dojo PRIVATE "/OPT:NOREF" "/NODEFAULTLIB:MSVCRT")
    endif()
elseif(APPLE)
    target_link_libraries(godot-dojo PRIVATE "-framework CoreFoundation")
endif()

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
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_SOURCE_DIR}/demo/addons/godot-dojo/assets
)