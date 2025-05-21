cmake_minimum_required(VERSION 3.31.6)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# This option tells CMake to export the compile commands for the target.
# This can be useful for debugging and profiling purposes.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

########################################################################################################################

# Project setup
set(PROJECT_NAME "godot-dojo")
project(
        ${PROJECT_NAME}
        LANGUAGES CXX
        VERSION 0.1
)

# Entry point settings
set(ENTRY_POINT "dojoc_library_init")
set(GODOT_MIN_REQUIREMENT 4.2)
set(GDP_TEMPLATE_FILE "plugin_template.gdextension.in")

########################################################################################################################

# Gather source files (include only if necessary to avoid issues with globbing in dynamic builds)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS include/*.hpp include/*.h)

# Add shared library target
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

########################################################################################################################

# FetchContent to acquire Godot bindings
include(FetchContent)
FetchContent_Declare(
        godot-cpp
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG godot-4.2.2-stable # Ensure the Godot bindings align with your target Godot version
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(godot-cpp)
list(APPEND CMAKE_MODULE_PATH "${godot-cpp_SOURCE_DIR}/cmake")

########################################################################################################################

# FetchContent to acquire Dojo.C bindings
include(FetchContent)
FetchContent_Declare(
        dojoc
        GIT_REPOSITORY https://github.com/dojoengine/dojo.c.git
        GIT_TAG v1.5.0
)
FetchContent_MakeAvailable(dojoc)
########################################################################################################################
## Build Dojoc

set(Rust_TOOLCHAIN "stable-x86_64-unknown-linux-gnu")

find_package(Rust REQUIRED)

add_rust_library(TARGET dojo_c
        SOURCE_DIRECTORY "${dojoc_SOURCE_DIR}"
        BINARY_DIRECTORY "${dojoc_BINARY_DIR}"
)




########################################################################################################################

# Configure the template .gdextension file
configure_file(${GDP_TEMPLATE_FILE} ${PROJECT_NAME}.gdextension)
set(ADDITIONAL_SRCS ${PROJECT_NAME}.gdextension)

########################################################################################################################

# Include private directories (make sure headers are correctly included in both private and public contexts)
set(
        PRIVATE_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

foreach (dir IN LISTS PRIVATE_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PRIVATE ${dir})
endforeach ()

# Ensure the Godot headers are added correctly
set(
        PUBLIC_INCLUDE_DIRS
        ${godot-cpp_SOURCE_DIR}/gdextension
        ${godot-cpp_SOURCE_DIR}/gen/include
        ${godot-cpp_BINARY_DIR}/src
        ${godot-cpp_BINARY_DIR}/gen/include/godot_cpp
        ${dojoc_SOURCE_DIR}

)

foreach (dir IN LISTS PUBLIC_INCLUDE_DIRS)
    target_include_directories(${PROJECT_NAME} PUBLIC ${dir})
endforeach ()

# Link the Godot C++ library to your project
target_link_libraries(${PROJECT_NAME} PRIVATE godot::cpp)
#target_link_libraries(${PROJECT_NAME} PRIVATE godot::cpp PRIVATE dojo_c)

## Link Dojo library
target_link_libraries(${PROJECT_NAME} PRIVATE dojo_c)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -ldbus-1")

########################################################################################################################

# Set up architecture-specific output filenames
if (NOT DEFINED BITS)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(BITS 64)
    else ()
        set(BITS 32)
    endif ()
endif ()

# Remove the "lib" prefix from the output name
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set(CMAKE_IMPORT_LIBRARY_PREFIX "")

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        ARCHIVE_OUTPUT_NAME ${PROJECT_NAME}.$<LOWER_CASE:$<PLATFORM_ID>>.template_$<LOWER_CASE:$<CONFIG>>.x86_${BITS}
)

# Set the output name and platform-specific customizations
set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}.$<LOWER_CASE:$<PLATFORM_ID>>.template_$<LOWER_CASE:$<CONFIG>>.x86_${BITS}
)


########################################################################################################################

# Remove old build files to prevent conflicts
file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/demo/bin/)
# Ensure the bin folder exists
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/demo/bin)

# Copy generated .dll to bin folder
add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/demo/bin/
)

# Copy .gdextension file to bin folder
add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.gdextension ${CMAKE_SOURCE_DIR}/demo/bin/
)


########################################################################################################################

