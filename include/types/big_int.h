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
using boost::multiprecision::int256_t;

// --- U128 ---
class U128 : public RefCounted {
    GDCLASS(U128, RefCounted);

private:
    uint128_t value;

protected:
    static void _bind_methods();

public:
    U128();
    ~U128();

    void _init();
    void _init_from_string(const String& p_value);
    U128(const uint8_t p_bytes[16]);

    String to_string() const;
    PackedByteArray to_bytes() const;
};

// --- I128 ---
class I128 : public RefCounted {
    GDCLASS(I128, RefCounted);

private:
    int128_t value;

protected:
    static void _bind_methods();

public:
    I128();
    ~I128();

    void _init();
    void _init_from_string(const String& p_value);
    I128(const uint8_t p_bytes[16]);

    String to_string() const;
    PackedByteArray to_bytes() const;
};

// --- U256 ---
class U256 : public RefCounted {
    GDCLASS(U256, RefCounted);

private:
    uint256_t value;

protected:
    static void _bind_methods();

public:
    U256();
    ~U256();

    void _init();
    void _init_from_string(const String& p_value);
    U256(const DOJO::U256& p_value);

    String to_string() const;
    PackedByteArray to_bytes() const;
};

#endif // BIG_INT_H