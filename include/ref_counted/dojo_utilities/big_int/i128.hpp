#pragma once
#include "godot_cpp/classes/ref_counted.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include "godot_cpp/variant/packed_string_array.hpp"

using namespace godot;
using boost::multiprecision::int128_t;

class I128 : public RefCounted {
    GDCLASS(I128, RefCounted);
    int128_t value;

public:
    I128() : value(0) {}

    void _init_from_int(int64_t p_value);
    void _init_from_string(const String& p_value);
    void _init_from_float(double p_value, int p_precision);

    String to_string() const;
    PackedStringArray to_calldata() const;
    double to_float(int p_precision = -1) const;

    static Ref<I128> from_int(int64_t p_value);
    static Ref<I128> from_string(const String& p_value);
    static Ref<I128> from_float(double p_value, int p_precision = -1);
    static Ref<I128> from_variant(const Variant& p_value);

protected:
    static void _bind_methods();
};