//
// Created by hazel on 2/06/25.
//
#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;

template<typename... Args>
String concat_args(Args&&... args) {
    String result;
    ((result += String(args)), ...);
    return result;
}

// Macros que siempre funcionan (errores y warnings)
#define LOG_ERROR(...) \
UtilityFunctions::push_error(concat_args(__VA_ARGS__))

#define LOG_WARNING(...) \
UtilityFunctions::push_warning(concat_args(__VA_ARGS__))

// Macros que solo funcionan cuando DEBUG_ENABLED está definido
#ifdef DEBUG_ENABLED
    #define LOG_INFO(...) \
        LOG_COLOR(cyan, "[b][INFO][/b] ", __VA_ARGS__)

    #define LOG_SUCCESS(...) \
        LOG_COLOR(green, "[b][SUCCESS][/b] ", __VA_ARGS__)

    #define LOG_DEBUG(...) \
        LOG_COLOR(wheat, "[DEBUG] ", __VA_ARGS__)

    #define LOG_COLOR(color, ...) \
    UtilityFunctions::print_rich("[color=" #color "]" + concat_args(__VA_ARGS__) + "[/color]")
#else
    // En release, estos macros no hacen nada
    #define LOG_INFO(...) ((void)0)
    #define LOG_SUCCESS(...) ((void)0)
    #define LOG_DEBUG(...) ((void)0)
    #define LOG_COLOR(color, ...) ((void)0)
#endif

#endif // DEBUG_MACROS_H