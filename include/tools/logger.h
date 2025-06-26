//
// Created by hazel on 2/06/25.
//
#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H
#include "godot_cpp/variant/utility_functions.hpp"
using namespace godot;

namespace logger_internal
{
    // Helper para convertir tipos problemáticos a tipos seguros
    template <typename T>
    auto make_safe_variant(const T& value) -> Variant
    {
        if constexpr (std::is_same_v<T, const char*>)
        {
            return Variant(String(value));
        }
        else if constexpr (std::is_same_v<T, char*>)
        {
            return Variant(String(value));
        }
        else if constexpr (std::is_arithmetic_v<T> && std::is_unsigned_v<T> && sizeof(T) >= sizeof(uint32_t))
        {
            // Para unsigned long, size_t, uintptr_t, etc.
            return Variant(static_cast<int64_t>(value));
        }
        else if constexpr (std::is_arithmetic_v<T>)
        {
            return Variant(value);
        }
        else
        {
            return Variant(value);
        }
    }

    template <typename... Args>
    inline String concat_all(Args... args)
    {
        String result;
        ((result += make_safe_variant(args).stringify()), ...);
        return result;
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
    static Dictionary error_dict(Args... args)
    {
        String message = logger_internal::concat_all(args...);
        Dictionary result = {};
        result["error"] = message;
        return result;
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

#endif // DEBUG_MACROS_H
