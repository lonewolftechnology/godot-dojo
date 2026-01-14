#pragma once

#include "dojo/dojo.hpp"
#include "godot_cpp/classes/object.hpp"

using namespace godot;

class CallbackUtils : public Object {
    GDCLASS(CallbackUtils, Object)

public:
    static Dictionary entity_to_dict(const dojo::Entity& entity);
    static Dictionary event_to_dict(const dojo::Event& event);
    static Dictionary token_balance_to_dict(const dojo::TokenBalance& balance);
    static Dictionary token_to_dict(const dojo::Token& token);
    static Dictionary transaction_to_dict(const dojo::Transaction& transaction);

    // Helpers para tipos anidados
    static Variant ty_to_variant(const std::shared_ptr<dojo::Ty>& ty);
    static Variant primitive_to_variant(const std::shared_ptr<dojo::Primitive>& primitive);
    static Dictionary struct_to_dict(const std::shared_ptr<dojo::Struct>& struct_ptr);
    static Dictionary enum_to_dict(const std::shared_ptr<dojo::EnumType>& enum_ptr);

protected:
    static void _bind_methods() {}
};