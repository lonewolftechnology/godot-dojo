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
    Array calldata;

    DOJO::FieldElement stored_to;
    std::string stored_selector;
    std::vector<DOJO::FieldElement> stored_calldata;
    DOJO::CArrayFieldElement c_array_calldata;

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

    void set_selector(const String& p_selector)
    {
        selector = p_selector;
        emit_changed();
    };

    Array get_calldata() const { return calldata; };

    void set_calldata(const Array& p_calldata)
    {
        calldata = p_calldata;
        emit_changed();
    };

    uintptr_t get_size()
    {
        LOG_DEBUG_EXTRA("Call", "calldata size: ", calldata.size());
        uintptr_t size = 1;
        if (calldata.size() > 0)
        {
            size = calldata.size();
        }
        LOG_DEBUG_EXTRA("Call", "final calldata size: ", size);
        return size;
    }

    DOJO::Call build()
    {
        DOJO::Call call = {};

        stored_to = FieldElement::from_string(to);
        stored_selector = selector.utf8().get_data();

        call.to = stored_to;
        call.selector = stored_selector.c_str();

        if (!calldata.is_empty())
        {
            LOG_DEBUG("Building Calldata");
            stored_calldata.clear();
            stored_calldata.reserve(calldata.size());

            for (int i = 0; i < calldata.size(); i++)
            {
                Ref<FieldElement> felt;
                felt.instantiate();
                felt = calldata[i];

                LOG_DEBUG_EXTRA("Call Build", felt->to_string());
                if (felt.is_valid())
                {
                    stored_calldata.push_back(felt->get_felt_no_ptr());
                }
            }

            c_array_calldata.data = stored_calldata.data();
            c_array_calldata.data_len = stored_calldata.size();
            call.calldata = c_array_calldata;
        }
        else
        {
            LOG_DEBUG_EXTRA("Call Build", "no calldata found, ignoring");
            // call.calldata = {};
        }

        return call;
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
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "calldata"), "set_calldata", "get_calldata");
    };
};

#endif //DOJO_ACTION_H
