#include "ref_counted/dojo_utilities/big_int/i128.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "tools/godot_helper.hpp"
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <sstream>

// Suppress false positive warning from GCC/Boost interaction
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

using boost::multiprecision::cpp_int;

void I128::_init_from_int(int64_t p_value) {
    int p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/default", 40);
    if (p_precision > 2048) {
        p_precision = 2048;
    }
    if (p_precision < 0) {
        p_precision = 0;
    }
    cpp_int val = p_value;
    val <<= p_precision;
    value = val.convert_to<int128_t>();
}

void I128::_init_from_string(const String& p_value) {
    if (p_value.begins_with("0x")) {
        boost::multiprecision::uint128_t u_val(p_value.utf8().get_data());
        
        if (boost::multiprecision::bit_test(u_val, 127)) {
            cpp_int temp = u_val;
            temp -= (cpp_int(1) << 128);
            value = temp.convert_to<int128_t>();
        } else {
            value = u_val.convert_to<int128_t>();
        }
    } else {
        value = int128_t(p_value.utf8().get_data());
    }
}

void I128::_init_from_float(double p_value, int p_precision) {
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
    cpp_int fixed_point_val = static_cast<cpp_int>(float_val * cpp_dec_float_100(multiplier));
    value = fixed_point_val.convert_to<int128_t>();
}

String I128::to_string() const {
    cpp_int val = value;
    if (val < 0) {
        val = GodotDojoHelper::to_starknet_negative_felt(val);
    }
    
    std::stringstream ss;
    ss << "0x" << std::hex << val;
    return {ss.str().c_str()};
}

String I128::_to_string() const
{
    return to_string();
}

PackedStringArray I128::to_calldata() const {
    PackedStringArray arr;
    arr.append(to_string());
    return arr;
}

double I128::to_float(int p_precision) const {
    if (p_precision < 0) {
        p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/default", 40);
    }

    if (p_precision > 2048) {
        p_precision = 2048;
    }

    typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100> > cpp_dec_float_100;
    
    cpp_dec_float_100 float_val(value);
    float_val = boost::multiprecision::ldexp(float_val, -p_precision);
    return static_cast<double>(float_val);
}

Ref<I128> I128::from_int(int64_t p_value) {
    Ref<I128> instance = memnew(I128);
    instance->_init_from_int(p_value);
    return instance;
}

Ref<I128> I128::from_string(const String& p_value) {
    Ref<I128> instance = memnew(I128);
    instance->_init_from_string(p_value);
    return instance;
}

Ref<I128> I128::from_float(double p_value, int p_precision) {
    Ref<I128> instance = memnew(I128);
    instance->_init_from_float(p_value, p_precision);
    return instance;
}

Ref<I128> I128::from_variant(const Variant& p_value) {
    if (p_value.get_type() == Variant::OBJECT) {
        Ref<I128> casted = p_value;
        if (casted.is_valid()) {
            return casted;
        }
    }

    Ref<I128> instance = memnew(I128);
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

Array I128::from_vector(const Variant& p_value) {
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
            break;
    }
    return arr;
}

void I128::_bind_methods() {
    ClassDB::bind_method(D_METHOD("to_string"), &I128::to_string);
    ClassDB::bind_method(D_METHOD("_to_string"), &I128::_to_string);
    ClassDB::bind_method(D_METHOD("to_float", "precision"), &I128::to_float, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("to_calldata"), &I128::to_calldata);
    ClassDB::bind_static_method("I128", D_METHOD("from_int", "value"), &I128::from_int);
    ClassDB::bind_static_method("I128", D_METHOD("from_string", "value"), &I128::from_string);
    ClassDB::bind_static_method("I128", D_METHOD("from_float", "value", "precision"), &I128::from_float, DEFVAL(-1));
    ClassDB::bind_static_method("I128", D_METHOD("from_variant", "value"), &I128::from_variant);
    ClassDB::bind_static_method("I128", D_METHOD("from_vector", "value"), &I128::from_vector);
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif