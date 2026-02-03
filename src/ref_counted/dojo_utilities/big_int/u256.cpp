#include "ref_counted/dojo_utilities/big_int/u256.hpp"
#include "ref_counted/dojo_utilities/big_int/u128.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "tools/godot_helper.hpp"
#include <sstream>
#include <boost/multiprecision/cpp_dec_float.hpp>

// Suppress false positive warning from GCC/Boost interaction
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"

using boost::multiprecision::cpp_int;

void U256::_init_from_int(int64_t p_value) {
    int p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/default", 40);
    if (p_precision > 2048) {
        p_precision = 2048;
    }
    if (p_precision < 0) {
        p_precision = 0;
    }
    cpp_int val = p_value;
    val <<= p_precision;

    if (val < 0) {
        val = GodotDojoHelper::to_starknet_negative_felt(val);
    }
    value = val.convert_to<uint256_t>();
}

void U256::_init_from_string(const String& p_value) {
    value = uint256_t(p_value.utf8().get_data());
}

void U256::_init_from_float(double p_value, int p_precision) {
    if (p_precision < 0) {
        p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/default", 40);
    }
    if (p_precision > 2048) {
        p_precision = 2048;
    }
    if (p_precision < 0) {
        p_precision = 0;
    }
    typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100> > cpp_dec_float_100;
    cpp_dec_float_100 float_val(p_value);
    cpp_int multiplier = 1;
    multiplier <<= p_precision;
    cpp_int val = static_cast<cpp_int>(float_val * cpp_dec_float_100(multiplier));

    if (val < 0) {
        val = GodotDojoHelper::to_starknet_negative_felt(val);
    }
    value = val.convert_to<uint256_t>();
}

String U256::to_string() const {
    std::stringstream ss;
    ss << "0x" << std::hex << value;
    return {ss.str().c_str()};
}

String U256::_to_string() const
{
    return to_string();
}

Ref<U128> U256::get_low() const {
    Ref<U128> u128 = memnew(U128);
    u128->set_value(value.convert_to<uint128_t>());
    return u128;
}

Ref<U128> U256::get_high() const {
    Ref<U128> u128 = memnew(U128);
    u128->set_value((value >> 128).convert_to<uint128_t>());
    return u128;
}

PackedStringArray U256::to_calldata() const {
    PackedStringArray arr;
    Ref<U128> low = get_low();
    Ref<U128> high = get_high();
    arr.append(low->to_string());
    arr.append(high->to_string());
    return arr;
}

double U256::to_float(int p_precision) const {
    if (p_precision < 0) {
        p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/default", 40);
    }
    
    if (p_precision > 2048) {
        p_precision = 2048;
    }

    cpp_int val = value;
    if (val > GodotDojoHelper::STARK_PRIME / 2) {
        val -= GodotDojoHelper::STARK_PRIME;
    }

    typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100> > cpp_dec_float_100;
    
    cpp_dec_float_100 float_val(val);
    float_val = boost::multiprecision::ldexp(float_val, -p_precision);
    return static_cast<double>(float_val);
}

Ref<U256> U256::from_int(int64_t p_value) {
    Ref<U256> instance = memnew(U256);
    instance->_init_from_int(p_value);
    return instance;
}

Ref<U256> U256::from_string(const String& p_value) {
    Ref<U256> instance = memnew(U256);
    instance->_init_from_string(p_value);
    return instance;
}

Ref<U256> U256::from_float(double p_value, int p_precision) {
    Ref<U256> instance = memnew(U256);
    instance->_init_from_float(p_value, p_precision);
    return instance;
}

Ref<U256> U256::from_variant(const Variant& p_value) {
    if (p_value.get_type() == Variant::OBJECT) {
        Ref<U256> casted = p_value;
        if (casted.is_valid()) {
            return casted;
        }
    }

    Ref<U256> instance = memnew(U256);
    switch (p_value.get_type()) {
        case Variant::INT:
            instance->_init_from_int(p_value);
            break;
        case Variant::FLOAT:
            instance->_init_from_float(p_value, -1);
            break;
        case Variant::STRING:
            instance->_init_from_string(p_value);
            break;
        default:
            break;
    }
    return instance;
}

void U256::_bind_methods() {
    ClassDB::bind_method(D_METHOD("to_string"), &U256::to_string);
    ClassDB::bind_method(D_METHOD("_to_string"), &U256::_to_string);
    ClassDB::bind_method(D_METHOD("to_float", "precision"), &U256::to_float, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("get_low"), &U256::get_low);
    ClassDB::bind_method(D_METHOD("get_high"), &U256::get_high);
    ClassDB::bind_method(D_METHOD("to_calldata"), &U256::to_calldata);
    ClassDB::bind_static_method("U256", D_METHOD("from_int", "value"), &U256::from_int);
    ClassDB::bind_static_method("U256", D_METHOD("from_string", "value"), &U256::from_string);
    ClassDB::bind_static_method("U256", D_METHOD("from_float", "value", "precision"), &U256::from_float, DEFVAL(-1));
    ClassDB::bind_static_method("U256", D_METHOD("from_variant", "value"), &U256::from_variant);
}

#pragma GCC diagnostic pop