#include "types/big_int.h"
#include "tools/logger.h"
#include <algorithm> // For std::reverse
#include <type_traits> // For std::is_signed
#include <godot_cpp/classes/project_settings.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

using boost::multiprecision::cpp_int;
typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100>> cpp_dec_float_100;

// Common template for to_bytes to reduce duplication
template <typename T, size_t N>
PackedByteArray value_to_bytes(const T& value)
{
    std::vector<uint8_t> bytes_vec;
    boost::multiprecision::export_bits(value, std::back_inserter(bytes_vec), 8);

    PackedByteArray bytes;
    bytes.resize(N);
    uint8_t* write_ptr = bytes.ptrw();

    uint8_t padding = 0x00;
    if constexpr (std::is_signed<T>::value)
    {
        if (value < 0)
        {
            padding = 0xFF;
        }
    }
    memset(write_ptr, padding, N);

    // export_bits is big-endian, we need little-endian for contracts
    uint8_t temp_buffer[N];
    memset(temp_buffer, padding, N);

    if (bytes_vec.size() > N)
    {
        // This handles cases where boost might give more bytes than expected for negative numbers
        memcpy(temp_buffer, bytes_vec.data() + (bytes_vec.size() - N), N);
    }
    else
    {
        memcpy(temp_buffer + (N - bytes_vec.size()), bytes_vec.data(), bytes_vec.size());
    }

    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));
    memcpy(write_ptr, temp_buffer, N);

    return bytes;
}

template <typename T>
DOJO::FieldElement value_to_felt(const T& value)
{
    std::vector<uint8_t> bytes_vec;
    boost::multiprecision::export_bits(value, std::back_inserter(bytes_vec), 8);

    DOJO::FieldElement felt;

    uint8_t padding = 0x00;
    if constexpr (std::is_signed<T>::value)
    {
        if (value < 0)
        {
            padding = 0xFF;
        }
    }
    memset(felt.data, padding, 32);

    // export_bits is big-endian, we need little-endian for contracts
    uint8_t temp_buffer[32];
    memset(temp_buffer, padding, 32);

    if (bytes_vec.size() > 32)
    {
        // This handles cases where boost might give more bytes than expected for negative numbers
        memcpy(temp_buffer, bytes_vec.data() + (bytes_vec.size() - 32), 32);
    }
    else
    {
        memcpy(temp_buffer + (32 - bytes_vec.size()), bytes_vec.data(), bytes_vec.size());
    }

    memcpy(felt.data, temp_buffer, 32);

    return felt;
}

// Template helper to initialize any big integer class from a Variant.
// This removes code duplication from the _init methods and factory methods.
template <typename T>
void _initialize_from_variant(T* instance, const Variant& p_value)
{
    // Initialize to a known, safe state first.
    instance->set_value(0);

    switch (p_value.get_type())
    {
        case Variant::NIL:
            // This handles .new() called with no arguments.
            break;
        case Variant::STRING:
            instance->_init_from_string(p_value);
            break;
        case Variant::INT:
            instance->_init_from_int(p_value);
            break;
        case Variant::FLOAT:
            instance->_init_from_float(p_value, ProjectSettings::get_singleton()->get("dojo/config/fixed_point/default"));
            break;
        default:
        {
            String class_name = instance->get_class();
            Logger::error("Error: Invalid type for " + class_name + ".new() or .from_variant(). Must be int, float, String, or empty.");
            break;
        }
    }
}

// U128 Implementation
U128::U128()
{
}

U128::~U128()
{
}

void U128::_init(const Variant& p_value)
{
    // Default constructor for `U128.new()`
    _initialize_from_variant(this, p_value);
}

void U128::_init_from_string(const String& p_value)
{
    // Project is compiled with -fno-exceptions
    value = uint128_t(p_value.utf8().get_data());
}

void U128::_init_from_int(int64_t p_value) { value = p_value; }

void U128::_init_from_float(double p_value, int p_precision)
{
    cpp_int shift = 1;
    shift <<= p_precision;
    cpp_dec_float_100 val_100 = p_value;
    val_100 *= cpp_dec_float_100(shift);
    cpp_int val_int(val_100);

    // Re-use string initialization
    value = uint128_t(val_int.str().c_str());
}

U128::U128(const uint8_t p_bytes[16])
{
    std::vector<uint8_t> bytes(p_bytes, p_bytes + 16);
    std::reverse(bytes.begin(), bytes.end()); // to big-endian for boost
    boost::multiprecision::import_bits(value, bytes.begin(), bytes.end());
}

U128::U128(const uint128_t& p_value) : value(p_value)
{
}

String U128::to_string() const
{
    std::string s = value.str();
    return String(s.c_str());
}

PackedByteArray U128::to_bytes() const { return value_to_bytes<uint128_t, 16>(value); }

