#include "temp/dojo_helper.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "tools/logger.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

DojoHelpers* DojoHelpers::singleton = nullptr;
using boost::multiprecision::cpp_int;
typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100> > cpp_dec_float_100;


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

// these use boost::multiprecision, that can be included separately but not sure if it's header only
double DojoHelpers::variant_to_double_fp(const Variant& value, const int precision) {

    cpp_int int_val;
    // TODO convert variant (from Cairo, could be a simple int or array of ints for u128 and up) to cpp_int
    switch (value.get_type()) {

        case Variant::BOOL:
        case Variant::INT:
        case Variant::FLOAT:
            int_val = (int64_t)value;
            break;
        case Variant::STRING:
            int_val = cpp_int(String(value).utf8().get_data());
            break;
        case Variant::ARRAY: {
            // Other variant array types go here?
            // take the bytes from the array here and construct the big integer
            PackedByteArray bytes = value;
            for (int i=0; i<bytes.size(); i++) {
                int_val += bytes[i];
                int_val <<= 8;
            };

        } break;
    };

    cpp_int divisor = 1;
    divisor <<= precision;
    cpp_dec_float_100 result = cpp_dec_float_100(int_val) / cpp_dec_float_100(divisor);

    return (double)result;
}

Variant DojoHelpers::double_to_variant(const double value, const int precision) {

    cpp_int shift = 1;
    shift <<= precision;
    cpp_dec_float_100 val_100 = (double)value;
    val_100 *= precision;

    cpp_int val_int(val_100);

    // convert val_100 to Variant here for use with field element etc
    if (msb(val_int) < 64) {
        return Variant((int64_t)val_int);
    };

    PackedByteArray arr;
    int bytes = (msb(val_int) / 8) + 1;
    arr.resize(bytes);
    for (int i=0; i<bytes; i++) {

        arr[bytes - i] = (uint8_t)(val_int & 0xff);
        val_int >>= 8;
    };

    return Variant(arr);
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

dojo_bindings::COptionc_char DojoHelpers::create_option_from_string(const String& option)
{
    DOJO::COptionc_char coption = {};
    if (option.is_empty())
    {
        Logger::debug_extra("Option", "Empty option, setting to Nonec_char");
        coption.tag = DOJO::COptionc_char_Tag::Nonec_char;
    }
    else
    {
        Logger::debug_extra("Option", "Setting option to Somec_char with", option);
        coption.tag = DOJO::COptionc_char_Tag::Somec_char;
        coption.some = option.utf8().get_data();
    }
    return coption;
}
