//
// Created by hazel on 2/06/25.
//
#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;

// Concatena los elementos en un String de Godot
template<typename... Args>
String concat_args(Args&&... args) {
    String result;
    ((result += String(args)), ...);
    return result;
}

// Utility Macros
#define FIRST_ARG(first, ...) concat_args(first)
#define REST_ARGS(first, ...) concat_args(__VA_ARGS__)

// Macros que siempre funcionan (errores y warnings)
#define LOG_ERROR(...) \
UtilityFunctions::push_error(concat_args(__VA_ARGS__))

#define LOG_WARNING(...) \
UtilityFunctions::push_warning(concat_args(__VA_ARGS__))

// Macros que solo funcionan cuando DEBUG_ENABLED está activo
// Despues se puede ver si se mueben de lugar o no
#ifdef DEBUG_ENABLED
    #define LOG_INFO(...) \
        TYPED_LOG_COLOR(cyan, "INFO", __VA_ARGS__)

    #define LOG_SUCCESS(...) \
        TYPED_LOG_COLOR(green, "SUCCESS", __VA_ARGS__)

    #define LOG_DEBUG(...) \
        TYPED_LOG_COLOR(wheat, "DEBUG", __VA_ARGS__)

    #define TYPED_LOG_COLOR(color, ...) \
    UtilityFunctions::print_rich("[color=" #color "][b][" + FIRST_ARG(__VA_ARGS__) + "][/b][/color] ", REST_ARGS(__VA_ARGS__))

    #define LOG_COLOR(color, ...) \
        UtilityFunctions::print_rich("[color=" #color "]" + concat_args(__VA_ARGS__) + "[/color]")

#else
    // En release, estos macros no hacen en nada
    #define LOG_INFO(...) ((void)0)
    #define LOG_SUCCESS(...) ((void)0)
    #define LOG_DEBUG(...) ((void)0)
    #define LOG_COLOR(color, ...) ((void)0)
#endif

#endif // DEBUG_MACROS_H