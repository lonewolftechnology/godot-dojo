#include "ref_counted/dojo_utilities/big_int/u256.hpp"
#include "ref_counted/dojo_utilities/big_int/u128.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "tools/godot_helper.hpp"
#include <sstream>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <vector>

#include "tools/logger.hpp"

// Suppress false positive warning from GCC/Boost interaction
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

using boost::multiprecision::cpp_int;

void U256::_init_from_int(int64_t p_value) {
    cpp_int val = p_value;
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
        p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/256");
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

PackedByteArray U256::to_bytes() const {
    PackedByteArray arr;
    arr.resize(32);
    uint8_t* ptr = arr.ptrw();
    for (int i = 0; i < 32; ++i) {
        ptr[i] = 0;
    }

    std::vector<uint8_t> vec;
    boost::multiprecision::export_bits(value, std::back_inserter(vec), 8);

    int count = vec.size();
    if (count > 32) count = 32;
    int offset = 32 - count;
    for (int i = 0; i < count; ++i) {
        ptr[offset + i] = vec[i];
    }
    return arr;
}

double U256::to_float(int p_precision) const {
    if (p_precision < 0) {
        p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/256");
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

Array U256::from_vector(const Variant& p_value) {
    Array arr;
    switch (p_value.get_type()) {
        case Variant::VECTOR2: {
            Vector2 v = p_value;
            arr.append(from_float(v.x));
            arr.append(from_float(v.y));
            break;
        }
        case Variant::VECTOR2I: {
            Vector2i v = p_value;
            arr.append(from_int(v.x));
            arr.append(from_int(v.y));
            break;
        }
        case Variant::VECTOR3: {
            Vector3 v = p_value;
            arr.append(from_float(v.x));
            arr.append(from_float(v.y));
            arr.append(from_float(v.z));
            break;
        }
        case Variant::VECTOR3I: {
            Vector3i v = p_value;
            arr.append(from_int(v.x));
            arr.append(from_int(v.y));
            arr.append(from_int(v.z));
            break;
        }
        case Variant::VECTOR4: {
            Vector4 v = p_value;
            arr.append(from_float(v.x));
            arr.append(from_float(v.y));
            arr.append(from_float(v.z));
            arr.append(from_float(v.w));
            break;
        }
        case Variant::VECTOR4I: {
            Vector4i v = p_value;
            arr.append(from_int(v.x));
            arr.append(from_int(v.y));
            arr.append(from_int(v.z));
            arr.append(from_int(v.w));
            break;
        }
        default:
            Logger::error("Only Vector type are supported");
            break;
    }
    return arr;
}

void U256::_bind_methods() {
    ClassDB::bind_method(D_METHOD("to_string"), &U256::to_string);
    ClassDB::bind_method(D_METHOD("_to_string"), &U256::_to_string);
    ClassDB::bind_method(D_METHOD("to_float", "precision"), &U256::to_float, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("get_low"), &U256::get_low);
    ClassDB::bind_method(D_METHOD("get_high"), &U256::get_high);
    ClassDB::bind_method(D_METHOD("to_calldata"), &U256::to_calldata);
    ClassDB::bind_method(D_METHOD("to_bytes"), &U256::to_bytes);
    ClassDB::bind_static_method("U256", D_METHOD("from_int", "value"), &U256::from_int);
    ClassDB::bind_static_method("U256", D_METHOD("from_string", "value"), &U256::from_string);
    ClassDB::bind_static_method("U256", D_METHOD("from_float", "value", "precision"), &U256::from_float, DEFVAL(-1));
    ClassDB::bind_static_method("U256", D_METHOD("from_variant", "value"), &U256::from_variant);
    ClassDB::bind_static_method("U256", D_METHOD("from_vector", "value"), &U256::from_vector);
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif