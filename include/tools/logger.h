//
// Created by hazel on 2/06/25.
//
#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H
#include "godot_cpp/variant/utility_functions.hpp"
using namespace godot;

namespace logger_internal
{
    template <typename T>
    inline String to_string(const T& value)
    {
        return Variant(value).stringify();
    }

    template <>
    inline String to_string<String>(const String& value)
    {
        return value;
    }

    template <typename... Args>
    inline String concat_all(Args... args)
    {
        return (to_string(args) + ...);
    }
}

class Logger
{
public:
    template <typename... Args>
    static void error(Args... args)
    {
        String message = logger_internal::concat_all(args...);
        UtilityFunctions::push_error(message);
    }

    template <typename... Args>
    static void warning(Args... args)
    {
        String message = logger_internal::concat_all(args...);
        UtilityFunctions::push_warning(message);
    }

    template <typename... Args>
    static void log_color(const String& color, Args... args)
    {
#ifdef DEBUG_ENABLED
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=" + color + "]" + message + "[/color]";
        UtilityFunctions::print_rich(formatted);
#endif
    }

    template <typename... Args>
    static void typed_log_color(const String& color, const String& type, Args... args)
    {
#ifdef DEBUG_ENABLED
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=" + color + "][b][" + type + "][/b][/color] " + message;
        UtilityFunctions::print_rich(formatted);
#endif
    }

    template <typename... Args>
    static void info(Args... args)
    {
        typed_log_color("cyan", "INFO", args...);
    }

    template <typename... Args>
    static void success(Args... args)
    {
        typed_log_color("green", "SUCCESS", args...);
    }

    template <typename... Args>
    static void debug(Args... args)
    {
        typed_log_color("wheat", "DEBUG", args...);
    }

    template <typename... Args>
    static void custom(const String& type, Args... args)
    {
        typed_log_color("magenta", type, args...);
    }

    template <typename... Args>
    static void success_extra(const String& type, Args... args)
    {
#ifdef DEBUG_ENABLED
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=green][b][" + type + "][/b][/color] " + message;
        UtilityFunctions::print_rich(formatted);
#endif
    }

    template <typename... Args>
    static void debug_extra(const String& type, Args... args)
    {
#ifdef DEBUG_ENABLED
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=cyan][b][" + type + "][/b][/color] " + message;
        UtilityFunctions::print_rich(formatted);
#endif
    }


    static void empty_line()
    {
        UtilityFunctions::print_rich("\n");
    }
};

// Alias para compatibilidad (los voy a borrar despues)
#define LOG_ERROR(...) Logger::error(__VA_ARGS__)
#define LOG_WARNING(...) Logger::warning(__VA_ARGS__)

#ifdef DEBUG_ENABLED
#define LOG_INFO(...) Logger::info(__VA_ARGS__)
#define LOG_SUCCESS(...) Logger::success(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::debug(__VA_ARGS__)
#define LOG_CUSTOM(type, ...) Logger::custom(type, __VA_ARGS__)
#define LOG_SUCCESS_EXTRA(type, ...) Logger::success_extra(type, __VA_ARGS__)
#define LOG_DEBUG_EXTRA(type, ...) Logger::debug_extra(type, __VA_ARGS__)
#define LOG_COLOR(color, ...) Logger::log_color(color, __VA_ARGS__)
#else
#define LOG_INFO(...) ((void)0)
#define LOG_SUCCESS(...) ((void)0)
#define LOG_DEBUG(...) ((void)0)
#define LOG_CUSTOM(type, ...) ((void)0)
#define LOG_SUCCESS_EXTRA(type, ...) ((void)0)
#define LOG_DEBUG_EXTRA(type, ...) ((void)0)
#define LOG_COLOR(color, ...) ((void)0)
#endif

#endif // DEBUG_MACROS_H
