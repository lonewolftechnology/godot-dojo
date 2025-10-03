#include "tools/dojo_helper.h"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "tools/logger.h"
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <types/big_int.h>
#include <deque>
#include "variant/field_element.h"

DojoHelpers *DojoHelpers::singleton = nullptr;
using boost::multiprecision::cpp_int;
using boost::multiprecision::int128_t;
using boost::multiprecision::uint128_t;
using boost::multiprecision::uint256_t;
using boost::multiprecision::int256_t;

typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<100> > cpp_dec_float_100;

// The STARK prime P, as a constant.
// P = 2^251 + 17 * 2^192 + 1
const cpp_int DojoHelpers::STARK_PRIME = (cpp_int(1) << 251) + (cpp_int(17) * (cpp_int(1) << 192)) + 1;

cpp_int DojoHelpers::to_starknet_negative_felt(cpp_int val) {
    if (val < 0) {
        val += STARK_PRIME;
    }
    return val;
}

DojoHelpers::DojoHelpers() {
    singleton = this;
}

DojoHelpers::~DojoHelpers() {
    singleton = nullptr;
}

String DojoHelpers::get_katana_url() {
    return get_setting("dojo/config/katana_url");
}

Variant DojoHelpers::get_setting(const String &setting, const Variant &default_value) {
    if (default_value.get_type() == Variant::NIL) {
        return ProjectSettings::get_singleton()->get_setting(setting);
    }
    return ProjectSettings::get_singleton()->get_setting(setting, default_value);
}

Variant DojoHelpers::get_custom_setting(const String& category, const String &setting) {
    Variant result = get_setting(vformat("%s/%s", category, setting));
    if (result.get_type() == Variant::NIL) {
        Logger::debug_extra("DojoHelper", vformat("%s not found, trying dojo setting", setting));
        result = get_dojo_setting(setting);
    }
    return result;
}

Variant DojoHelpers::get_dojo_setting(const String &setting) {
    return get_setting("dojo/config/" + setting);
}

Ref<DojoPolicies> DojoHelpers::get_default_policies() {
    Dictionary default_policies = get_setting("dojo/config/policies", TypedArray<DojoPolicy>());

    String contract_address = get_setting("dojo/config/contract_address", "0x0");
    Ref<DojoPolicies> policies = {};
    policies.instantiate();
    policies->set_dojo_contract(contract_address);
    policies->set_policies(default_policies);
    return policies;
}

// these use boost::multiprecision
double DojoHelpers::variant_to_double_fp(const Variant &value, const int &precision) {
    cpp_int int_val;
    // TODO convert variant (from Cairo, could be a simple int or array of ints for u128 and up) to cpp_int
    switch (value.get_type()) {
        case Variant::BOOL:
        case Variant::INT:
        case Variant::FLOAT:
            int_val = static_cast<int64_t>(value);
            break;
        case Variant::STRING:
            int_val = cpp_int(String(value).utf8().get_data());
            break;
        case Variant::ARRAY: {
            // Other variant array types go here?
            // take the bytes from the array here and construct the big integer
            // Assumes little-endian PackedByteArray
            PackedByteArray bytes = value;
            for (int i = bytes.size() - 1; i >= 0; i--) {
                int_val <<= 8;
                int_val |= bytes[i];
            }
            break;
        }
    };

    cpp_int divisor = 1;
    divisor <<= precision;
    cpp_dec_float_100 result = cpp_dec_float_100(int_val) / cpp_dec_float_100(divisor);

    return static_cast<double>(result);
}

Variant DojoHelpers::double_to_variant_fp(const double &value, const int &precision) {
    cpp_int shift = 1;
    shift <<= precision;
    cpp_dec_float_100 val_100 = value;
    val_100 *= cpp_dec_float_100(shift);

    cpp_int val_int(val_100);

    // If it fits in int64, let the INT handler in prepare_dojo_call_data deal with it.
    // This handles small positive and negative numbers.
    if (val_int >= std::numeric_limits<int64_t>::min() && val_int <= std::numeric_limits<int64_t>::max()) {
        return {static_cast<int64_t>(val_int)};
    }

    val_int = to_starknet_negative_felt(val_int);

    PackedByteArray arr;
    int bytes = (val_int == 0) ? 0 : (msb(val_int) / 8) + 1;
    arr.resize(bytes);
    // Serialize as little-endian
    for (int i = 0; i < bytes; i++) {
        arr[i] = static_cast<uint8_t>(val_int & 0xff);
        val_int >>= 8;
    }
    return {arr};
}

