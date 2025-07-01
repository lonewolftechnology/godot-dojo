#include "temp/dojo_helper.h"
#include <godot_cpp/classes/engine.hpp>

#include "tools/logger.h"

void DojoHelpers::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_contracts_addresses", "addresses"), &DojoHelpers::set_contracts_addresses);
    ClassDB::bind_method(D_METHOD("callback_proxy", "key", "models"), &DojoHelpers::callback_proxy);
}

DojoHelpers::DojoHelpers() {}
DojoHelpers::~DojoHelpers() {}

void DojoHelpers::set_contracts_addresses(const Dictionary &addresses) {
    Logger::info("Llamada a set_contracts_addresses: ", addresses);
}
void DojoHelpers::execute_from_outside(const Dictionary &account, const String &to, const String &selector, const String &calldata_parameter) {
    Logger::info("Llamada a execute_from_outside: to=", to, " selector=", selector, " params=", calldata_parameter, " cuenta=", account);
}

void DojoHelpers::callback_proxy(const Variant &key, const Array &models) {
    Logger::info("Llamada a callback_proxy, key: ", key, ", models len: ", models.size());
}

// ------------------- TypeConverter -----------------------

String TypeConverter::to_string(const Variant &member) {
    if (member.get_type() == Variant::STRING)
        return member;
    return String("[No String]");
}

int TypeConverter::to_int(const Variant &member) {
    if (member.get_type() == Variant::INT)
        return member;
    return 0;
}

int64_t TypeConverter::to_long(const Variant &member) {
    if (member.get_type() == Variant::INT)
        return static_cast<int64_t>(int(member));
    if (member.get_type() == Variant::FLOAT)
        return static_cast<int64_t>(double(member));
    return 0;
}

bool TypeConverter::to_bool(const Variant &member) {
    if (member.get_type() == Variant::BOOL)
        return member;
    return false;
}

int TypeConverter::to_direction(const Variant &member) {
    // Adaptar si tienes un enum de dirección real.
    if (member.get_type() == Variant::INT)
        return member;
    return -1;
}

Vector2 TypeConverter::to_vec2(const Variant &member) {
    if (member.get_type() == Variant::VECTOR2)
        return member;
    return Vector2();
}

template<typename T>
Array TypeConverter::to_array(const Variant &member) {
    Array arr;
    if (member.get_type() == Variant::ARRAY) {
        arr = member;
    }
    // DojoArray
    return arr;
}

// ------------------- Funciones template ------------------

template<typename T>
Array convert_to_felt_hexa(const T &value, const String &value_type) {
    Array arr;
    // wip tests DojoTy-Primitive-etc
    return arr;
}

template <typename T>
void convert_ty(const Variant& member, const String& expected_name, const String& expected_type, T& output)
{
    Logger::info("Llamada a convert_ty con expected_name: ", expected_name, " expected_type: ", expected_type);
    // tests
}
