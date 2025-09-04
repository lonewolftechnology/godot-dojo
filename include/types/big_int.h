#ifndef BIG_INT_H
#define BIG_INT_H

#include <dojo_types.h>

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/core/class_db.hpp"
#include <boost/multiprecision/cpp_int.hpp>

using namespace godot;
using boost::multiprecision::uint128_t;
using boost::multiprecision::int128_t;
using boost::multiprecision::uint256_t;

// --- U128 ---
class U128 : public RefCounted
{
    GDCLASS(U128, RefCounted);

    uint128_t value;

public:
    U128();
    ~U128();

    void _init(const Variant& p_value = Variant());
    void _init_from_string(const String& p_value);
    void _init_from_int(int64_t p_value);
    void _init_from_float(double p_value, int p_precision);
    U128(const uint8_t p_bytes[16]);
    U128(const uint128_t& p_value);
    void set_value(const uint128_t& p_value) { value = p_value; }

    String to_string() const;
    Variant _to_string() const { return to_bytes(); };
    PackedByteArray to_bytes() const;
    DOJO::FieldElement to_felt() const;
    PackedByteArray _to_felt_bytes() const;

    static Ref<U128> from_variant(const Variant& p_value);

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("init_from_string", "value"), &U128::_init_from_string);
        ClassDB::bind_method(D_METHOD("init_from_int", "value"), &U128::_init_from_int);
        ClassDB::bind_method(D_METHOD("to_string"), &U128::to_string);
        ClassDB::bind_method(D_METHOD("to_bytes"), &U128::to_bytes);
        ClassDB::bind_method(D_METHOD("to_felt"), &U128::_to_felt_bytes);

        ClassDB::bind_static_method("U128", D_METHOD("from_variant", "value"), &U128::from_variant);
    }
};


// --- I128 ---
class I128 : public RefCounted
{
    GDCLASS(I128, RefCounted);

    int128_t value;

public:
    I128();
    ~I128();

    void _init(const Variant& p_value = Variant());
    void _init_from_string(const String& p_value);
    void _init_from_int(int64_t p_value);
    void _init_from_float(double p_value, int p_precision);
    I128(const uint8_t p_bytes[16]);
    I128(const int128_t& p_value);
    void set_value(const int128_t& p_value) { value = p_value; }

    String to_string() const;
    Variant _to_string() const { return to_bytes(); };
    PackedByteArray to_bytes() const;
    DOJO::FieldElement to_felt() const;
    PackedByteArray _to_felt_bytes() const;

    static Ref<I128> from_variant(const Variant& p_value);

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("init_from_string", "value"), &I128::_init_from_string);
        ClassDB::bind_method(D_METHOD("init_from_int", "value"), &I128::_init_from_int);
        ClassDB::bind_method(D_METHOD("to_string"), &I128::to_string);
        ClassDB::bind_method(D_METHOD("to_bytes"), &I128::to_bytes);
        ClassDB::bind_method(D_METHOD("to_felt"), &I128::_to_felt_bytes);

        ClassDB::bind_static_method("I128", D_METHOD("from_variant", "value"), &I128::from_variant);
    }
};

// --- U256 ---
class U256 : public RefCounted
{
    GDCLASS(U256, RefCounted);

    uint256_t value;

public:
    U256();
    ~U256();

    void _init(const Variant& p_value = Variant());
    void _init_from_string(const String& p_value);
    void _init_from_int(int64_t p_value);
    void _init_from_float(double p_value, int p_precision);
    U256(const DOJO::U256& p_value);
    void set_value(const uint256_t& p_value) { value = p_value; }

    String to_string() const;
    Variant _to_string() const { return to_bytes(); };
    PackedByteArray to_bytes() const;
    Ref<U128> get_low() const;
    Ref<U128> get_high() const;

    // @deprecated: This is incorrect for calldata. Use get_low/get_high.
    DOJO::FieldElement to_felt() const;
    PackedByteArray _to_felt_bytes() const; // Uses the deprecated to_felt

    static Ref<U256> from_variant(const Variant& p_value);

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("init_from_string", "value"), &U256::_init_from_string);
        ClassDB::bind_method(D_METHOD("init_from_int", "value"), &U256::_init_from_int);
        ClassDB::bind_method(D_METHOD("to_string"), &U256::to_string);
        ClassDB::bind_method(D_METHOD("to_bytes"), &U256::to_bytes);
        ClassDB::bind_method(D_METHOD("get_low"), &U256::get_low);
        ClassDB::bind_method(D_METHOD("get_high"), &U256::get_high);
        ClassDB::bind_method(D_METHOD("to_felt"), &U256::_to_felt_bytes);

        ClassDB::bind_static_method("U256", D_METHOD("from_variant", "value"), &U256::from_variant);
    }
};

#endif // BIG_INT_H