int64_t DojoHelpers::float_to_fixed(const double &value, const int &precision) {
    return static_cast<int64_t>(value * (1LL << precision));
}

double DojoHelpers::fixed_to_float(const int &value, const int &precision) {
    return value / static_cast<double>(1LL << precision);
}

int64_t DojoHelpers::float_to_fixed_64(const float &value) {
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/64"));
}

double DojoHelpers::fixed_to_float_64(const int &value) {
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/64"));
}

int64_t DojoHelpers::float_to_fixed_128(const float &value) {
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/128"));
}

double DojoHelpers::fixed_to_float_128(const int &value) {
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/128"));
}

int64_t DojoHelpers::float_to_fixed_256(const float &value) {
    return float_to_fixed(value, get_setting("dojo/config/fixed_point/256"));
}

double DojoHelpers::fixed_to_float_256(const int &value) {
    return fixed_to_float(value, get_setting("dojo/config/fixed_point/256"));
}

String DojoHelpers::u256_to_string_boost(const DOJO::U256 &u256) {
    std::vector<uint8_t> bytes(u256.data, u256.data + 32);

    std::reverse(bytes.begin(), bytes.end());

    cpp_int big_num;

    import_bits(big_num, bytes.begin(), bytes.end());

    return {big_num.str().c_str()};
}

DOJO::U256 DojoHelpers::string_to_fixed_point_u256(const String &integer_str, int precision) {
    cpp_int integer_value(integer_str.utf8().get_data());

    cpp_int multiplier = 1;
    multiplier <<= precision;

    cpp_int fixed_point_value = integer_value * multiplier;

    if (fixed_point_value < 0) {
        fixed_point_value += STARK_PRIME;
    }

    cpp_int max_u256 = (cpp_int(1) << 256) - 1;
    if (fixed_point_value > max_u256) {
        Logger::error("Error: El valor de punto fijo excede los 256 bits.");
        return {};
    }

    std::vector<uint8_t> bytes;
    boost::multiprecision::export_bits(fixed_point_value, std::back_inserter(bytes), 8);

    DOJO::U256 result = {};

    size_t num_bytes = bytes.size();
    if (num_bytes > 32) {
        Logger::error("Error: export_bits generó más de 32 bytes.");
        return {};
    }

    uint8_t temp_buffer[32] = {0};
    memcpy(temp_buffer + (32 - num_bytes), bytes.data(), num_bytes);

    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));

    memcpy(result.data, temp_buffer, 32);

    return result;
}

String DojoHelpers::u256_fixed_point_to_string(const DOJO::U256 &u256, int precision) {
    std::vector<uint8_t> bytes(u256.data, u256.data + 32);
    std::reverse(bytes.begin(), bytes.end()); // Invertir a big-endian para Boost

    cpp_int fixed_point_value;
    boost::multiprecision::import_bits(fixed_point_value, bytes.begin(), bytes.end());

    if (fixed_point_value > STARK_PRIME / 2) {
        fixed_point_value -= STARK_PRIME;
    }

    cpp_int divisor = 1;
    divisor <<= precision;

    cpp_dec_float_100 float_result = cpp_dec_float_100(fixed_point_value) / cpp_dec_float_100(divisor);

    std::string std_string_result = float_result.str(100, std::ios_base::fixed);

    std_string_result.erase(std_string_result.find_last_not_of('0') + 1, std::string::npos);
    if (!std_string_result.empty() && std_string_result.back() == '.') {
        std_string_result.pop_back();
    }

    return {std_string_result.c_str()};
}

void DojoHelpers::string_to_i8(const String &str, int8_t *out_val) {
    *out_val = static_cast<int8_t>(str.to_int());
}

void DojoHelpers::string_to_i16(const String &str, int16_t *out_val) {
    *out_val = static_cast<int16_t>(str.to_int());
}

void DojoHelpers::string_to_i32(const String &str, int32_t *out_val) {
    *out_val = static_cast<int32_t>(str.to_int());
}

void DojoHelpers::string_to_i64(const String &str, int64_t *out_val) {
    *out_val = str.to_int();
}

void DojoHelpers::string_to_i128(const String &str, uint8_t *out_val) {
    int128_t val(str.utf8().get_data());
    std::vector<uint8_t> bytes;
    export_bits(val, std::back_inserter(bytes), 8);

    size_t num_bytes = bytes.size();
    if (num_bytes > 16) {
        Logger::error("Error: String value exceeds 128 bits.");
        return;
    }

    uint8_t temp_buffer[16];
    uint8_t padding = (val < 0) ? 0xFF : 0x00;
    memset(temp_buffer, padding, 16);

    memcpy(temp_buffer + (16 - num_bytes), bytes.data(), num_bytes);
    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));
    memcpy(out_val, temp_buffer, 16);
}

