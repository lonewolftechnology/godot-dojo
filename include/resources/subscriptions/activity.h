#ifndef ACTIVITY_H
#define ACTIVITY_H
#include "resources/dojo_subscription.h"
#include "dojo_types.h"
#include "variant/ty/dojo_array.h"

using namespace godot;

class ActivitySubscription: public DojoSubscription {
    GDCLASS(ActivitySubscription, DojoSubscription)

    TypedArray<String> world_addresses;
    TypedArray<String> namespaces;
    TypedArray<String> caller_addresses;

public:
    ActivitySubscription(){}
    ~ActivitySubscription(){}

    Type get_type() const override {
        return ACTIVITY;
    }

    DOJO::CArrayc_char get_native_namespaces() const {
        return DojoArrayHelpers::string_array_to_native_carray_str(namespaces);
    }

    DOJO::CArrayFieldElement get_native_world_addresses() const {
        return DojoArrayHelpers::string_array_to_native_carray_felt(world_addresses);
    }

    DOJO::CArrayFieldElement get_native_caller_addresses() const {
        return DojoArrayHelpers::string_array_to_native_carray_felt(caller_addresses);
    }

    void set_world_addresses(const TypedArray<String>& p_array){ world_addresses = p_array; }
    TypedArray<String> get_world_addresses(){ return world_addresses; }

    void set_namespaces(const TypedArray<String>& p_array){ namespaces = p_array; }
    TypedArray<String> get_namespaces(){ return namespaces; }

    void set_caller_addresses(const TypedArray<String>& p_array){ caller_addresses = p_array; }
    TypedArray<String> get_caller_addresses(){ return caller_addresses; }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_array"), &ActivitySubscription::set_world_addresses);
        ClassDB::bind_method(D_METHOD("get_world_addresses"), &ActivitySubscription::get_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "world_addresses"), "set_world_addresses", "get_world_addresses");

        ClassDB::bind_method(D_METHOD("set_namespaces", "p_array"), &ActivitySubscription::set_namespaces);
        ClassDB::bind_method(D_METHOD("get_namespaces"), &ActivitySubscription::get_namespaces);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "namespaces"), "set_namespaces", "get_namespaces");

        ClassDB::bind_method(D_METHOD("set_caller_addresses", "p_array"), &ActivitySubscription::set_caller_addresses);
        ClassDB::bind_method(D_METHOD("get_caller_addresses"), &ActivitySubscription::get_caller_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "caller_addresses"), "set_caller_addresses", "get_caller_addresses");
    }
};
#endif //ACTIVITY_H