#include "temp/dojo_helper.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "tools/logger.h"

DojoHelpers* DojoHelpers::singleton = nullptr;

void DojoHelpers::_bind_methods()
{
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("convert_to_string_array", "var"),
                                &DojoHelpers::convert_to_string_array);
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("get_katana_url"), &DojoHelpers::get_katana_url);
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("get_setting", "setting"), &DojoHelpers::get_setting);
    // Generic. Default to precision 24
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("float_to_fixed", "value", "precision"),
                                &DojoHelpers::float_to_fixed, DEFVAL(24));
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("fixed_to_float", "value", "precision"),
                                &DojoHelpers::fixed_to_float, DEFVAL(24));
    // 64
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("float_to_fixed_64", "value"), &DojoHelpers::float_to_fixed_64);
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("fixed_to_float_64", "value"), &DojoHelpers::fixed_to_float_64);
    // 128
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("float_to_fixed_128", "value"),
                                &DojoHelpers::float_to_fixed_128);
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("fixed_to_float_128", "value"),
                                &DojoHelpers::fixed_to_float_128);
    // 256
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("float_to_fixed_256", "value"),
                                &DojoHelpers::float_to_fixed_256);
    ClassDB::bind_static_method("DojoHelpers", D_METHOD("fixed_to_float_256", "value"),
                                &DojoHelpers::fixed_to_float_256);
}

DojoHelpers::DojoHelpers()
{
    singleton = this;
}

DojoHelpers::~DojoHelpers()
{
    singleton = nullptr;
}

String DojoHelpers::get_katana_url()
{
    return get_setting("dojo/config/katana/rpc_url");
}

Variant DojoHelpers::get_setting(const String& setting)
{
    ProjectSettings* settings = ProjectSettings::get_singleton();
    if (settings->has_setting(setting))
    {
        return settings->get_setting(setting);
    }
    Logger::error("Setting not found", setting);
    return Variant();
}

int64_t DojoHelpers::float_to_fixed(const double& value, const int& precision)
{
    return static_cast<int64_t>(value * (1LL << precision));
}

double DojoHelpers::fixed_to_float(const int& value, const int& precision)
{
    return value / static_cast<double>(1LL << precision);
}

int64_t DojoHelpers::float_to_fixed_64(const float& value)
{
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/64"));
}

double DojoHelpers::fixed_to_float_64(const int& value)
{
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/64"));
}

int64_t DojoHelpers::float_to_fixed_128(const float& value)
{
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/128"));
}

double DojoHelpers::fixed_to_float_128(const int& value)
{
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/128"));
}

int64_t DojoHelpers::float_to_fixed_256(const float& value)
{
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/256"));
}

double DojoHelpers::fixed_to_float_256(const int& value)
{
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/256"));
}

TypedArray<String> DojoHelpers::convert_to_string_array(const Variant& var)
{
    TypedArray<String> arr;
    switch (var.get_type())
    {
    case Variant::DICTIONARY:
        {
            Dictionary dict = var;
            Array keys = dict.keys();
            for (int i = 0; i < keys.size(); ++i)
            {
                Variant val = dict[keys[i]];
                // arr.push_back(keys[i]);
                arr.push_back(val.operator String());
            }
            break;
        }
    case Variant::ARRAY:
        {
            Array v_arr = var;
            for (int i = 0; i < v_arr.size(); ++i)
            {
                arr.push_back(v_arr[i].operator String());
            }
            break;
        }
    case Variant::VECTOR2:
        {
            Vector2 v = var;
            arr.push_back(String::num(v.x));
            arr.push_back(String::num(v.y));
            break;
        }
    case Variant::INT:
    case Variant::FLOAT:
    case Variant::STRING:
        {
            arr.push_back(var.operator String());
            break;
        }
    default:
        break;
    }
    return arr;
}
