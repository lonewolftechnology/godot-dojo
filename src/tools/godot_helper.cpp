#include "tools/godot_helper.hpp"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "tools/logger.hpp"
#include <boost/multiprecision/cpp_dec_float.hpp>

GodotHelper *GodotHelper::singleton = nullptr;

using boost::multiprecision::cpp_int;
typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100> > cpp_dec_float_100;

// The STARK prime P, as a constant.
// P = 2^251 + 17 * 2^192 + 1
const cpp_int GodotHelper::STARK_PRIME = (cpp_int(1) << 251) + (cpp_int(17) * (cpp_int(1) << 192)) + 1;

cpp_int GodotHelper::to_starknet_negative_felt(cpp_int val) {
    if (val < 0) {
        val += STARK_PRIME;
    }
    return val;
}

GodotHelper::GodotHelper() {
    singleton = this;
}

GodotHelper::~GodotHelper() {
    singleton = nullptr;
}

String GodotHelper::get_katana_url() {
    return get_setting("dojo/config/katana_url");
}

Variant GodotHelper::get_setting(const String &setting, const Variant &default_value) {
    if (default_value.get_type() == Variant::NIL) {
        return ProjectSettings::get_singleton()->get_setting(setting);
    }
    return ProjectSettings::get_singleton()->get_setting(setting, default_value);
}

Variant GodotHelper::get_custom_setting(const String& category, const String &setting) {
    Variant result = get_setting(vformat("%s/config/%s", category, setting));
    if (result.get_type() == Variant::NIL) {
        Logger::debug_extra("GodotHelper", vformat("%s not found, trying dojo setting", setting));
        result = get_dojo_setting(setting);
    }
    return result;
}

Variant GodotHelper::get_dojo_setting(const String &setting, const Variant& default_value) {
    return get_setting("dojo/config/" + setting, default_value);
}

Variant GodotHelper::get_torii_setting(const String &setting, const Variant& default_value) {
    return get_setting("dojo/config/torii/" + setting, default_value);
}

Dictionary GodotHelper::get_policies(const String& custom) {
    Dictionary policies = {};

    if (custom.is_empty()) {
        policies = get_custom_setting(custom, "policies");
    } else {
        policies = get_dojo_setting("policies");
    }

    return policies;
}

int64_t GodotHelper::float_to_fixed(const double &value, const int &precision) {
    return static_cast<int64_t>(value * (1LL << precision));
}

double GodotHelper::fixed_to_float(const int &value, const int &precision) {
    return value / static_cast<double>(1LL << precision);
}

int64_t GodotHelper::float_to_fixed_64(const float &value) {
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/64"));
}

double GodotHelper::fixed_to_float_64(const int &value) {
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/64"));
}

int64_t GodotHelper::float_to_fixed_128(const float &value) {
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/128"));
}

double GodotHelper::fixed_to_float_128(const int &value) {
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/128"));
}

int64_t GodotHelper::float_to_fixed_256(const float &value) {
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/256"));
}

double GodotHelper::fixed_to_float_256(const int &value) {
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/256"));
}

double GodotHelper::variant_to_double_fp(const Variant &value, const int &precision) {
    cpp_int int_val;
    switch (value.get_type()) {
        case Variant::BOOL:
        case Variant::INT:
        case Variant::FLOAT:
            int_val = static_cast<int64_t>(value);
            break;
        case Variant::STRING:
            int_val = cpp_int(String(value).utf8().get_data());
            break;
        case Variant::ARRAY: {
            PackedByteArray bytes = value;
            for (int i = bytes.size() - 1; i >= 0; i--) {
                int_val <<= 8;
                int_val |= bytes[i];
            }
            break;
        }
        default:
            break;
    };

    cpp_int divisor = 1;
    divisor <<= precision;
    cpp_dec_float_100 result = cpp_dec_float_100(int_val) / cpp_dec_float_100(divisor);

    return static_cast<double>(result);
}

Variant GodotHelper::double_to_variant_fp(const double &value, const int &precision) {
    cpp_int shift = 1;
    shift <<= precision;
    cpp_dec_float_100 val_100 = value;
    val_100 *= cpp_dec_float_100(shift);

    cpp_int val_int(val_100);

    if (val_int >= std::numeric_limits<int64_t>::min() && val_int <= std::numeric_limits<int64_t>::max()) {
        return {static_cast<int64_t>(val_int)};
    }

    val_int = to_starknet_negative_felt(val_int);

    PackedByteArray arr;
    int bytes = (val_int == 0) ? 0 : (msb(val_int) / 8) + 1;
    arr.resize(bytes);
    for (int i = 0; i < bytes; i++) {
        arr[i] = static_cast<uint8_t>(val_int & 0xff);
        val_int >>= 8;
    }
    return {arr};
}

