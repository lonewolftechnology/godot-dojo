//
// Created by hazel on 5/06/25.
//

#include <classes/event_subscription.h>

#include "variant/field_element.h"
#include <variant/primitive.h>

void EventSubscription::_bind_methods()
{
    // ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "callback", PROPERTY_HINT_RESOURCE_TYPE, "Callable"), "set_callback", "get_callback");
}

EventSubscription::EventSubscription()
{
}

EventSubscription::~EventSubscription()
{
}

Callable EventSubscription::get_callback() const
{
    return callback;
}

void EventSubscription::set_callback(const Callable& p_callback)
{
    callback = p_callback;
    LOG_INFO("Callback Received");
}

void EventSubscription::on_entity_update(dojo_bindings::FieldElement* entity_id,
                                         dojo_bindings::CArrayStruct models) const
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
    Array arguments = {};
    arguments.append(entity_felt.as_packed_array());
    FieldElement nulledFelt = {0};
    if (entity_felt.as_packed_array().hex_encode() == nulledFelt.as_packed_array().hex_encode())
    // if (&entity_id == &nullFelt)
    {
        LOG_WARNING("Entity ID is 0, first call?");
        // sleep(2);
        return;
    }

    // if (models.data == nullptr)
    // {
    //     LOG_ERROR("models.data is null");
    //     return;
    // }
    //
    // if (models.data->children.data == nullptr)
    // {
    //     LOG_ERROR("models.data->children.data is null");
    //     return;
    // }

    LOG_INFO("[color=RED]START EVENT SUBSCRIPTION[/color]");
    // Accede a models.data->children.data->ty de forma segura
    LOG_INFO("model name ", models.data->name);
    auto children = models.data->children;
    //std::cout << "children.data_len: " << children.data_len << std::endl;
    // std::cout << "children.data: " << children.data << std::endl;

    // Convertir `CArrayMember` a `std::vector`
    std::vector<dojo_bindings::Member> members(children.data, children.data + children.data_len);

    // Iterar y procesar los elementos
    for (const auto& member : members)
    {
        // Accede al miembro que necesites dependiendo de la estructura de `Member`.
        LOG_INFO("[color=MAGENTA]Procesando member... [/color]");
        LOG_INFO("member type: %s", typeid(member).name());
        LOG_INFO("member.name: %s", member.name);

        if (member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
        {
            LOG_INFO("member_type is [color=YELLOW]Primitive[/color]");
            dojo_bindings::Primitive primitive = member.ty->primitive;
            if (primitive.tag == dojo_bindings::Primitive_Tag::Felt252)
            {
                LOG_INFO("primitive.tag is [color=YELLOW]Felt252[/color]");
                FieldElement felt = {&primitive.felt252};
                // LOG_INFO("USERNAME ", dojo_bindings::controller_username(felt));

                LOG_DEBUG(felt.to_string());
                felt.bytearray_deserialize();
            }
            DojoPrimitive _primitive = DojoPrimitive(primitive);
            // result.append(_primitive.get_value());
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Struct_)
        {
            LOG_INFO("member_type is [color=YELLOW]Struct[/color]");
            dojo_bindings::Struct struct_ = member.ty->struct_;
            LOG_INFO("[color=Peru]struct_name[/color] [color=YELLOW]", struct_.name, "[/color]");
            std::vector<dojo_bindings::Member> struct_child(struct_.children.data,
                                                            struct_.children.data + struct_.children.data_len);
            String member_name = member.name;
            LOG_DEBUG(member_name);
            if (member_name == "vec")
            {
                Vector2 vec2 = {0, 0};
                for (const auto& struct_child_member : struct_child)
                {
                    LOG_INFO("struct_child_member.name: ", struct_child_member.name);

                    if (struct_child_member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
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
                        // LOG_DEBUG(struct_child_member.name, s_value.get_value());
                    }
                }
                LOG_SUCCESS("[color=MAGENTA]NEW VECTOR2 [/color]", vec2);

                arguments.append(vec2);
            }
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Array_)
        {
            LOG_INFO("member_type is [color=YELLOW]CArrayTy[/color]");
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::ByteArray)
        {
            LOG_INFO("member_type is [color=YELLOW]ByteArray[/color]");
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Enum_)
        {
            LOG_INFO("member_type is [color=YELLOW]Enum[/color]");
            dojo_bindings::Enum enum_ = member.ty->enum_;
            // LOG_INFO("enum_name [color=YELLOW]", enum_.name, "[/color]");
            // LOG_INFO("enum_option [color=YELLOW]", enum_.option, "[/color]");
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Tuple_)
        {
            LOG_INFO("member_type is [color=YELLOW]Tuple[/color]");
        }
    }
    // arguments.push_back(models); // Si también quieres pasar models
    for (int i = 0; i < arguments.size(); i++)
    {
        Variant arg = arguments[i];
        LOG_DEBUG("ARG: ", arg);
    }
    Variant callback_result = callback.call(Variant(arguments));
    LOG_INFO("RESULT: ", callback_result);
};
