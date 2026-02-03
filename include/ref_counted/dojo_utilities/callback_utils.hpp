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
    static Dictionary token_contract_to_dict(const dojo::TokenContract& contract);
    static Dictionary token_transfer_to_dict(const dojo::TokenTransfer& transfer);
    static Dictionary transaction_to_dict(const dojo::Transaction& transaction);
    static Dictionary contract_to_dict(const dojo::Contract& contract);
    static Dictionary controller_to_dict(const dojo::Controller& controller);
    static Dictionary achievement_to_dict(const dojo::Achievement& achievement);
    static Dictionary player_achievement_entry_to_dict(const dojo::PlayerAchievementEntry& entry);
    static Dictionary activity_to_dict(const dojo::Activity& activity);
    static Dictionary aggregation_entry_to_dict(const dojo::AggregationEntry& entry);
    static Dictionary table_search_results_to_dict(const dojo::TableSearchResults& results);

    // Helpers para tipos anidados
    static Variant ty_to_variant(const std::shared_ptr<dojo::Ty>& ty);
    static Variant primitive_to_variant(const std::shared_ptr<dojo::Primitive>& primitive);
    static Dictionary struct_to_dict(const std::shared_ptr<dojo::Struct>& struct_ptr);
    static Dictionary enum_to_dict(const std::shared_ptr<dojo::EnumType>& enum_ptr);
    static Dictionary sql_value_to_dict(const dojo::SqlValue& value);

protected:
    static void _bind_methods() {}
};