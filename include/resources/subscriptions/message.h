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

    Ref<OptionClause> clause;

public:
    Type get_type() const override {
        return MESSAGE;
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


protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_clause"), &MessageSubscription::get_clause);
        ClassDB::bind_method(D_METHOD("set_clause", "p_clause"), &MessageSubscription::set_clause);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "clause", PROPERTY_HINT_RESOURCE_TYPE, "OptionClause"), "set_clause", "get_clause");
    }

};

#endif //MESSAGE_H
