#pragma once
#include "godot_cpp/classes/ref_counted.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include "godot_cpp/variant/packed_string_array.hpp"

using namespace godot;
using boost::multiprecision::uint256_t;

class U128;

class U256 : public RefCounted {
    GDCLASS(U256, RefCounted);
    uint256_t value;

public:
    U256() : value(0) {}

    void _init_from_int(int64_t p_value);
    void _init_from_string(const String& p_value);
    void _init_from_float(double p_value, int p_precision);
    String to_string() const;
    String _to_string() const;
    Ref<U128> get_low() const;
    Ref<U128> get_high() const;
    PackedStringArray to_calldata() const;
    double to_float(int p_precision = -1) const;

    static Ref<U256> from_int(int64_t p_value);
    static Ref<U256> from_string(const String& p_value);
    static Ref<U256> from_float(double p_value, int p_precision = -1);
    static Ref<U256> from_variant(const Variant& p_value);

protected:
    static void _bind_methods();
};