DOJO::FieldElement U128::to_felt() const { return value_to_felt<uint128_t>(value); }

Ref<U128> U128::from_variant(const Variant& p_value)
{
    Ref<U128> instance = memnew(U128);
    _initialize_from_variant(instance.ptr(), p_value);
    return instance;
}

PackedByteArray U128::_to_felt_bytes() const
{
    DOJO::FieldElement felt = to_felt();
    PackedByteArray bytes;
    bytes.resize(32);
    memcpy(bytes.ptrw(), felt.data, 32);
    return bytes;
}

// I128 Implementation
I128::I128()
{
}

I128::~I128()
{
}

void I128::_init(const Variant& p_value)
{
    // Default constructor for `I128.new()`
    _initialize_from_variant(this, p_value);
}

void I128::_init_from_string(const String& p_value)
{
    // Project is compiled with -fno-exceptions
    value = int128_t(p_value.utf8().get_data());
}

void I128::_init_from_int(int64_t p_value) { value = p_value; }

void I128::_init_from_float(double p_value, int p_precision)
{
    cpp_int shift = 1;
    shift <<= p_precision;
    cpp_dec_float_100 val_100 = p_value;
    val_100 *= cpp_dec_float_100(shift);
    cpp_int val_int(val_100);

    // Re-use string initialization
    value = int128_t(val_int.str().c_str());
}

I128::I128(const uint8_t p_bytes[16])
{
    std::vector<uint8_t> bytes(p_bytes, p_bytes + 16);
    std::reverse(bytes.begin(), bytes.end()); // to big-endian for boost
    boost::multiprecision::import_bits(value, bytes.begin(), bytes.end());
}

I128::I128(const int128_t& p_value) : value(p_value)
{
}

String I128::to_string() const
{
    std::string s = value.str();
    return String(s.c_str());
}

PackedByteArray I128::to_bytes() const { return value_to_bytes<int128_t, 16>(value); }

DOJO::FieldElement I128::to_felt() const { return value_to_felt<int128_t>(value); }

Ref<I128> I128::from_variant(const Variant& p_value)
{
    Ref<I128> instance = memnew(I128);
    _initialize_from_variant(instance.ptr(), p_value);
    return instance;
}

PackedByteArray I128::_to_felt_bytes() const
{
    DOJO::FieldElement felt = to_felt();
    PackedByteArray bytes;
    bytes.resize(32);
    memcpy(bytes.ptrw(), felt.data, 32);
    return bytes;
}

// U256 Implementation
U256::U256()
{
}

U256::~U256()
{
}

void U256::_init(const Variant& p_value)
{
    // Default constructor for `U256.new()`
    _initialize_from_variant(this, p_value);
}

void U256::_init_from_string(const String& p_value)
{
    // Project is compiled with -fno-exceptions
    value = uint256_t(p_value.utf8().get_data());
}

void U256::_init_from_int(int64_t p_value) { value = p_value; }

void U256::_init_from_float(double p_value, int p_precision)
{
    cpp_int shift = 1;
    shift <<= p_precision;
    cpp_dec_float_100 val_100 = p_value;
    val_100 *= cpp_dec_float_100(shift);
    cpp_int val_int(val_100);

    // Re-use string initialization
    value = uint256_t(val_int.str().c_str());
}

U256::U256(const DOJO::U256& p_value)
{
    std::vector<uint8_t> bytes(p_value.data, p_value.data + 32);
    std::reverse(bytes.begin(), bytes.end()); // to big-endian for boost
    boost::multiprecision::import_bits(value, bytes.begin(), bytes.end());
}

String U256::to_string() const
{
    std::string s = value.str();
    return String(s.c_str());
}

PackedByteArray U256::to_bytes() const { return value_to_bytes<uint256_t, 32>(value); }

Ref<U128> U256::get_low() const
{
    return memnew(U128(value.convert_to<uint128_t>()));
}

Ref<U128> U256::get_high() const
{
    return memnew(U128((value >> 128).convert_to<uint128_t>()));
}

DOJO::FieldElement U256::to_felt() const
{
    Logger::warning(
        "U256::to_felt() is deprecated and likely incorrect for calldata. A u256 is represented by two felts (low, high). This function truncates to the low part. Use get_low() and get_high() instead.");
    return value_to_felt<uint128_t>(value.convert_to<uint128_t>());
}

Ref<U256> U256::from_variant(const Variant& p_value)
{
    Ref<U256> instance = memnew(U256);
    _initialize_from_variant(instance.ptr(), p_value);
    return instance;
}

PackedByteArray U256::_to_felt_bytes() const
{
    DOJO::FieldElement felt = to_felt();
    PackedByteArray bytes;
    bytes.resize(32);
    memcpy(bytes.ptrw(), felt.data, 32);
    return bytes;
}
