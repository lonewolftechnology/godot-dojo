//
// Created by hazel on 2/06/25.
//
#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "tools/dojo_helper.h"
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
        ((result += make_safe_variant(args).stringify() + " "), ...);
        return result;
    }
}

class Logger
{
    static bool is_level_enabled(const String& level)
    {
        return DojoHelpers::get_log_level_enabled(level);
    }

private:
    template <typename... Args>
    static void _print_typed(const String& color, const String& type, Args... args)
    {
#ifdef DEBUG_ENABLED
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=" + color + "][b][" + type + "][/b][/color] " + message;
#ifdef WEB_ENABLED
        formatted = formatted.insert(0, "[color=green][b][WEB][/b][/color]");
#endif
        UtilityFunctions::print_rich(formatted);
#endif
    }

public:
    template <typename... Args>
    static void error(Args... args)
    {
        if (!is_level_enabled("error")) { return; }
        String message = logger_internal::concat_all(args...);
        ERR_PRINT_ED(message.utf8().get_data());
    }

    template <typename... Args>
    static Dictionary error_dict(Args... args)
    {
        if (!is_level_enabled("error")) { return {}; }
        String message = logger_internal::concat_all(args...);
        Dictionary result = {};
        result["error"] = message;
        error(message);
        return result;
    }

    template <typename... Args>
    static void warning(Args... args)
    {
        if (!is_level_enabled("warning")) { return; }
        String message = logger_internal::concat_all(args...);
        WARN_PRINT_ED(message.utf8().get_data());
    }

    template <typename... Args>
    static void info(Args... args)
    {
        if (!is_level_enabled("info")) { return; }
        _print_typed("cyan", "INFO", args...);
    }

    template <typename... Args>
    static void success(Args... args)
    {
        if (!is_level_enabled("success")) { return; }
        _print_typed("green", "SUCCESS", args...);
    }

    template <typename... Args>
    static void debug(Args... args)
    {
        if (!is_level_enabled("debug")) { return; }
        _print_typed("wheat", "DEBUG", args...);
    }

    template <typename... Args>
    static void custom(const String& type, Args... args)
    {
        if (!is_level_enabled("debug")) { return; }
        _print_typed("magenta", type, args...);
    }

    template <typename... Args>
    static void custom_color(const String& color, const String& type, Args... args)
    {
        if (!is_level_enabled("debug")) { return; }
        _print_typed(color, type, args...);
    }

    template <typename... Args>
    static void success_extra(const String& type, Args... args)
    {
        if (!is_level_enabled("debug")) { return; }
        _print_typed("green", type, args...);
    }

    template <typename... Args>
    static void debug_extra(const String& type, Args... args)
    {
        if (!is_level_enabled("debug")) { return; }
        _print_typed("cyan", type, args...);
    }

    static void empty_line()
    {
        UtilityFunctions::print_rich("\n");
    }
};

#endif // DEBUG_MACROS_H
