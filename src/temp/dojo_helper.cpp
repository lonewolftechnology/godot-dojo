#include "temp/dojo_helper.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "tools/logger.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

DojoHelpers* DojoHelpers::singleton = nullptr;
using boost::multiprecision::cpp_int;
typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100> > cpp_dec_float_100;

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
    return {};
}

// these use boost::multiprecision, that can be included separately but not sure if it's header only
double DojoHelpers::variant_to_double_fp(const Variant& value, const int precision) {

    cpp_int int_val;
    // TODO convert variant (from Cairo, could be a simple int or array of ints for u128 and up) to cpp_int
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

    return static_cast<double>(result);
}

Variant DojoHelpers::double_to_variant(const double value, const int precision) {

    cpp_int shift = 1;
    shift <<= precision;
    cpp_dec_float_100 val_100 = (double)value;
    val_100 *= precision;

    cpp_int val_int(val_100);

    // convert val_100 to Variant here for use with field element etc
    if (msb(val_int) < 64) {
        return {static_cast<int64_t>(val_int)};
    };

    PackedByteArray arr;
    int bytes = (msb(val_int) / 8) + 1;
    arr.resize(bytes);
    for (int i=0; i<bytes; i++) {

        arr[bytes - i] = static_cast<uint8_t>(val_int & 0xff);
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
                const Variant& val = dict[keys[i]];
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
            const Vector2 v = var;
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

DOJO::COptionc_char DojoHelpers::create_option_from_string(const String& option)
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

String DojoHelpers::u256_to_string_boost(const DOJO::U256& u256)
{
    std::vector<uint8_t> bytes(u256.data, u256.data + 32);

    std::reverse(bytes.begin(), bytes.end());

    cpp_int big_num;

    import_bits(big_num, bytes.begin(), bytes.end());

    return {big_num.str().c_str()};
}

DOJO::U256 DojoHelpers::string_to_fixed_point_u256(const String& integer_str, int precision)
{
    cpp_int integer_value(integer_str.utf8().get_data());

    cpp_int multiplier = 1;
    multiplier <<= precision;

    cpp_int fixed_point_value = integer_value * multiplier;

    cpp_int max_u256 = (cpp_int(1) << 256) - 1;
    if (fixed_point_value > max_u256) {
        Logger::error("Error: El valor de punto fijo excede los 256 bits.");
        return {};
    }

    std::vector<uint8_t> bytes;
    boost::multiprecision::export_bits(fixed_point_value, std::back_inserter(bytes), 8);

    DOJO::U256 result = {};

    size_t num_bytes = bytes.size();
    if (num_bytes > 32) {
         Logger::error("Error: export_bits generó más de 32 bytes.");
         return {};
    }

    uint8_t temp_buffer[32] = {0};
    memcpy(temp_buffer + (32 - num_bytes), bytes.data(), num_bytes);

    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));

    memcpy(result.data, temp_buffer, 32);

    return result;
}

String DojoHelpers::u256_fixed_point_to_string(const DOJO::U256& u256, int precision)
{
    std::vector<uint8_t> bytes(u256.data, u256.data + 32);
    std::reverse(bytes.begin(), bytes.end()); // Invertir a big-endian para Boost

    cpp_int fixed_point_value;
    boost::multiprecision::import_bits(fixed_point_value, bytes.begin(), bytes.end());

    cpp_int divisor = 1;
    divisor <<= precision;

    cpp_dec_float_100 float_result = cpp_dec_float_100(fixed_point_value) / cpp_dec_float_100(divisor);

    std::string std_string_result = float_result.str(100, std::ios_base::fixed);

    std_string_result.erase(std_string_result.find_last_not_of('0') + 1, std::string::npos);
    if (std_string_result.back() == '.') {
        std_string_result.pop_back();
    }

    return {std_string_result.c_str()};
}