#include "ref_counted/dojo_utilities/felt.hpp"

void Felt::_init_from_string(const String& p_value) {
    value = cpp_int(p_value.utf8().get_data());
}

String Felt::to_string() const {
    std::stringstream ss;
    ss << "0x" << std::hex << value;
    return {ss.str().c_str()};
}

String Felt::to_short_string() const {
    // Convert to bytes
    std::vector<uint8_t> bytes;
    export_bits(value, std::back_inserter(bytes), 8);
    // Cairo short strings are big-endian. export_bits is big-endian by default for most backends,
    // but let's ensure we handle the string construction correctly.

    String s;
    for (uint8_t b : bytes) {
        if (b != 0) { // Skip null bytes if any, though usually short strings don't have embedded nulls at start if treated as number
            s += String::chr(b);
        }
    }
    return s;
}

Ref<Felt> Felt::from_string(const String& p_value) {
    Ref<Felt> instance = memnew(Felt);
    instance->_init_from_string(p_value);
    return instance;
}

void Felt::_bind_methods() {
    ClassDB::bind_method(D_METHOD("to_string"), &Felt::to_string);
    ClassDB::bind_method(D_METHOD("to_short_string"), &Felt::to_short_string);
    ClassDB::bind_static_method("Felt", D_METHOD("from_string", "value"), &Felt::from_string);
}