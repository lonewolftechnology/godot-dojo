#include "ref_counted/dojo_utilities/big_int/u128.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <vector>

#include "tools/logger.hpp"

// Suppress false positive warning from GCC/Boost interaction
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif

using boost::multiprecision::cpp_int;

void U128::_init_from_int(int64_t p_value) {
    cpp_int val = p_value;
    if (p_value < 0) {
        is_signed = true;
        signed_value = val.convert_to<int128_t>();
    } else {
        is_signed = false;
        signed_value = 0;
    }

    if (val < 0) {
        val += (cpp_int(1) << 128);
    }
    value = val.convert_to<uint128_t>();
}

void U128::_init_from_string(const String& p_value)
{
    value = uint128_t(p_value.utf8().get_data());
}

void U128::_init_from_float(double p_value, int p_precision) {
    if (p_precision < 0) {
        p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/default");
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

    if (p_value < 0) {
        is_signed = true;
        signed_value = val.convert_to<int128_t>();
    } else {
        is_signed = false;
        signed_value = 0;
    }

    if (val < 0) {
        val += (cpp_int(1) << 128);
    }
    value = val.convert_to<uint128_t>();
}

String U128::to_string() const {
    std::stringstream ss;
    ss << "0x" << std::hex << value;
    return String(ss.str().c_str());
}

String U128::_to_string() const
{
    return to_string();
}

PackedStringArray U128::to_calldata() const {
    PackedStringArray arr;
    arr.append(to_string());
    return arr;
}

PackedByteArray U128::to_bytes() const {
    PackedByteArray arr;
    arr.resize(16);
    uint8_t* ptr = arr.ptrw();
    for (int i = 0; i < 16; ++i) {
        ptr[i] = 0;
    }

    std::vector<uint8_t> vec;
    boost::multiprecision::export_bits(value, std::back_inserter(vec), 8);

    int count = vec.size();
    if (count > 16) count = 16;
    int offset = 16 - count;
    for (int i = 0; i < count; ++i) {
        ptr[offset + i] = vec[i];
    }
    return arr;
}

double U128::to_float(int p_precision) const {
    if (p_precision < 0) {
        p_precision = ProjectSettings::get_singleton()->get_setting("dojo/config/fixed_point/default");
    }

    if (p_precision > 2048) {
        p_precision = 2048;
    }

    typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100> > cpp_dec_float_100;
    
    cpp_dec_float_100 float_val;
    if (is_signed) {
        float_val = cpp_dec_float_100(signed_value);
    } else {
        float_val = cpp_dec_float_100(value);
    }
    
    float_val = boost::multiprecision::ldexp(float_val, -p_precision);
    return static_cast<double>(float_val);
}

Ref<U128> U128::from_int(int64_t p_value) {
    Ref<U128> instance = memnew(U128);
    instance->_init_from_int(p_value);
    return instance;
}

Ref<U128> U128::from_string(const String& p_value)
{
    Ref<U128> instance = memnew(U128);
    instance->_init_from_string(p_value);
    return instance;
}

Ref<U128> U128::from_float(double p_value, int p_precision) {
    Ref<U128> instance = memnew(U128);
    instance->_init_from_float(p_value, p_precision);
    return instance;
}

Ref<U128> U128::from_variant(const Variant& p_value) {
    if (p_value.get_type() == Variant::OBJECT) {
        Ref<U128> casted = p_value;
        if (casted.is_valid()) {
            return casted;
        }
    }

    Ref<U128> instance = memnew(U128);
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

Array U128::from_vector(const Variant& p_value) {
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

void U128::_bind_methods() {
    ClassDB::bind_method(D_METHOD("to_string"), &U128::to_string);
    ClassDB::bind_method(D_METHOD("_to_string"), &U128::_to_string);
    ClassDB::bind_method(D_METHOD("to_float", "precision"), &U128::to_float, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("to_calldata"), &U128::to_calldata);
    ClassDB::bind_method(D_METHOD("to_bytes"), &U128::to_bytes);
    ClassDB::bind_static_method("U128", D_METHOD("from_int", "value"), &U128::from_int);
    ClassDB::bind_static_method("U128", D_METHOD("from_string", "value"), &U128::from_string);
    ClassDB::bind_static_method("U128", D_METHOD("from_float", "value", "precision"), &U128::from_float, DEFVAL(-1));
    ClassDB::bind_static_method("U128", D_METHOD("from_variant", "value"), &U128::from_variant);
    ClassDB::bind_static_method("U128", D_METHOD("from_vector", "value"), &U128::from_vector);
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
