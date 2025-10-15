//
// Created by hazel on 25/08/25.
//

#ifndef ENTITY_H
#define ENTITY_H

#include "resources/dojo_subscription.h"
#include "ref_counted/options/option_clause.h"
#include "variant/ty/dojo_array.h"

using namespace godot;

class EntitySubscription : public DojoSubscription
{
    GDCLASS(EntitySubscription, DojoSubscription);

    Ref<OptionClause> clause;
    TypedArray<String> world_addresses;

public:
    Type get_type() const override {
        return ENTITY;
    }

    Ref<OptionClause> get_clause() const {
        return clause;
    }

    void set_clause(const Ref<OptionClause>& p_clause) {
        clause = p_clause;
    }

    DOJO::COptionClause get_native_clause() const {
        if (clause.is_valid()) {
            return clause->get_native_option();
        }
        return {DOJO::COptionClause_Tag::NoneClause, {}};
    }

    DOJO::CArrayFieldElement get_native_world_addresses() const {
        return DojoArray::CFieldElementArrayHelper(world_addresses).c_array;
    }

    void set_world_addresses(const TypedArray<String>& p_addresses){ world_addresses = p_addresses; }
    TypedArray<String> get_world_addresses() const { return world_addresses; }


protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_clause"), &EntitySubscription::get_clause);
        ClassDB::bind_method(D_METHOD("set_clause", "p_clause"), &EntitySubscription::set_clause);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "clause", PROPERTY_HINT_RESOURCE_TYPE, "OptionClause"), "set_clause", "get_clause");

        ClassDB::bind_method(D_METHOD("get_world_addresses"), &EntitySubscription::get_world_addresses);
        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_addresses"), &EntitySubscription::set_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "world_addresses"), "set_world_addresses", "get_world_addresses");
    }

};

#endif //ENTITY_H
