//
// Created by hazel on 1/07/25.
//

#ifndef DOJO_HELPER_H
#define DOJO_HELPER_H

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class DojoHelpers : public Object {
    GDCLASS(DojoHelpers, Object)

protected:
    static void _bind_methods();

public:
    DojoHelpers();
    ~DojoHelpers();

    void set_contracts_addresses(const Dictionary &addresses);
    void execute_from_outside(const Dictionary &account, const String &to, const String &selector, const String &calldata_parameter);
    void callback_proxy(const Variant &key, const Array &models);

    template<typename T>
    static void convert_ty(const Variant &member, const String &expected_name, const String &expected_type, T &output);
};

class TypeConverter {
public:
    static String to_string(const Variant &member);
    static int to_int(const Variant &member);
    static int64_t to_long(const Variant &member);
    static bool to_bool(const Variant &member);

    template<typename T>
    static Array to_array(const Variant &member);

    static int to_direction(const Variant &member);
    static Vector2 to_vec2(const Variant &member);
};

template<typename T>
Array convert_to_felt_hexa(const T &value, const String &value_type);

template<typename T>
void convert_ty(const Variant &member, const String &expected_name, const String &expected_type, T &output);
#endif //DOJO_HELPER_H
