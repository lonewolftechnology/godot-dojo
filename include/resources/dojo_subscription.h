#ifndef DOJO_SUBSCRIPTION_H
#define DOJO_SUBSCRIPTION_H

#include "dojo_types.h"
#include "godot_cpp/classes/resource.hpp"
#include "tools/logger.h"

using namespace godot;

class DojoSubscription : public Resource {
    GDCLASS(DojoSubscription, Resource);

    Callable callback;
    DOJO::Subscription* subscription = nullptr;
    String name;

public:
    enum Type {
        EVENT,
        MESSAGE,
        ENTITY,
        TOKEN,
        TOKEN_BALANCE,
        TOKEN_TRANSFER,
        TRANSACTION,
        STARKNET,
        CONTRACT,
        AGGREGATION,
        ACTIVITY,
        ACHIEVEMENT_PROGRESSION,
        NIL
    };

    virtual ~DojoSubscription() {
        cancel();
    }

    void cancel() {
        if (subscription) {
            Logger::info("Subscription [color=Green]",name, "[/color] canceled");
            DOJO::subscription_cancel(subscription);
            subscription = nullptr;
        }
    }

    Callable get_callback() const {
        return callback;
    }

    void set_callback(const Callable& p_callback) {
        callback = p_callback;
    }

    void update_callback(const Callable& p_callback) {
        if (!callback.is_valid())
        {
            Logger::debug_extra(get_name(), "Invalid or null callback, ignoring callback update");
        }
        else
        {
            Logger::debug_extra(get_name(), "Updated Callback");
            set_callback(callback);

        }
    }

    String get_name() const
    {
        if (name.is_empty())
        {
            return get_class();
        }else
        {
            return name;
        }
    }
    void set_name(const String& p_name) { name = p_name; }

    DOJO::Subscription* get_subscription() const { return subscription; }
    void set_subscription(DOJO::Subscription* p_subscription) { this->subscription = p_subscription; }

    virtual Type get_type() const { return NIL; }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_callback", "p_callback"), &DojoSubscription::set_callback);
        ClassDB::bind_method(D_METHOD("get_callback"), &DojoSubscription::get_callback);
        ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "callback"), "set_callback", "get_callback");

        ClassDB::bind_method(D_METHOD("set_name", "p_name"), &DojoSubscription::set_name);
        ClassDB::bind_method(D_METHOD("get_name"), &DojoSubscription::get_name);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");

        ClassDB::bind_method(D_METHOD("cancel"), &DojoSubscription::cancel);
        ClassDB::bind_method(D_METHOD("get_type"), &DojoSubscription::get_type);

        ADD_PROPERTY(PropertyInfo(
                         Variant::INT,
                         "type",
                         PROPERTY_HINT_ENUM,
                         "Event,Message,Entity,Token,TokenBalance,TokenTransfer,Transaction,Starknet,Contract,Aggregation,Activity,Achievement Progression,Nil",
                         PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_READ_ONLY
                     ),
                     "", // No setter
                     "get_type"
        );

        BIND_ENUM_CONSTANT(EVENT);
        BIND_ENUM_CONSTANT(MESSAGE);
        BIND_ENUM_CONSTANT(ENTITY);
        BIND_ENUM_CONSTANT(TOKEN);
        BIND_ENUM_CONSTANT(TOKEN_BALANCE);
        BIND_ENUM_CONSTANT(TOKEN_TRANSFER);
        BIND_ENUM_CONSTANT(TRANSACTION);
        BIND_ENUM_CONSTANT(STARKNET);
        BIND_ENUM_CONSTANT(CONTRACT);
        BIND_ENUM_CONSTANT(AGGREGATION);
        BIND_ENUM_CONSTANT(ACTIVITY);
        BIND_ENUM_CONSTANT(ACHIEVEMENT_PROGRESSION);
        BIND_ENUM_CONSTANT(NIL);
    }
};

VARIANT_ENUM_CAST(DojoSubscription::Type);

#endif //DOJO_SUBSCRIPTION_H