//
// Created by hazel on 5/06/25.
//

#include "classes/event_subscription.h"
#include "classes/torii_client.h"

#include "variant/field_element.h"
#include "variant/ty/primitive.h"
#include "tools/logger.h"
#include "variant/ty/dojo_array.h"
#include "variant/ty/ty.h"

Ref<EventSubscription> EventSubscription::g_active_instance = nullptr;
EventSubscription::EventSubscription()
{
}

EventSubscription::~EventSubscription()
{
    if (g_active_instance.is_valid() && g_active_instance.ptr() == this)
    {
        g_active_instance.unref();
        Logger::info("Active instance cleared");
    }
}

Callable EventSubscription::get_callback() const
{
    return callback;
}

void EventSubscription::set_callback(const Callable& p_callback)
{
    callback = p_callback;
    Logger::info("Callback ", name," set successfully");
}

EventSubscription* EventSubscription::get_active_instance()
{
    if (g_active_instance.is_valid())
    {
        return g_active_instance.ptr();
    }
    return nullptr;
}

void EventSubscription::set_active_instance(EventSubscription* instance)
{
    if (instance != nullptr)
    {
        g_active_instance = Ref<EventSubscription>(instance);
        Logger::info("Active instance set successfully");
    }
    else
    {
        g_active_instance.unref();
        Logger::info("Active instance cleared");
    }
}

void subscription_wrapper(DOJO::FieldElement entity_id, DOJO::CArrayStruct models)
{
    EventSubscription* instance = EventSubscription::get_active_instance();
    if (instance != nullptr)
    {
        Logger::info("Processing event - instance ", instance->get_name(), " found");
        instance->on_event_update(&entity_id, models);
    }
    else
    {
        Logger::error("Subscription instance is null - no active subscription found");
    }
}

bool EventSubscription::setup(ToriiClient* torii, const DOJO::COptionClause& event_clause, const Callable& p_callback)
{
    if (torii == nullptr)
    {
        Logger::error("ToriiClient not found");
        return false;
    }

    DOJO::ToriiClient* client = torii->get_client();
    if (client == nullptr)
    {
        Logger::error("ToriiClient->get_client() returned null");
        return false;
    }

    callback = p_callback;
    set_active_instance(this);
    DOJO::ResultSubscription resSubscription = DOJO::client_on_entity_state_update(
        client, event_clause, subscription_wrapper
    );

    if (resSubscription.tag == DOJO::ErrSubscription)
    {
        Logger::error("Subscription: ", GET_DOJO_ERROR(resSubscription));
        torii->emit_signal("subscription_error", GET_DOJO_ERROR(resSubscription));
        set_active_instance(nullptr);
        return false;
    }

    subscription = resSubscription.ok;
    Logger::success("Subscription created successfully");
    return true;
}

void EventSubscription::on_event_update(DOJO::FieldElement* entity_id, DOJO::CArrayStruct models) const
{
    Logger::custom_color("red",get_name(), "Update Event Received");

    if (callback.is_null())
    {
        Logger::error("NULL CALLABLE RECEIVED");
        return;
    }
    if (!callback.is_valid())
    {
        Logger::error("INVALID CALLABLE RECEIVED");
        return;
    }

    FieldElement entity_felt = {entity_id};
    const String entity_id_string = entity_felt.to_string();

    if (entity_id_string == FieldElement::nulled_as_string())
    {
        Logger::warning("Entity ID is 0, first call?");
        return;
    }
    Logger::info("Entity ID: ", entity_id_string);

    if (models.data == nullptr)
    {
        Logger::error("models.data in event is null");
        return;
    }

    Logger::info("model name ", models.data->name);
    const DOJO::CArrayMember children = models.data->children;

    if (children.data == nullptr)
    {
        Logger::error("children.data is null");
        return;
    }

    const ArrayDojo members = {children};
    Array arguments = members.get_value();

    for (int i = 0; i < arguments.size(); i++)
    {
        const Variant& arg = arguments[i];
        Logger::debug("ARG: ", arg);
    }
    Dictionary result = {};
    result["entity_id"] = entity_id_string;
    result["data"] = arguments;
    const Variant callback_result = callback.call(result);
    Logger::info("Event Callback", callback_result);
}
