//
// Created by hazel on 1/07/25.
//

#ifndef DOJO_HELPER_H
#define DOJO_HELPER_H

#include <godot_cpp/classes/object.hpp>

using namespace godot;

class DojoHelpers : public Object
{
    GDCLASS(DojoHelpers, Object)

protected:
    static void _bind_methods();
    static DojoHelpers* singleton;

public:
    DojoHelpers();
    ~DojoHelpers();

    static DojoHelpers* get_singleton() { return singleton; };
    static String get_katana_url();
    static Variant get_setting(const String& setting);

    static int64_t float_to_fixed(const double& value, const int& precision = 24);
    static double fixed_to_float(const int& value, const int& precision = 24);

    static int64_t float_to_fixed_64(const float& value);
    static double fixed_to_float_64(const int& value);

    static int64_t float_to_fixed_128(const float& value);
    static double fixed_to_float_128(const int& value);

    static int64_t float_to_fixed_256(const float& value);
    static double fixed_to_float_256(const int& value);

    // The idea is to turn types to string array, as that what dojo expects for structs
    // For example, in dojo_starter there is a struct called Vec2 which has two members x and y.
    // So to send a Vector2 from Godot/GDScript it should be an array of two members, x and y.
    static TypedArray<String> convert_to_string_array(const Variant& var);
};


#endif //DOJO_HELPER_H
