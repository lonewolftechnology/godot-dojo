//
// Created by hazel on 2/06/25.
//
#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H
#include "godot_cpp/variant/utility_functions.hpp"
using namespace godot;

// Concatena los elementos en un String de Godot
template <typename... Args>
String concat_args(Args&&... args)
{
    String result;
    ((result += Variant(args).stringify()), ...);
    return result;
}

// Utility Macros

// Macros que siempre funcionan (errores y warnings)
#define LOG_ERROR(...) \
UtilityFunctions::push_error(concat_args(__VA_ARGS__))

#define LOG_WARNING(...) \
UtilityFunctions::push_warning(concat_args(__VA_ARGS__))

// Macros que solo funcionan cuando DEBUG_ENABLED está activo
// Despues se puede ver si se mueven de lugar o no
#ifdef DEBUG_ENABLED
#define LOG_INFO(...) \
        TYPED_LOG_COLOR(cyan, "INFO", __VA_ARGS__)

#define LOG_SUCCESS(...) \
        TYPED_LOG_COLOR(green, "SUCCESS", __VA_ARGS__)

#define LOG_DEBUG(...) \
        TYPED_LOG_COLOR(wheat, "DEBUG", __VA_ARGS__)

#define LOG_CUSTOM(type, ...) \
        TYPED_LOG_COLOR(magenta, type, __VA_ARGS__)

#define LOG_SUCCESS_EXTRA(type, ...) \
        LOG_SUCCESS("[color=green][b][" type "][/b][/color]", __VA_ARGS__)

#define LOG_DEBUG_EXTRA(type, ...) \
        LOG_DEBUG("[color=cyan][b][" type "][/b][/color] ", __VA_ARGS__)

#define TYPED_LOG_COLOR(color, type, ...) \
    UtilityFunctions::print_rich("[color=" #color "][b][" type "][/b][/color] ", concat_args(__VA_ARGS__))

#define LOG_COLOR(color, ...) \
        UtilityFunctions::print_rich("[color=" #color "]" + concat_args(__VA_ARGS__) + "[/color]")

#else
    // En release, estos macros no hacen en nada
    #define LOG_INFO(...) ((void)0)
    #define LOG_SUCCESS(...) ((void)0)
    #define LOG_DEBUG(...) ((void)0)
    #define LOG_COLOR(color, ...) ((void)0)
    #define LOG_CUSTOM(type, ...) ((void)0)
    #define LOG_SUCCESS_EXTRA(type, ...) ((void)0)
    #define LOG_DEBUG_EXTRA(type, ...) ((void)0)
    #define TYPED_LOG_COLOR(color, type, ...) ((void)0)
#endif

#endif // DEBUG_MACROS_H