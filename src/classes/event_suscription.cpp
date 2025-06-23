//
// Created by hazel on 5/06/25.
//

#include "classes/event_subscription.h"
#include "classes/torii_client.h"

#include "variant/field_element.h"
#include "variant/primitive.h"
#include "debug_macros.h"

Ref<EventSubscription> EventSubscription::g_active_instance = nullptr;

void EventSubscription::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_callback"), &EventSubscription::get_callback);
    ClassDB::bind_method(D_METHOD("set_callback", "callback"), &EventSubscription::set_callback);
}

EventSubscription::EventSubscription()
{
    LOG_INFO("EventSubscription constructor called");
}

EventSubscription::~EventSubscription()
{
    LOG_INFO("EventSubscription destructor called");
    if (g_active_instance.is_valid() && g_active_instance.ptr() == this)
    {
        g_active_instance.unref();
        LOG_INFO("Active instance cleared");
    }
}

Callable EventSubscription::get_callback() const
{
    return callback;
}

void EventSubscription::set_callback(const Callable& p_callback)
{
    callback = p_callback;
    LOG_INFO("Callback set successfully");
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
        LOG_INFO("Active instance set successfully");
    }
    else
    {
        g_active_instance.unref();
        LOG_INFO("Active instance cleared");
    }
}

void subscription_wrapper(DOJO::FieldElement entity_id, DOJO::CArrayStruct models)
{
    LOG_INFO("Event received in subscription_wrapper");

    EventSubscription* instance = EventSubscription::get_active_instance();
    if (instance != nullptr)
    {
        LOG_INFO("Processing event - instance found");
        instance->on_event_update(&entity_id, models);
    }
    else
    {
        LOG_ERROR("Subscription instance is null - no active subscription found");
    }
}

bool EventSubscription::setup(ToriiClient* torii, const DOJO::COptionClause& event_clause, const Callable& p_callback)
{
    LOG_INFO("Setting up EventSubscription");

    if (torii == nullptr)
    {
        LOG_ERROR("ToriiClient is null");
        return false;
    }

    DOJO::ToriiClient* client = torii->get_client();
    if (client == nullptr)
    {
        LOG_ERROR("ToriiClient->get_client() returned null");
        return false;
    }
    LOG_INFO("Setting up EventSubscription - client found");
    callback = p_callback;
    LOG_INFO("Setting up EventSubscription - callback set");
    set_active_instance(this);
    LOG_INFO("Setting up EventSubscription - active instance set");
    LOG_INFO("Calling client_on_entity_state_update");
    DOJO::ResultSubscription resSubscription = DOJO::client_on_entity_state_update(
        client, event_clause, subscription_wrapper
    );

    if (resSubscription.tag == DOJO::ErrSubscription)
    {
        LOG_ERROR("Error al crear suscripción de entidades: ", GET_DOJO_ERROR(resSubscription));
        torii->emit_signal("subscription_error", GET_DOJO_ERROR(resSubscription));
        set_active_instance(nullptr);
        return false;
    }

    subscription = resSubscription.ok;
    LOG_SUCCESS("Suscripción de entidades creada exitosamente");
    return true;
}

