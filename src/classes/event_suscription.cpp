//
// Created by hazel on 5/06/25.
//

#include <classes/event_subscription.h>

#include "variant/field_element.h"
#include <variant/primitive.h>
EventSubscription::EventSubscription()
{
}

EventSubscription::~EventSubscription()
{
}

void EventSubscription::on_entity_update(dojo_bindings::FieldElement* entity_id, dojo_bindings::CArrayStruct models)
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
    Array arguments;
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

    LOG_INFO("[color=RED]START[/color]");
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
        LOG_INFO("[color=PERU]Procesando member... [/color]");
        LOG_INFO("member type: %s", typeid(member).name());
        LOG_INFO("member.name: %s", member.name);

        if (member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
        {
            LOG_INFO("member_type is [color=YELLOW]Primitive[/color]");
            dojo_bindings::Primitive primitive = member.ty->primitive;
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
            Dictionary vec2 = {};
            for (const auto& struct_child_member : struct_child)
            {
                LOG_INFO("struct_child_member.name: ", struct_child_member.name);

                if (struct_child_member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
                {
                    DojoPrimitive s_value = {struct_child_member.ty->primitive};
                    vec2[struct_child_member.name] = s_value.get_value();
                    LOG_INFO(struct_child_member.name, s_value.get_value());
                }


            }
            arguments.append(vec2);
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
    Variant callback_result = callback.call(arguments);
    LOG_INFO("RESULT: ", callback_result);

};