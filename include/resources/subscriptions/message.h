//
// Created by hazel on 25/08/25.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include "resources/dojo_subscription.h"
#include "ref_counted/options/option_clause.h"

using namespace godot;

class MessageSubscription : public DojoSubscription
{
    GDCLASS(MessageSubscription, DojoSubscription);

    Ref<DojoOptionClause> clause;
    TypedArray<String> world_addresses;

public:
    Type get_type() const override {
        return MESSAGE;
    }

    Ref<DojoOptionChar> get_clause() const {
        return clause;
    }

    void set_clause(const Ref<DojoOptionChar>& p_clause) {
        clause = p_clause;
    }

    DOJO::COptionClause get_native_clause() const {
        if (clause.is_valid()) {
            return clause->get_native_option();
        }
        return {DOJO::COptionClause_Tag::NoneClause, {}};
    }
    DOJO::CArrayFieldElement get_native_world_addresses() const {
        if (world_addresses.is_empty()) {
            Logger::debug_extra("Message Subscription", "Empty world addresses, fetching ProjectSettings");
            TypedArray<String> setting = DojoHelpers::get_torii_setting("worlds");
            if (setting.is_empty()) {
                Logger::error("World addresses not found in Project Settings");
                static DOJO::FieldElement dummy; // A static dummy pointer.
                return {&dummy, 0};
            }
            return DojoArray::CFieldElementArrayHelper(setting).c_array;
        }
        return DojoArray::CFieldElementArrayHelper(world_addresses).c_array;
    }


    void set_world_addresses(const TypedArray<String>& p_addresses){ world_addresses = p_addresses; }
    TypedArray<String> get_world_addresses() const { return world_addresses; }

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_clause"), &MessageSubscription::get_clause);
        ClassDB::bind_method(D_METHOD("set_clause", "p_clause"), &MessageSubscription::set_clause);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "clause", PROPERTY_HINT_RESOURCE_TYPE, "DojoOptionChar"), "set_clause", "get_clause");

        ClassDB::bind_method(D_METHOD("get_world_addresses"), &MessageSubscription::get_world_addresses);
        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_addresses"), &MessageSubscription::set_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "world_addresses"), "set_world_addresses", "get_world_addresses");
    }

};

#endif //MESSAGE_H
