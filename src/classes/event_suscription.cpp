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

void EventSubscription::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_callback"), &EventSubscription::get_callback);
    ClassDB::bind_method(D_METHOD("set_callback", "callback"), &EventSubscription::set_callback);
}

EventSubscription::EventSubscription()
{
    Logger::info("EventSubscription constructor called");
}

EventSubscription::~EventSubscription()
{
    Logger::info("EventSubscription destructor called");
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
    Logger::info("Callback set successfully");
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
    Logger::info("Event received in subscription_wrapper");

    EventSubscription* instance = EventSubscription::get_active_instance();
    if (instance != nullptr)
    {
        Logger::info("Processing event - instance found");
        instance->on_event_update(&entity_id, models);
    }
    else
    {
        Logger::error("Subscription instance is null - no active subscription found");
    }
}

bool EventSubscription::setup(ToriiClient* torii, const DOJO::COptionClause& event_clause, const Callable& p_callback)
{
    Logger::info("Setting up EventSubscription");

    if (torii == nullptr)
    {
        Logger::error("ToriiClient is null");
        return false;
    }

    DOJO::ToriiClient* client = torii->get_client();
    if (client == nullptr)
    {
        Logger::error("ToriiClient->get_client() returned null");
        return false;
    }
    Logger::info("Setting up EventSubscription - client found");
    callback = p_callback;
    Logger::info("Setting up EventSubscription - callback set");
    set_active_instance(this);
    Logger::info("Setting up EventSubscription - active instance set");
    Logger::info("Calling client_on_entity_state_update");
    DOJO::ResultSubscription resSubscription = DOJO::client_on_entity_state_update(
        client, event_clause, subscription_wrapper
    );

    if (resSubscription.tag == DOJO::ErrSubscription)
    {
        Logger::error("Error al crear suscripción de entidades: ", GET_DOJO_ERROR(resSubscription));
        torii->emit_signal("subscription_error", GET_DOJO_ERROR(resSubscription));
        set_active_instance(nullptr);
        return false;
    }

    subscription = resSubscription.ok;
    Logger::success("Suscripción de entidades creada exitosamente");
    return true;
}

void EventSubscription::on_event_update(DOJO::FieldElement* entity_id, DOJO::CArrayStruct models) const
{
    Logger::info("Update Event Received");

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
    Logger::info("Entity ID: ", entity_felt.to_string());

    if (entity_felt.as_packed_array().hex_encode() == FieldElement::nulled_as_string())
    {
        Logger::warning("Entity ID is 0, first call?");
        return;
    }

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

    Logger::info("Calling callback with arguments");
    const Variant callback_result = callback.call(arguments);
    Logger::info("Event Callback", callback_result);
}
