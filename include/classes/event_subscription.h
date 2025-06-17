//
// Created by hazel on 5/06/25.
//

#ifndef EVENT_SUBSCRIPTION_H
#define EVENT_SUBSCRIPTION_H

#include "debug_macros.h"
#include "dojo_types.hpp"
#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;

    class EventSubscription : public  RefCounted
    {
        GDCLASS(EventSubscription, RefCounted);

    private:
        Callable callback;
        DOJO::Subscription* subscription = nullptr;


    protected:
        static void _bind_methods();

    public:
        EventSubscription();
        ~EventSubscription();

        Callable get_callback() const;
        void set_callback(const Callable& p_callback);

        void on_entity_update(DOJO::FieldElement* entity_id, DOJO::CArray<DOJO::Struct> models) const;
        void set_subscription(DOJO::Subscription* subscription) { this->subscription = subscription; }
    };


#endif //EVENT_SUBSCRIPTION_H
