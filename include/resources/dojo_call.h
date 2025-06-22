//
// Created by hazel on 21/06/25.
//

#ifndef DOJO_ACTION_H
#define DOJO_ACTION_H
#include "godot_cpp/classes/resource.hpp"
#include "variant/field_element.h"

using namespace godot;

class DojoCall : public Resource
{
    GDCLASS(DojoCall, Resource);

    String to;
    String selector;
    Array calldata;

public:
    DojoCall()
    {
        to = "";
        selector = "";
        calldata = {};
    };
    ~DojoCall(){};

    String get_to() const{return to;};
    void set_to(const String& p_to)
    {
        to = p_to;
    };

    String get_selector() const{return selector;};
    void set_selector(const String& p_selector)
    {
        selector = p_selector;
        selector_buffer = CharString();
            emit_changed();
    };

    Array get_calldata() const{return calldata;};
    void set_calldata(const Array& p_calldata)
    {
        calldata = p_calldata;
        selector_buffer = CharString();
        emit_changed();
    };

    CharString get_selector_buffer() const { return selector_buffer; }
    CharString get_to_buffer() const { return to_buffer; }

    DOJO::Call build()
    {
        DOJO::Call call = {};
        call.to = FieldElement::from_string(to);
        call.selector = selector.utf8().get_data();
        DOJO::CArrayFieldElement felt_array = {};
        if (calldata.size() == 0)
        {
            felt_array.data = nullptr;
            felt_array.data_len = 0;
        }
        felt_array.data_len = calldata.size();
        call.calldata = felt_array;
        return call;
    }

protected:
    mutable CharString to_buffer;
    mutable CharString selector_buffer;

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
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "calldata"), "set_calldata", "get_calldata");
    };
};

#endif //DOJO_ACTION_H
