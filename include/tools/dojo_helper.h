//
// Created by hazel on 1/07/25.
//

#ifndef DOJO_HELPER_H
#define DOJO_HELPER_H

#include "godot_cpp/classes/object.hpp"
#include "dojo_types.h"
#include "godot_cpp/core/class_db.hpp"
#include "variant/field_element.h"
#include <boost/multiprecision/cpp_int.hpp>
using namespace godot;
using boost::multiprecision::cpp_int;

// Struct to hold the data for a DOJO::Call
// This is used to prepare the data in a helper and then construct the
// final DOJO::Call in the calling scope to avoid lifetime issues.
struct DojoCallData {
    DOJO::FieldElement to;
    std::string selector_str;
    std::vector<DOJO::FieldElement> calldata_felts;
    bool is_valid = false;
};

class DojoHelpers : public Object
{
    GDCLASS(DojoHelpers, Object)
    static const cpp_int STARK_PRIME;
    static cpp_int to_starknet_negative_felt(cpp_int val);

public:
    DojoHelpers();
    ~DojoHelpers();

    static constexpr uint64_t U32_OFFSET = 2147483648ULL; // 2^31

    static uint32_t signed_to_u32_offset(int32_t signed_value) {
        return static_cast<uint32_t>(static_cast<int64_t>(signed_value) + U32_OFFSET);
    }

    static int32_t u32_to_signed_offset(uint32_t u32_value) {
        return static_cast<int32_t>(static_cast<uint64_t>(u32_value) - U32_OFFSET);
    }

    static DojoHelpers* get_singleton() { return singleton; };
    static String get_katana_url();
    static Variant get_setting(const String& setting);

    static int64_t float_to_fixed(const double& value, const int& precision = 24);
    static double fixed_to_float(const int& value, const int& precision = 24);

    static int64_t float_to_fixed_64(const float& value);
    static double fixed_to_float_64(const int& value);

    static int64_t float_to_fixed_128(const float& value);
    static double fixed_to_float_128(const int& value);

    static int64_t float_to_fixed_256(const float& value);
    static double fixed_to_float_256(const int& value);

    static double variant_to_double_fp(const Variant& value, const int& precision);
    static Variant double_to_variant_fp(const double& value, const int& precision);

    static String u256ToString(const DOJO::U256& u256);
    static String packed_byte_array_to_numeric_string(const PackedByteArray& bytes);
    static String u256_to_string_boost(const DOJO::U256& u256);
    static String fixed_point_to_string(const String& fixed_point_value_str, int precision);
    static DOJO::U256 string_to_fixed_point_u256(const String& integer_str, int precision);
    static String u256_fixed_point_to_string(const dojo_bindings::U256& u256, int precision);

    static void string_to_i8(const String& str, int8_t* out_val);
    static void string_to_i16(const String& str, int16_t* out_val);
    static void string_to_i32(const String& str, int32_t* out_val);
    static void string_to_i64(const String& str, int64_t* out_val);
    static void string_to_i128(const String& str, uint8_t* out_val);
    static void string_to_u128(const String& str, uint8_t* out_val);
    static DOJO::U256 string_to_u256(const String& str);
    static void string_to_i256(const String& str, uint8_t* out_val);

    static void float_string_to_fixed_point_i128(const String& str, int precision, uint8_t* out_val);
    static String i128_fixed_point_to_float_string(const uint8_t* val, int precision);

    static void float_string_to_fixed_point_i256(const String& str, int precision, uint8_t* out_val);
    static String i256_fixed_point_to_float_string(const uint8_t* val, int precision);

    static String bytes_to_i128_string(const PackedByteArray& bytes);
    static String bytes_to_u128_string(const PackedByteArray& bytes);
    static String bytes_to_u256_string(const PackedByteArray& bytes);

    static Ref<FieldElement> string_to_field_element(const String& str);
    static DojoCallData prepare_dojo_call_data(const String& to, const String& selector, const Array& args);
    static bool is_valid_calldata(const Variant& calldata);


protected:
    static void _bind_methods()
    {
        ClassDB::bind_static_method("DojoHelpers", D_METHOD("get_katana_url"), &DojoHelpers::get_katana_url);
        ClassDB::bind_static_method("DojoHelpers", D_METHOD("get_setting", "setting"), &DojoHelpers::get_setting);

        ClassDB::bind_static_method("DojoHelpers", D_METHOD("signed_to_u32_offset", "signed_value"), &DojoHelpers::signed_to_u32_offset);
        ClassDB::bind_static_method("DojoHelpers", D_METHOD("u32_to_signed_offset", "u32_value"), &DojoHelpers::u32_to_signed_offset);

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
        ClassDB::bind_static_method("DojoHelpers", D_METHOD("string_to_field_element", "str"), &DojoHelpers::string_to_field_element);

        ClassDB::bind_static_method("DojoHelpers", D_METHOD("double_to_variant_fp", "value", "precision"), &DojoHelpers::double_to_variant_fp);
        ClassDB::bind_static_method("DojoHelpers", D_METHOD("variant_to_double_fp", "value", "precision"), &DojoHelpers::variant_to_double_fp);

        ClassDB::bind_static_method("DojoHelpers", D_METHOD("bytes_to_i128_string", "bytes"), &DojoHelpers::bytes_to_i128_string);
        ClassDB::bind_static_method("DojoHelpers", D_METHOD("bytes_to_u128_string", "bytes"), &DojoHelpers::bytes_to_u128_string);
        ClassDB::bind_static_method("DojoHelpers", D_METHOD("bytes_to_u256_string", "bytes"), &DojoHelpers::bytes_to_u256_string);
    }
    static DojoHelpers* singleton;

};


#endif //DOJO_HELPER_H