void DojoHelpers::string_to_u128(const String &str, uint8_t *out_val) {
    uint128_t val(str.utf8().get_data());
    std::vector<uint8_t> bytes;
    export_bits(val, std::back_inserter(bytes), 8);

    size_t num_bytes = bytes.size();
    if (num_bytes > 16) {
        Logger::error("Error: String value exceeds 128 bits.");
        return;
    }

    uint8_t temp_buffer[16] = {0};
    memcpy(temp_buffer + (16 - num_bytes), bytes.data(), num_bytes);
    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));
    memcpy(out_val, temp_buffer, 16);
}

DOJO::U256 DojoHelpers::string_to_u256(const String &str) {
    uint256_t val(str.utf8().get_data());
    std::vector<uint8_t> bytes;
    export_bits(val, std::back_inserter(bytes), 8);

    DOJO::U256 result = {};
    size_t num_bytes = bytes.size();
    if (num_bytes > 32) {
        Logger::error("Error: String value exceeds 256 bits.");
        return result;
    }

    uint8_t temp_buffer[32] = {0};
    memcpy(temp_buffer + (32 - num_bytes), bytes.data(), num_bytes);
    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));
    memcpy(result.data, temp_buffer, 32);

    return result;
}

void DojoHelpers::string_to_i256(const String &str, uint8_t *out_val) {
    int256_t val(str.utf8().get_data());
    std::vector<uint8_t> bytes;
    export_bits(val, std::back_inserter(bytes), 8);

    size_t num_bytes = bytes.size();
    if (num_bytes > 32) {
        Logger::error("Error: String value exceeds 256 bits.");
        return;
    }

    uint8_t temp_buffer[32];
    uint8_t padding = (val < 0) ? 0xFF : 0x00;
    memset(temp_buffer, padding, 32);

    memcpy(temp_buffer + (32 - num_bytes), bytes.data(), num_bytes);
    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));
    memcpy(out_val, temp_buffer, 32);
}

void DojoHelpers::float_string_to_fixed_point_i128(const String &str, int precision, uint8_t *out_val) {
    cpp_dec_float_100 float_val(str.utf8().get_data());
    cpp_int multiplier = 1;
    multiplier <<= precision;

    int128_t fixed_point_val = static_cast<int128_t>(float_val * cpp_dec_float_100(multiplier));

    std::vector<uint8_t> bytes;
    export_bits(fixed_point_val, std::back_inserter(bytes), 8);

    size_t num_bytes = bytes.size();
    if (num_bytes > 16) {
        Logger::error("Error: Fixed point value exceeds 128 bits.");
        return;
    }

    uint8_t temp_buffer[16];
    uint8_t padding = (fixed_point_val < 0) ? 0xFF : 0x00;
    memset(temp_buffer, padding, 16);

    memcpy(temp_buffer + (16 - num_bytes), bytes.data(), num_bytes);
    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));
    memcpy(out_val, temp_buffer, 16);
}

String DojoHelpers::i128_fixed_point_to_float_string(const uint8_t *val, int precision) {
    std::vector<uint8_t> bytes(val, val + 16);
    std::reverse(bytes.begin(), bytes.end());

    int128_t fixed_point_val;
    import_bits(fixed_point_val, bytes.begin(), bytes.end());

    cpp_int divisor = 1;
    divisor <<= precision;

    cpp_dec_float_100 float_result = cpp_dec_float_100(fixed_point_val) / cpp_dec_float_100(divisor);

    return {float_result.str(100, std::ios_base::fixed).c_str()};
}

void DojoHelpers::float_string_to_fixed_point_i256(const String &str, int precision, uint8_t *out_val) {
    cpp_dec_float_100 float_val(str.utf8().get_data());
    cpp_int multiplier = 1;
    multiplier <<= precision;

    int256_t fixed_point_val = static_cast<int256_t>(float_val * cpp_dec_float_100(multiplier));

    std::vector<uint8_t> bytes;
    export_bits(fixed_point_val, std::back_inserter(bytes), 8);

    size_t num_bytes = bytes.size();
    if (num_bytes > 32) {
        Logger::error("Error: Fixed point value exceeds 256 bits.");
        return;
    }

    uint8_t temp_buffer[32];
    uint8_t padding = (fixed_point_val < 0) ? 0xFF : 0x00;
    memset(temp_buffer, padding, 32);

    memcpy(temp_buffer + (32 - num_bytes), bytes.data(), num_bytes);
    std::reverse(std::begin(temp_buffer), std::end(temp_buffer));
    memcpy(out_val, temp_buffer, 32);
}

