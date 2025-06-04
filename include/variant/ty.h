//
// Created by hazel on 2/06/25.
//

#ifndef TY_H
#define TY_H
#include <godot_cpp/godot.hpp>
#include <dojo_types.h>

#include <classes/ref_counted.hpp>
#include <variant/variant.hpp>

using namespace godot;

class Ty : public RefCounted
{
    GDCLASS(Ty, RefCounted);
    friend class Variant;

private:
    Variant value = Variant::NIL;
    String type = "";

public:
    Ty();
    ~Ty() override;
    Ty(const dojo_bindings::Ty &ty);

    void set_value(const Variant &p_value) { value = p_value; }

    Variant get_value() const { return value; }

    String get_type() const { return type; }
    // void set_type(const String &p_type) { type = p_type; }



protected:
    static void _bind_methods();
};
#endif //TY_H
