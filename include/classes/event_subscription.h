//
// Created by hazel on 5/06/25.
//

#ifndef EVENT_SUBSCRIPTION_H
#define EVENT_SUBSCRIPTION_H

#include "dojo_types.h"
#include "torii_client.h"
#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;

class EventSubscription : public RefCounted
{
    GDCLASS(EventSubscription, RefCounted);

    Callable callback;
    DOJO::Subscription* subscription = nullptr;

    static Ref<EventSubscription> g_active_instance;

protected:
    static void _bind_methods();

public:
    EventSubscription();
    ~EventSubscription();

    Callable get_callback() const;
    void set_callback(const Callable& p_callback);
    bool setup(ToriiClient* torii, const dojo_bindings::COptionClause& event_clause, const Callable& p_callback);

    void on_event_update(DOJO::FieldElement* entity_id, DOJO::CArrayStruct models) const;
    void set_subscription(DOJO::Subscription* subscription) { this->subscription = subscription; }

    static EventSubscription* get_active_instance();
    static void set_active_instance(EventSubscription* instance);
};


#endif //EVENT_SUBSCRIPTION_H