String DojoHelpers::i256_fixed_point_to_float_string(const uint8_t *val, int precision) {
    std::vector<uint8_t> bytes(val, val + 32);
    std::reverse(bytes.begin(), bytes.end());

    int256_t fixed_point_val;
    import_bits(fixed_point_val, bytes.begin(), bytes.end());

    cpp_int divisor = 1;
    divisor <<= precision;

    cpp_dec_float_100 float_result = cpp_dec_float_100(fixed_point_val) / cpp_dec_float_100(divisor);

    return {float_result.str(100, std::ios_base::fixed).c_str()};
}

Ref<FieldElement> DojoHelpers::string_to_field_element(const String &str) {
    Ref<FieldElement> felt = memnew(FieldElement(str));
    return felt;
}

DojoCallData DojoHelpers::prepare_dojo_call_data(const String &to, const String &selector, const Array &args) {
    DojoCallData call_data;
    call_data.is_valid = false; // Default to invalid

    call_data.to = FieldElement::from_string(to);
    call_data.selector_str = selector.utf8().get_data();

    if (args.size() > 0) {
        Logger::debug_extra("DojoHelpers", "Preparing DojoCallData (Iterative)");

        std::deque<Variant> worklist;
        for (int i = 0; i < args.size(); ++i) {
            worklist.push_back(args[i]);
        }

        while (!worklist.empty()) {
            Variant arg = worklist.front();
            worklist.pop_front();
            String class_name = "";
            if (arg.get_type() == Variant::Type::OBJECT) {
                class_name = static_cast<Ref<RefCounted>>(arg)->get_class();
            } else {
                class_name = Variant::get_type_name(arg.get_type());
            }
            Logger::debug_extra("Calldata", "Processing: " + class_name, arg.stringify());

            switch (arg.get_type()) {
                case Variant::Type::ARRAY: {
                    Array inner_array = arg;
                    for (int i = inner_array.size() - 1; i >= 0; --i) {
                        worklist.push_front(inner_array[i]);
                    }
                    continue;
                }
                case Variant::Type::FLOAT: {
                    worklist.push_front(double_to_variant_fp(arg, get_setting("dojo/config/fixed_point/default")));
                    continue;
                }
                case Variant::Type::VECTOR2: {
                    Vector2 vec = arg;
                    worklist.push_front(vec.y);
                    worklist.push_front(vec.x);
                    continue;
                }
                case Variant::Type::VECTOR2I: {
                    Vector2i vec = arg;
                    worklist.push_front(static_cast<int64_t>(vec.y));
                    worklist.push_front(static_cast<int64_t>(vec.x));
                    continue;
                }
                case Variant::Type::VECTOR3: {
                    Vector3 vec = arg;
                    worklist.push_front(vec.z);
                    worklist.push_front(vec.y);
                    worklist.push_front(vec.x);
                    continue;
                }
                case Variant::Type::VECTOR3I: {
                    Vector3i vec = arg;
                    worklist.push_front(static_cast<int64_t>(vec.z));
                    worklist.push_front(static_cast<int64_t>(vec.y));
                    worklist.push_front(static_cast<int64_t>(vec.x));
                    continue;
                }
                case Variant::Type::BOOL:
                    call_data.calldata_felts.push_back(FieldElement::from_enum(arg));
                    break;
                case Variant::Type::INT: {
                    int64_t int_val = arg;
                    cpp_int felt_val;
                    if (int_val < 0) {
                        felt_val = to_starknet_negative_felt(int_val);
                    } else {
                        felt_val = int_val;
                    }

                    std::vector<uint8_t> bytes;
                    export_bits(felt_val, std::back_inserter(bytes), 8);

                    DOJO::FieldElement felt = {};
                    size_t num_bytes = bytes.size();
                    if (num_bytes <= 32) {
                        memcpy(felt.data + (32 - num_bytes), bytes.data(), num_bytes);
                    } else {
                        Logger::error("Calldata", "Integer value is too large for a field element.");
                        return call_data;
                    }
                    call_data.calldata_felts.push_back(felt);
                    break;
                }
                case Variant::Type::PACKED_BYTE_ARRAY: {
                    const PackedByteArray v_array = static_cast<PackedByteArray>(arg);
                    if (v_array.size() == 32) {
                        // u256 / felt252
                        DOJO::FieldElement felt;
                        memcpy(&felt, v_array.ptr(), 32);
                        call_data.calldata_felts.push_back(felt);
                    } else if (v_array.size() == 16) {
                        // u128 / i128
                        DOJO::FieldElement felt;
                        memset(&felt, 0, sizeof(DOJO::FieldElement));
                        memcpy(&felt, v_array.ptr(), 16);
                        call_data.calldata_felts.push_back(felt);
                    } else {
                        String error_msg = "Unsupported PackedByteArray size: " + String::num_int64(v_array.size()) +
                                           ". Expected 16 or 32.";
                        Logger::error("Calldata", error_msg);
                        return call_data; // is_valid is false
                    }
                    break;
                }
                case Variant::Type::STRING:
                    if (static_cast<String>(arg).begins_with("0x")) {
                        call_data.calldata_felts.push_back(FieldElement::from_string(arg));
                    } else {
                        call_data.calldata_felts.push_back(FieldElement::cairo_short_string_to_felt(arg));
                    }
                    break;
                case Variant::Type::OBJECT: {
                    Ref<RefCounted> obj = arg;
                    if (obj.is_valid()) {
                        if (obj->is_class("U256")) {
                            Ref<U256> u256 = obj;
                            // For Cairo calldata, the order is [low, high].
                            // We push high first, then low, so low is processed first.
                            worklist.push_front(u256->get_high());
                            worklist.push_front(u256->get_low());
                            continue;
                        }
                        if (obj->is_class("U128")) {
                            Ref<U128> u128 = obj;
                            // Convert to a 32-byte felt representation and let the PACKED_BYTE_ARRAY case handle it.
                            worklist.push_front(u128->_to_felt_bytes());
                            continue;
                        }
                        if (obj->is_class("I128")) {
                            Ref<I128> i128 = obj;
                            // Convert to a 32-byte felt representation and let the PACKED_BYTE_ARRAY case handle it.
                            worklist.push_front(i128->_to_felt_bytes());
                            continue;
                        }
                    }
                    Logger::warning(
                        "prepare_dojo_call_data: Encountered an unexpected OBJECT type in calldata. It will be ignored. Class: "
                        + class_name);
                    break;
                }
                default:
                    Logger::warning(
                        "prepare_dojo_call_data: Unsupported variant type in calldata: " + Variant::get_type_name(
                            arg.get_type()));
                    break;
            }
        }
    }
    Logger::debug_extra("Calldata", call_data.calldata_felts.size(), "felts");
    call_data.is_valid = true;
    return call_data;
}

