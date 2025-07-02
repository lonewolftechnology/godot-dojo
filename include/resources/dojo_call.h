//
// Created by hazel on 21/06/25.
//

#ifndef DOJO_ACTION_H
#define DOJO_ACTION_H
#include "tools/logger.h"
#include "godot_cpp/classes/resource.hpp"
#include "variant/field_element.h"

using namespace godot;

class DojoCall : public Resource
{
    GDCLASS(DojoCall, Resource);

    String to;
    String selector;
    PackedStringArray calldata;

public:
    DojoCall()
    {
    };

    ~DojoCall()
    {
    };

    String get_to() const { return to; };

    void set_to(const String& p_to)
    {
        to = p_to;
        emit_changed();
    };

    String get_selector() const { return selector; };
    const char* get_selector_ctr() const { return selector.utf8().get_data(); };

    void set_selector(const String& p_selector)
    {
        selector = p_selector;
        emit_changed();
    };

    PackedStringArray get_calldata() const { return calldata; };

    void set_calldata(const Array& p_calldata)
    {
        calldata = p_calldata;
        emit_changed();
    };

    uintptr_t get_size()
    {
        Logger::debug_extra("Call", "calldata size: ", calldata.size());
        return calldata.size();
    }

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_to"), &DojoCall::get_to);
        ClassDB::bind_method(D_METHOD("set_to", "to"), &DojoCall::set_to);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "to"), "set_to", "get_to");

        ClassDB::bind_method(D_METHOD("get_selector"), &DojoCall::get_selector);
        ClassDB::bind_method(D_METHOD("set_selector", "selector"), &DojoCall::set_selector);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "selector"), "set_selector", "get_selector");

        ClassDB::bind_method(D_METHOD("get_calldata"), &DojoCall::get_calldata);
        ClassDB::bind_method(D_METHOD("set_calldata", "calldata"), &DojoCall::set_calldata);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "calldata"), "set_calldata", "get_calldata");
    };
};

#endif //DOJO_ACTION_H
