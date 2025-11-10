//
// Created by hazel on 2/06/25.
//
#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H
#include "godot_cpp/classes/project_settings.hpp"
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
        ((result += make_safe_variant(args).stringify() + " "), ...);
        return result;
    }
}

class Logger
{
private:
	static bool is_enabled()
	{
		static bool initialized = false;
		static bool enabled = true;

		if (!initialized)
		{
			if (ProjectSettings::get_singleton()->has_setting("dojo/config/enable_extra_logs"))
			{
				enabled = ProjectSettings::get_singleton()->get_setting("dojo/config/enable_extra_logs");
			}
			initialized = true;
		}
		return enabled;
	}

public:
    template <typename... Args>
    static void error(Args... args)
    {
        if (!is_enabled()) { return; }
        String message = logger_internal::concat_all(args...);
        UtilityFunctions::push_error(message);
    }
    // Doesn't send error, maybe change later and add a warning version
    template <typename... Args>
    static Dictionary error_dict(Args... args)
    {
        if (!is_enabled()) { return {}; }
        String message = logger_internal::concat_all(args...);
        Dictionary result = {};
        result["error"] = message;
        error(message);
        return result;
    }

    template <typename... Args>
    static void warning(Args... args)
    {
        if (!is_enabled()) { return; }
        String message = logger_internal::concat_all(args...);
        UtilityFunctions::push_warning(message);
    }

    template <typename... Args>
    static void log_color(const String& color, Args... args)
    {
#ifdef DEBUG_ENABLED
        if (!is_enabled()) { return; }
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=" + color + "]" + message + "[/color]";
        UtilityFunctions::print_rich(formatted);
#endif
    }

    template <typename... Args>
    static void typed_log_color(const String& color, const String& type, Args... args)
    {
#ifdef DEBUG_ENABLED
        if (!is_enabled()) { return; }
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=" + color + "][b][" + type + "][/b][/color] " + message;
#ifdef WEB_ENABLED
        formatted = formatted.insert(0, "[color=green][b][WEB][/b][/color]");
#endif
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
    static void custom_color(const String& color, const String& type, Args... args)
    {
        typed_log_color(color, type, args...);
    }

    template <typename... Args>
    static void success_extra(const String& type, Args... args)
    {
#ifdef DEBUG_ENABLED
        if (!is_enabled()) { return; }
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=green][b][" + type + "][/b][/color] " + message;
#ifdef WEB_ENABLED
        formatted = formatted.insert(0, "[color=green][b][WEB][/b][/color]");
#endif
        UtilityFunctions::print_rich(formatted);
#endif
    }

    template <typename... Args>
    static void debug_extra(const String& type, Args... args)
    {
#ifdef DEBUG_ENABLED
        if (!is_enabled()) { return; }
        String message = logger_internal::concat_all(args...);
        String formatted = "[color=cyan][b][" + type + "][/b][/color] " + message;
#ifdef WEB_ENABLED
        formatted = formatted.insert(0, "[color=green][b][WEB][/b][/color]");
#endif
        UtilityFunctions::print_rich(formatted);
#endif
    }

    static void empty_line()
    {
        if (!is_enabled()) { return; }
        UtilityFunctions::print_rich("\n");
    }
};

#endif // DEBUG_MACROS_H