bool DojoHelpers::is_valid_calldata(const Variant &calldata) {
    if (calldata.get_type() != Variant::Type::ARRAY) {
        Logger::error("Error: Invalid Array.");
        return false;
    }
    return true;
}

String DojoHelpers::bytes_to_i128_string(const PackedByteArray &bytes) {
    if (bytes.size() != 16) {
        Logger::error("Error: PackedByteArray must be 16 bytes for i128.");
        return "";
    }

    std::vector<uint8_t> byte_vector;
    byte_vector.resize(16);
    memcpy(byte_vector.data(), bytes.ptr(), 16);
    std::reverse(byte_vector.begin(), byte_vector.end());

    int128_t val;
    import_bits(val, byte_vector.begin(), byte_vector.end());

    return {val.str().c_str()};
}

String DojoHelpers::bytes_to_u128_string(const PackedByteArray &bytes) {
    if (bytes.size() != 16) {
        Logger::error("Error: PackedByteArray must be 16 bytes for u128.");
        return "";
    }

    std::vector<uint8_t> byte_vector;
    byte_vector.resize(16);
    memcpy(byte_vector.data(), bytes.ptr(), 16);
    std::reverse(byte_vector.begin(), byte_vector.end());

    uint128_t val;
    import_bits(val, byte_vector.begin(), byte_vector.end());

    return {val.str().c_str()};
}

String DojoHelpers::bytes_to_u256_string(const PackedByteArray &bytes) {
    if (bytes.size() != 32) {
        Logger::error("Error: PackedByteArray must be 32 bytes for u256.");
        return "";
    }

    std::vector<uint8_t> byte_vector;
    byte_vector.resize(32);
    memcpy(byte_vector.data(), bytes.ptr(), 32);
    std::reverse(byte_vector.begin(), byte_vector.end());

    uint256_t val;
    import_bits(val, byte_vector.begin(), byte_vector.end());

    return {val.str().c_str()};
}
