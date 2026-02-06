#pragma once

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include <boost/multiprecision/cpp_int.hpp>

using namespace godot;
using boost::multiprecision::cpp_int;

class GodotDojoHelper : public Object {
    GDCLASS(GodotDojoHelper, Object)

public:
    GodotDojoHelper();
    ~GodotDojoHelper();

    static const cpp_int STARK_PRIME;
    static cpp_int to_starknet_negative_felt(cpp_int val);

    static GodotDojoHelper *get_singleton() { return singleton; };

    static String decode_hex(const String &hex);
    static String decode_hex(const std::string& hex);
    static String encode_hex(const String &input);

    static String get_katana_url();
    static Variant get_setting(const String &setting, const Variant &default_value = Variant());
    static Variant get_custom_setting(const String &category, const String &setting);
    static Variant get_dojo_setting(const String &setting, const Variant &default_value = Variant());
    static Variant get_torii_setting(const String &setting, const Variant &default_value = Variant());
    static Dictionary get_policies(const String &custom  = String());

    // Fixed point conversions
    static int64_t float_to_fixed(const double &value, const int &precision = 24);
    static double fixed_to_float(const int &value, const int &precision = 24);
    static int64_t float_to_fixed_64(const float &value);
    static double fixed_to_float_64(const int &value);
    static int64_t float_to_fixed_128(const float &value);
    static double fixed_to_float_128(const int &value);
    static int64_t float_to_fixed_256(const float &value);
    static double fixed_to_float_256(const int &value);

    static Variant double_to_variant_fp(const double &value, const int &precision);
    static double variant_to_double_fp(const Variant &value, const int &precision);
    static double bytes_to_double(const PackedByteArray& bytes, bool is_signed = false, int precision = 0);

    // Log levels
    static bool get_log_level_enabled(const String& level);
    static void set_log_level_enabled(const String& level, bool enabled);
    static void set_error_enabled(bool enabled);
    static void set_warning_enabled(bool enabled);
    static void set_info_enabled(bool enabled);
    static void set_debug_enabled(bool enabled);
    static void set_success_enabled(bool enabled);

protected:
    static GodotDojoHelper *singleton;

    static void _bind_methods();
};
