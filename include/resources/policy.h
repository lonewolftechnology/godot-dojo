//
// Created by hazel on 21/06/25.
//

#ifndef POLICY_H
#define POLICY_H
#include "godot_cpp/classes/resource.hpp"

using namespace godot;

class Policy : public Resource
{
    GDCLASS(Policy, Resource)

public:
    Policy()
    {
        target = "";
        method = "";
        description = "";
    };
    ~Policy(){};
    void set_target(const String& p_target){target = p_target;};
    String get_target() const {return target;};
    void set_method(const String& p_method){method = p_method;};
    String get_method() const {return method;};
    void set_description(const String& p_description){description = p_description;};
    String get_description() const {return description;};

protected:
    String target;
    String method;
    String description;

    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("set_target", "target"), &Policy::set_target);
        ClassDB::bind_method(D_METHOD("get_target"), &Policy::get_target);
        ClassDB::bind_method(D_METHOD("set_method", "method"), &Policy::set_method);
        ClassDB::bind_method(D_METHOD("get_method"), &Policy::get_method);
        ClassDB::bind_method(D_METHOD("set_description", "description"), &Policy::set_description);
        ClassDB::bind_method(D_METHOD("get_description"), &Policy::get_description);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "target"), "set_target", "get_target");
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "method"), "set_method", "get_method");
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
    };
};

#endif //POLICY_H