bool GodotHelper::get_log_level_enabled(const String& level)
{
    String setting_path = "dojo/config/debug/" + level;
    return ProjectSettings::get_singleton()->get_setting(setting_path);
}

void GodotHelper::set_log_level_enabled(const String& level, bool enabled)
{
    ProjectSettings* settings = ProjectSettings::get_singleton();
    String setting_path = "dojo/config/debug/" + level;
    settings->set_setting(setting_path, enabled);
    if (Engine::get_singleton()->is_editor_hint()) {
        settings->save();
    }
}

void GodotHelper::set_error_enabled(bool enabled)
{
    set_log_level_enabled("error", enabled);
}

void GodotHelper::set_warning_enabled(bool enabled)
{
    set_log_level_enabled("warning", enabled);
}

void GodotHelper::set_info_enabled(bool enabled)
{
    set_log_level_enabled("info", enabled);
}

void GodotHelper::set_debug_enabled(bool enabled)
{
    set_log_level_enabled("debug", enabled);
}

void GodotHelper::set_success_enabled(bool enabled)
{
    set_log_level_enabled("success", enabled);
}

void GodotHelper::_bind_methods() {
    ClassDB::bind_static_method("GodotHelper", D_METHOD("get_katana_url"), &GodotHelper::get_katana_url);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("get_setting", "setting", "default_value"),
                                &GodotHelper::get_setting, DEFVAL(Variant()));
    ClassDB::bind_static_method("GodotHelper", D_METHOD("get_dojo_setting", "setting", "default_value"),
                                &GodotHelper::get_dojo_setting, DEFVAL(Variant()));
    ClassDB::bind_static_method("GodotHelper", D_METHOD("get_torii_setting", "setting", "default_value"),
                        &GodotHelper::get_torii_setting, DEFVAL(Variant()));
    ClassDB::bind_static_method("GodotHelper", D_METHOD("get_policies", "p_custom"),
                                &GodotHelper::get_policies, DEFVAL(String()));
    ClassDB::bind_static_method("GodotHelper", D_METHOD("get_custom_setting", "category", "setting"),
                                &GodotHelper::get_custom_setting);

    // Generic. Default to precision 24
    ClassDB::bind_static_method("GodotHelper", D_METHOD("float_to_fixed", "value", "precision"),
                                &GodotHelper::float_to_fixed, DEFVAL(24));
    ClassDB::bind_static_method("GodotHelper", D_METHOD("fixed_to_float", "value", "precision"),
                                &GodotHelper::fixed_to_float, DEFVAL(24));
    // 64
    ClassDB::bind_static_method("GodotHelper", D_METHOD("float_to_fixed_64", "value"),
                                &GodotHelper::float_to_fixed_64);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("fixed_to_float_64", "value"),
                                &GodotHelper::fixed_to_float_64);
    // 128
    ClassDB::bind_static_method("GodotHelper", D_METHOD("float_to_fixed_128", "value"),
                                &GodotHelper::float_to_fixed_128);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("fixed_to_float_128", "value"),
                                &GodotHelper::fixed_to_float_128);
    // 256
    ClassDB::bind_static_method("GodotHelper", D_METHOD("float_to_fixed_256", "value"),
                                &GodotHelper::float_to_fixed_256);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("fixed_to_float_256", "value"),
                                &GodotHelper::fixed_to_float_256);

    ClassDB::bind_static_method("GodotHelper", D_METHOD("double_to_variant_fp", "value", "precision"),
                                &GodotHelper::double_to_variant_fp);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("variant_to_double_fp", "value", "precision"),
                                &GodotHelper::variant_to_double_fp);

    ClassDB::bind_static_method("GodotHelper", D_METHOD("get_log_level_enabled", "level"), &GodotHelper::get_log_level_enabled);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("set_log_level_enabled", "level", "enabled"), &GodotHelper::set_log_level_enabled);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("set_error_enabled", "enabled"), &GodotHelper::set_error_enabled);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("set_warning_enabled", "enabled"), &GodotHelper::set_warning_enabled);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("set_info_enabled", "enabled"), &GodotHelper::set_info_enabled);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("set_debug_enabled", "enabled"), &GodotHelper::set_debug_enabled);
    ClassDB::bind_static_method("GodotHelper", D_METHOD("set_success_enabled", "enabled"), &GodotHelper::set_success_enabled);
}