void EventSubscription::on_event_update(DOJO::FieldElement* entity_id, DOJO::CArrayStruct models) const
{
    LOG_INFO("Entity Update Event Received");

    if (callback.is_null())
    {
        LOG_ERROR("NULL CALLABLE RECEIVED");
        return;
    }
    if (!callback.is_valid())
    {
        LOG_ERROR("INVALID CALLABLE RECEIVED");
        return;
    }

    FieldElement entity_felt = {entity_id};
    LOG_INFO("Entity ID: ", entity_felt.to_string());
    Array arguments = {};
    FieldElement nulledFelt = {0};

    if (entity_felt.as_packed_array().hex_encode() == nulledFelt.as_packed_array().hex_encode())
    {
        LOG_WARNING("Entity ID is 0, first call?");
        return;
    }

    if (models.data == nullptr)
    {
        LOG_ERROR("models.data is null");
        return;
    }

    LOG_INFO("[color=RED]START EVENT SUBSCRIPTION[/color]");
    LOG_INFO("model name ", models.data->name);
    auto children = models.data->children;

    if (children.data == nullptr)
    {
        LOG_ERROR("children.data is null");
        return;
    }

    std::vector<DOJO::Member> members(children.data, children.data + children.data_len);

    for (const auto& member : members)
    {
        LOG_INFO("[color=MAGENTA]Procesando member... [/color]");
        LOG_INFO("member.name: %s", member.name);

        if (member.ty->tag == DOJO::Ty_Tag::Primitive_)
        {
            LOG_INFO("member_type is [color=YELLOW]Primitive[/color]");
            DOJO::Primitive primitive = member.ty->primitive;
            if (primitive.tag == DOJO::Primitive_Tag::Felt252)
            {
                LOG_INFO("primitive.tag is [color=YELLOW]Felt252[/color]");
                if (String(member.name) == "player")
                {
                    LOG_DEBUG("Player");
                    FieldElement felt = {&primitive.felt252};
                    arguments.append(felt.to_string());
                    felt.bytearray_deserialize();
                }
            }
            DojoPrimitive _primitive = DojoPrimitive(primitive);
        }
        else if (member.ty->tag == DOJO::Ty_Tag::Struct_)
        {
            LOG_INFO("member_type is [color=YELLOW]Struct[/color]");
            DOJO::Struct struct_ = member.ty->struct_;
            LOG_INFO("[color=Peru]struct_name[/color] [color=YELLOW]", struct_.name, "[/color]");
            std::vector<DOJO::Member> struct_child(struct_.children.data,
                                                   struct_.children.data + struct_.children.data_len);
            String member_name = member.name;
            LOG_DEBUG(member_name);
            if (member_name == "vec")
            {
                Vector2 vec2 = {0, 0};
                for (const auto& struct_child_member : struct_child)
                {
                    LOG_INFO("struct_child_member.name: ", struct_child_member.name);

                    if (struct_child_member.ty->tag == DOJO::Ty_Tag::Primitive_)
                    {
                        DojoPrimitive s_value = {struct_child_member.ty->primitive};
                        LOG_DEBUG(struct_child_member.name, " | ", s_value.get_value());
                        real_t s_value_converted = s_value.get_value();
                        if (String(struct_child_member.name) == "x")
                        {
                            LOG_WARNING("UPDATING X");
                            vec2.x = s_value_converted;
                        }
                        else if (String(struct_child_member.name) == "y")
                        {
                            LOG_WARNING("UPDATING Y");
                            vec2.y = s_value_converted;
                        }
                    }
                }
                LOG_SUCCESS("[color=MAGENTA]NEW VECTOR2 [/color]", vec2);
                arguments.append(vec2);
            }
        }
        else if (member.ty->tag == DOJO::Ty_Tag::Array_)
        {
            LOG_INFO("member_type is [color=YELLOW]CArrayTy[/color]");
        }
        else if (member.ty->tag == DOJO::Ty_Tag::ByteArray)
        {
            LOG_INFO("member_type is [color=YELLOW]ByteArray[/color]");
        }
        else if (member.ty->tag == DOJO::Ty_Tag::Enum_)
        {
            LOG_INFO("member_type is [color=YELLOW]Enum[/color]");
            DOJO::Enum enum_ = member.ty->enum_;
        }
        else if (member.ty->tag == DOJO::Ty_Tag::Tuple_)
        {
            LOG_INFO("member_type is [color=YELLOW]Tuple[/color]");
        }
    }

    for (int i = 0; i < arguments.size(); i++)
    {
        Variant arg = arguments[i];
        LOG_DEBUG("ARG: ", arg);
    }

    LOG_INFO("Calling callback with arguments");
    Variant callback_result = callback.call(Variant(arguments));
    LOG_INFO("RESULT: ", callback_result);
}
