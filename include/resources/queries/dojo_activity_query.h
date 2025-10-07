#ifndef DOJO_ACTIVITY_QUERY_H
#define DOJO_ACTIVITY_QUERY_H

#include "resources/dojo_query_base.h"
#include "dojo_types.h"
#include "ref_counted/options/option_u64.h"
#include "variant/ty/dojo_array.h"
using namespace godot;

class DojoActivityQuery : public DojoQueryBase {
    GDCLASS(DojoActivityQuery, DojoQueryBase)
    TypedArray<String> world_addresses;
    TypedArray<String> namespaces;
    TypedArray<String> caller_addresses;
    Ref<OptionU64> from_time;
    Ref<OptionU64> to_time;

public:
    DojoActivityQuery() {}

    ~DojoActivityQuery() {}

    void* get_native_query() {
        auto* query = new DOJO::ActivityQuery();

        query->world_addresses = DojoArrayHelpers::string_array_to_native_carray_felt(world_addresses);
        query->namespaces = DojoArrayHelpers::string_array_to_native_carray_str(namespaces);
        query->caller_addresses = DojoArrayHelpers::string_array_to_native_carray_felt(caller_addresses);
        query->from_time = from_time->get_native_option();
        query->to_time = to_time->get_native_option();

        return query;
    }

    void set_world_addresses(const TypedArray<String> &p_array) { world_addresses = p_array; }
    void set_namespaces(const TypedArray<String> &p_array) { namespaces = p_array; }
    void set_caller_addresses(const TypedArray<String> &p_array) { caller_addresses = p_array; }
    void set_from_time(const Ref<OptionU64> &p_option) { from_time = p_option; }
    void set_to_time(const Ref<OptionU64> &p_option) { to_time = p_option; }
    TypedArray<String> get_world_addresses() { return world_addresses; }
    TypedArray<String> get_namespaces() { return namespaces; }
    TypedArray<String> get_caller_addresses() { return caller_addresses; }
    Ref<OptionU64> get_from_time() { return from_time; }
    Ref<OptionU64> get_to_time() { return to_time; }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_array"), &DojoActivityQuery::set_world_addresses);
        ClassDB::bind_method(D_METHOD("get_world_addresses"), &DojoActivityQuery::get_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "world_addresses", PROPERTY_HINT_ARRAY_TYPE, "String"),
                     "set_world_addresses", "get_world_addresses");

        ClassDB::bind_method(D_METHOD("set_namespaces", "p_array"), &DojoActivityQuery::set_namespaces);
        ClassDB::bind_method(D_METHOD("get_namespaces"), &DojoActivityQuery::get_namespaces);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "namespaces", PROPERTY_HINT_ARRAY_TYPE, "String"), "set_namespaces",
                     "get_namespaces");

        ClassDB::bind_method(D_METHOD("set_caller_addresses", "p_array"), &DojoActivityQuery::set_caller_addresses);
        ClassDB::bind_method(D_METHOD("get_caller_addresses"), &DojoActivityQuery::get_caller_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "caller_addresses", PROPERTY_HINT_ARRAY_TYPE, "String"),
                     "set_caller_addresses", "get_caller_addresses");

        ClassDB::bind_method(D_METHOD("set_from_time", "p_option"), &DojoActivityQuery::set_from_time);
        ClassDB::bind_method(D_METHOD("get_from_time"), &DojoActivityQuery::get_from_time);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "from_time", PROPERTY_HINT_RESOURCE_TYPE, "OptionU64"),
                     "set_from_time", "get_from_time");

        ClassDB::bind_method(D_METHOD("set_to_time", "p_option"), &DojoActivityQuery::set_to_time);
        ClassDB::bind_method(D_METHOD("get_to_time"), &DojoActivityQuery::get_to_time);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "to_time", PROPERTY_HINT_RESOURCE_TYPE, "OptionU64"), "set_to_time",
                     "get_to_time");
    }
};

#endif //DOJO_ACTIVITY_QUERY_H
