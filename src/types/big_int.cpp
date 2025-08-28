#include "types/big_int.h"
#include "tools/logger.h"
#include <algorithm> // For std::reverse
#include <type_traits> // For std::is_signed

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

// U128 Implementation
U128::U128()
{
}

U128::~U128()
{
}

void U128::_init() { value = 0; }

void U128::_init_from_string(const String& p_value)
{
    // Project is compiled with -fno-exceptions
    value = uint128_t(p_value.utf8().get_data());
}

U128::U128(const uint8_t p_bytes[16])
{
    std::vector<uint8_t> bytes(p_bytes, p_bytes + 16);
    std::reverse(bytes.begin(), bytes.end()); // to big-endian for boost
    boost::multiprecision::import_bits(value, bytes.begin(), bytes.end());
}

String U128::to_string() const
{
    std::string s = value.str();
    return String(s.c_str());
}

PackedByteArray U128::to_bytes() const { return value_to_bytes<uint128_t, 16>(value); }

void U128::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_init_from_string", "value"), &U128::_init_from_string);
    ClassDB::bind_method(D_METHOD("to_string"), &U128::to_string);
    ClassDB::bind_method(D_METHOD("to_bytes"), &U128::to_bytes);
}

// I128 Implementation
I128::I128()
{
}

I128::~I128()
{
}

void I128::_init() { value = 0; }

void I128::_init_from_string(const String& p_value)
{
    // Project is compiled with -fno-exceptions
    value = int128_t(p_value.utf8().get_data());
}

I128::I128(const uint8_t p_bytes[16])
{
    std::vector<uint8_t> bytes(p_bytes, p_bytes + 16);
    std::reverse(bytes.begin(), bytes.end()); // to big-endian for boost
    boost::multiprecision::import_bits(value, bytes.begin(), bytes.end());
}

String I128::to_string() const
{
    std::string s = value.str();
    return String(s.c_str());
}

PackedByteArray I128::to_bytes() const { return value_to_bytes<int128_t, 16>(value); }

void I128::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_init_from_string", "value"), &I128::_init_from_string);
    ClassDB::bind_method(D_METHOD("to_string"), &I128::to_string);
    ClassDB::bind_method(D_METHOD("to_bytes"), &I128::to_bytes);
}

// U256 Implementation
U256::U256()
{
}

U256::~U256()
{
}

void U256::_init() { value = 0; }

void U256::_init_from_string(const String& p_value)
{
    // Project is compiled with -fno-exceptions
    value = uint256_t(p_value.utf8().get_data());
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

void U256::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("_init_from_string", "value"), &U256::_init_from_string);
    ClassDB::bind_method(D_METHOD("to_string"), &U256::to_string);
    ClassDB::bind_method(D_METHOD("to_bytes"), &U256::to_bytes);
}
