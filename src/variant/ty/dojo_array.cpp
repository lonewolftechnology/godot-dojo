#include "variant/ty/dojo_array.h"
#include "variant/ty/struct.h"
#include "variant/ty/ty.h"
#include "ref_counted/options/option_clause.h"
#include "ref_counted/options/option_f64.h"
#include "ref_counted/options/option_u256.h"

DojoArray::DojoArray(DOJO::CArrayTy array)
{
    value = CArrayTyToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayToken array)
{
    value = CArrayTokenToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayEntity array)
{
    value = CArrayEntityToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayc_char array)
{
    value = CArrayc_charToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayClause array)
{
    value = CArrayClauseToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayWorld array)
{
    value = CArrayWorldToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayStruct array)
{
    value = CArrayStructToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayMember array)
{
    value = CArrayMemberToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayOrderBy array)
{
    value = CArrayOrderByToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayEnumOption array)
{
    value = CArrayEnumOptionToVariant(array);
}

DojoArray::DojoArray(DOJO::PageController array)
{
    value = PageControllerToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayMemberValue array)
{
    value = CArrayMemberValueToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayFieldElement array)
{
    value = CArrayFieldElementToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayTokenContract array)
{
    value = CArrayTokenContractToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayCOptionFieldElement array)
{
    value = CArrayCOptionFieldElementToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayModel array)
{
    value = CArrayModelToVariant(array);
}

DojoArray::DojoArray(dojo_bindings::CArrayActionCount array) {
    value = CArrayActionCountToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayActivity array)
{
    value = CArrayActivityToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayAggregationEntry array)
{
    value = CArrayAggregationEntryToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayAchievement array)
{
    value = CArrayAchievementToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayTokenTransfer array)
{
    value = CArrayTokenTransferToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayTokenBalance array)
{
    value = CArrayTokenBalanceToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayAchievementTask array)
{
    value = CArrayAchievementTaskToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayPlayerAchievementEntry array)
{
    value = CArrayPlayerAchievementEntryToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayPlayerAchievementProgress array)
{
    value = CArrayPlayerAchievementProgressToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayTaskProgress array)
{
    value = CArrayTaskProgressToVariant(array);
}

DojoArray::DojoArray(DOJO::CArrayTableSearchResults array) {
    value = CArrayTableSearchResultsToVariant(array);
}
DojoArray::DojoArray(DOJO::CArraySearchMatch array) {
    value = CArraySearchMatchToVariant(array);
}

DojoArray::DojoArray(dojo_bindings::CArraySearchMatchField array) {
    value = CArraySearchMatchFieldToVariant(array);
}

Variant DojoArray::CArrayTyToVariant(DOJO::CArrayTy array_ty)
{
    std::vector<DOJO::Ty> array_ty_vector(array_ty.data, array_ty.data + array_ty.data_len);
    Array result;

    for (const auto& array_ty_element : array_ty_vector)
    {
        Ref<DojoTy> dojo_ty = memnew(DojoTy(array_ty_element));
        Dictionary data;
        data[dojo_ty->get_name()] = dojo_ty->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayTokenToVariant(DOJO::CArrayToken array)
{
    Array result = {};
    std::vector<DOJO::Token> array_token_vector(array.data, array.data + array.data_len);
    for (const auto& token : array_token_vector)
    {
        Dictionary data;
        data["contract_address"] = FieldElement::get_as_string_no_ptr(token.contract_address);
        Ref<DojoOptionU256> token_id = memnew(DojoOptionU256(token.token_id));
        data["token_id"] = token_id->get_value();
        data["name"] = token.name;
        data["symbol"] = token.symbol;
        data["decimals"] = token.decimals;
        data["metadata"] = token.metadata;
        Ref<DojoOptionU256> total_supply = memnew(DojoOptionU256(token.total_supply));
        data["total_supply"] = total_supply->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayEntityToVariant(DOJO::CArrayEntity array)
{
    Array result = {};
    Logger::debug_extra("CArrayEntityToVariant", "array.data_len: ", array.data_len);
    std::vector array_entity_vector(array.data, array.data + array.data_len);

    for (const auto& entity : array_entity_vector)
    {
        Dictionary data;

        data["hashed_keys"] = FieldElement::get_as_string_no_ptr(entity.hashed_keys);
        data["models"] = CArrayStructToVariant(entity.models);

        result.append(data);
    }
    Logger::success_extra("CArrayEntityToVariant", result);
    return result;
}

Variant DojoArray::CArrayc_charToVariant(DOJO::CArrayc_char array)
{
    Array result_array;
    std::vector<const char*> strings(array.data, array.data + array.data_len);

    for (const auto* c_str : strings)
    {
        result_array.append(String(c_str));
    }
    return result_array;
}

Variant DojoArray::CArrayClauseToVariant(DOJO::CArrayClause array)
{
    Array result;
    std::vector array_clause_vector(array.data, array.data + array.data_len);

    for (const auto& clause : array_clause_vector)
    {
        Dictionary data;
        result.append(DojoOptionClause::from_native(clause));
    }
    return result;
}

Variant DojoArray::CArrayWorldToVariant(DOJO::CArrayWorld array)
{
    Array result;
    std::vector<DOJO::World> worlds(array.data, array.data + array.data_len);

    for (const auto& world : worlds)
    {
        Dictionary world_dict;
        world_dict["world_address"] = FieldElement::get_as_string_no_ptr(world.world_address);
        world_dict["models"] = CArrayModelToVariant(world.models);

        result.append(world_dict);
    }

    return result;
}

Variant DojoArray::CArrayStructToVariant(DOJO::CArrayStruct array)
{
    Array result;
    std::vector<DOJO::Struct> array_struct_vector(array.data, array.data + array.data_len);

    for (const auto& struct_ : array_struct_vector)
    {
        Dictionary data;
        Ref<DojoStruct> dojo_struct = memnew(DojoStruct(struct_));

        data[dojo_struct->get_name()] = dojo_struct->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayMemberToVariant(DOJO::CArrayMember array)
{
    Array result;
    std::vector<DOJO::Member> array_member_vector(array.data, array.data + array.data_len);

    for (const auto& member : array_member_vector)
    {
        Dictionary data;
        Ref<DojoTy> dojo_ty = memnew(DojoTy(member.ty));
        String data_name = dojo_ty->get_name();
        if (data_name.is_empty())
        {
            data_name = member.name;
        }
        data[data_name] = dojo_ty->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayOrderByToVariant(DOJO::CArrayOrderBy array)
{
    Array result;
    std::vector<DOJO::OrderBy> array_order_by_vector(array.data, array.data + array.data_len);

    for (const auto& orderby : array_order_by_vector)
    {
        Dictionary data;
        data["field"] = orderby.field;
        data["direction"] = static_cast<uint8_t>(orderby.direction);
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayEnumOptionToVariant(DOJO::CArrayEnumOption array)
{
    Array result;
    std::vector<DOJO::EnumOption> array_enum_option_vector(array.data, array.data + array.data_len);

    for (const auto& enum_option : array_enum_option_vector)
    {
        Dictionary data;
        data["name"] = enum_option.name;
        data["data"] = memnew(DojoTy(enum_option.ty))->get_value();
        result.append(data);
    }
    return result;
}

Variant DojoArray::PageControllerToVariant(const DOJO::PageController& page)
{
    TypedArray<Dictionary> result;
    DOJO::CArrayController array = page.items;
    std::vector<DOJO::Controller> array_controller_vector(array.data, array.data + array.data_len);

    for (const auto& controller : array_controller_vector)
    {
        Dictionary data;
        Ref<FieldElement> controller_addr = memnew(FieldElement(controller.address));
        data["address"] = controller_addr->to_string();
        data["felt_data"] = controller_addr->to_string();
        data["username"] = controller.username;
        data["deployed_at_timestamp"] = controller.deployed_at_timestamp;
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayMemberValueToVariant(DOJO::CArrayMemberValue array)
{
    Array result;
    std::vector<DOJO::MemberValue> array_member_value_vector(array.data, array.data + array.data_len);

    for (const auto& member_value : array_member_value_vector)
    {
        Variant data;
        if (member_value.tag == DOJO::List)
        {
            Ref<DojoArray> array_dojo = memnew(DojoArray(member_value.list));
            data = array_dojo->get_value();
        }
        if (member_value.tag == DOJO::PrimitiveValue)
        {
            Ref<DojoPrimitive> dojo_ty = memnew(DojoPrimitive(member_value.primitive_value));
            data = dojo_ty->get_value();
        }
        if (member_value.tag == DOJO::String)
        {
            data = member_value.string;
        }
        result.append(data);
    }
    return result;
}

Variant DojoArray::CArrayFieldElementToVariant(DOJO::CArrayFieldElement array)
{
    Array result_array;
    std::vector<DOJO::FieldElement> elements(array.data, array.data + array.data_len);

    for (const auto& element : elements)
    {
        Ref<FieldElement> felt = memnew(FieldElement(element));
        result_array.append(felt->to_string());
    }
    return result_array;
}

Variant DojoArray::CArrayTokenContractToVariant(DOJO::CArrayTokenContract array)
{
    Array result;
    std::vector<DOJO::TokenContract> elements(array.data, array.data + array.data_len);

    for (const auto& element : elements)
    {
        Dictionary element_dict = {};

        Ref<FieldElement> contract_address = memnew(FieldElement(element.contract_address));
        element_dict["contract_address"] = contract_address->to_string();
        element_dict["name"] = element.name;
        element_dict["symbol"] = element.symbol;
        element_dict["decimals"] = element.decimals;
        element_dict["token_metadata"] = element.token_metadata;
        element_dict["metadata"] = element.metadata;
        element_dict["total_supply"] = memnew(DojoOptionU256(element.total_supply));

        result.append(element_dict);
    }
    return result;
}

Variant DojoArray::CArrayCOptionFieldElementToVariant(DOJO::CArrayCOptionFieldElement array)
{
    Array result;
    std::vector<DOJO::COptionFieldElement> options(array.data, array.data + array.data_len);

    for (const auto& option : options)
    {
        if (option.tag == DOJO::SomeFieldElement)
        {
            result.append(FieldElement(option.some).to_string());
        }
    }
    return result;
}

Variant DojoArray::CArrayModelToVariant(DOJO::CArrayModel array)
{
    std::vector<DOJO::Model> models_vec(array.data, array.data + array.data_len);
    Array result;

    for (const auto& model : models_vec)
    {
        Dictionary model_dict;
        Ref<FieldElement> selector_felt = memnew(FieldElement(model.selector));
        Ref<DojoTy> metadata_ty = memnew(DojoTy(model.schema));

        model_dict["world_address"] = FieldElement::get_as_string_no_ptr(model.world_address);
        model_dict["schema"] = metadata_ty->get_value();
        model_dict["namespace"] = model.namespace_;
        model_dict["name"] = model.name;
        model_dict["selector"] = FieldElement::get_as_string_no_ptr(model.selector);
        model_dict["packed_size"] = Variant(model.packed_size);
        model_dict["unpacked_size"] = Variant(model.unpacked_size);
        model_dict["class_hash"] = FieldElement::get_as_string_no_ptr(model.class_hash);
        model_dict["contract_address"] = FieldElement::get_as_string_no_ptr(model.contract_address);
        model_dict["layout"] = model.layout;
        model_dict["use_legacy_store"] = model.use_legacy_store;
        model_dict["selector_txt1"] = selector_felt->bytearray_deserialize(32);
        model_dict["selector_txt2"] = selector_felt->parse_cairo();

        result.append(model_dict);
    }
    return result;
}

Variant DojoArray::CArrayActionCountToVariant(dojo_bindings::CArrayActionCount array) {
    std::vector<DOJO::ActionCount> action_counts(array.data, array.data + array.data_len);
    Array result = {};

    for (const auto& action : action_counts) {
        Dictionary action_dict;
        action_dict["action_name"] = action.action_name;
        action_dict["count"] = action.count;
        result.append(action_dict);
    }

    return result;
}

Variant DojoArray::CArrayActivityToVariant(DOJO::CArrayActivity array)
{
    std::vector<DOJO::Activity> activities(array.data, array.data + array.data_len);
    Array result;

    for (const auto& activity : activities)
    {
        Dictionary activity_dict;
        activity_dict["id"] = activity.id;
        activity_dict["world_address"] = FieldElement::get_as_string_no_ptr(activity.world_address);
        activity_dict["namespace"] = activity.namespace_;
        activity_dict["caller_address"] = FieldElement::get_as_string_no_ptr(activity.caller_address);
        activity_dict["session_start"] = activity.session_start;
        activity_dict["session_end"] = activity.session_end;
        activity_dict["action_count"] = activity.action_count;
        activity_dict["actions"] = CArrayActionCountToVariant(activity.actions);
        activity_dict["updated_at"] = activity.updated_at;

        result.append(activity_dict);
    }

    return result;
}

Variant DojoArray::CArrayAggregationEntryToVariant(DOJO::CArrayAggregationEntry array)
{
    std::vector<DOJO::AggregationEntry> entries(array.data, array.data + array.data_len);
    Array result;

    for (const auto& entry : entries)
    {
        Dictionary entry_dict;
        entry_dict["id"] = entry.id;
        entry_dict["aggregator_id"] = entry.aggregator_id;
        entry_dict["entity_id"] = entry.entity_id;
        entry_dict["value"] = memnew(U256(entry.value));
        entry_dict["display_value"] = entry.display_value;
        entry_dict["position"] = entry.position;
        entry_dict["model_id"] = entry.model_id;
        entry_dict["created_at"] = entry.created_at;
        entry_dict["updated_at"] = entry.updated_at;

        result.append(entry_dict);
    }

    return result;
}


Variant DojoArray::CArrayAchievementToVariant(DOJO::CArrayAchievement array)
 {
     std::vector<DOJO::Achievement> achievements(array.data, array.data + array.data_len);
     Array result;

     for (const auto& achievement : achievements)
     {
         Dictionary achievement_dict;
         achievement_dict["id"] = achievement.id;
         achievement_dict["world_address"] = FieldElement::get_as_string_no_ptr(achievement.world_address);
         achievement_dict["namespace"] = achievement.namespace_;
         achievement_dict["entity_id"] = achievement.entity_id;
         achievement_dict["hidden"] = achievement.hidden;
         achievement_dict["index"] = achievement.index;
         achievement_dict["points"] = achievement.points;
         achievement_dict["start"] = achievement.start;
         achievement_dict["end"] = achievement.end;
         achievement_dict["group"] = achievement.group;
         achievement_dict["icon"] = achievement.icon;
         achievement_dict["title"] = achievement.title;
         achievement_dict["description"] = achievement.description;
         achievement_dict["tasks"] = CArrayAchievementTaskToVariant(achievement.tasks);
         achievement_dict["data"] = achievement.data;
         achievement_dict["total_completions"] = achievement.total_completions;
         achievement_dict["completion_rate"] = achievement.completion_rate;
         achievement_dict["created_at"] = achievement.created_at;
         achievement_dict["updated_at"] = achievement.updated_at;

         result.append(achievement_dict);
     }

     return result;
 }

 Variant DojoArray::CArrayPlayerAchievementEntryToVariant(DOJO::CArrayPlayerAchievementEntry array)
 {
     std::vector<DOJO::PlayerAchievementEntry> entries(array.data, array.data + array.data_len);
     Array result;

     for (const auto& entry : entries)
     {
         Dictionary entry_dict;
         entry_dict["player_address"] = FieldElement::get_as_string_no_ptr(entry.player_address);

         Dictionary stats_dict;
         stats_dict["total_points"] = entry.stats.total_points;
         stats_dict["completed_achievements"] = entry.stats.completed_achievements;
         stats_dict["total_achievements"] = entry.stats.total_achievements;
         stats_dict["completion_percentage"] = entry.stats.completion_percentage;
         if (entry.stats.last_achievement_at.tag == DOJO::Someu64) {
             stats_dict["last_achievement_at"] = entry.stats.last_achievement_at.some;
         } else {
             stats_dict["last_achievement_at"] = Variant();
         }
         stats_dict["created_at"] = entry.stats.created_at;
         stats_dict["updated_at"] = entry.stats.updated_at;
         entry_dict["stats"] = stats_dict;

         entry_dict["achievements"] = CArrayPlayerAchievementProgressToVariant(entry.achievements);

         result.append(entry_dict);
     }

     return result;
 }

 Variant DojoArray::CArrayPlayerAchievementProgressToVariant(DOJO::CArrayPlayerAchievementProgress array)
 {
     std::vector<DOJO::PlayerAchievementProgress> progresses(array.data, array.data + array.data_len);
     Array result;

     for (const auto& progress : progresses)
     {
         Dictionary progress_dict;

         // Convert Achievement
         Dictionary achievement_dict;
         achievement_dict["id"] = progress.achievement.id;
         achievement_dict["world_address"] = FieldElement::get_as_string_no_ptr(progress.achievement.world_address);
         achievement_dict["namespace"] = progress.achievement.namespace_;
         achievement_dict["entity_id"] = progress.achievement.entity_id;
         achievement_dict["hidden"] = progress.achievement.hidden;
         achievement_dict["index"] = progress.achievement.index;
         achievement_dict["points"] = progress.achievement.points;
         achievement_dict["start"] = progress.achievement.start;
         achievement_dict["end"] = progress.achievement.end;
         achievement_dict["group"] = progress.achievement.group;
         achievement_dict["icon"] = progress.achievement.icon;
         achievement_dict["title"] = progress.achievement.title;
         achievement_dict["description"] = progress.achievement.description;
         achievement_dict["tasks"] = CArrayAchievementTaskToVariant(progress.achievement.tasks);
         achievement_dict["data"] = progress.achievement.data;
         achievement_dict["total_completions"] = progress.achievement.total_completions;
         achievement_dict["completion_rate"] = progress.achievement.completion_rate;
         achievement_dict["created_at"] = progress.achievement.created_at;
         achievement_dict["updated_at"] = progress.achievement.updated_at;
         progress_dict["achievement"] = achievement_dict;

         progress_dict["task_progress"] = CArrayTaskProgressToVariant(progress.task_progress);
         progress_dict["completed"] = progress.completed;
         progress_dict["progress_percentage"] = progress.progress_percentage;

         result.append(progress_dict);
     }

     return result;
 }

 Variant DojoArray::CArrayTaskProgressToVariant(DOJO::CArrayTaskProgress array)
 {
     std::vector<DOJO::TaskProgress> tasks(array.data, array.data + array.data_len);
     Array result;

     for (const auto& task : tasks)
     {
         Dictionary task_dict;
         task_dict["task_id"] = task.task_id;
         task_dict["count"] = task.count;
         task_dict["completed"] = task.completed;

         result.append(task_dict);
     }

     return result;
 }

Variant DojoArray::CArrayTableSearchResultsToVariant(dojo_bindings::CArrayTableSearchResults array) {
    std::vector<DOJO::TableSearchResults> table_search_results(array.data, array.data + array.data_len);
    Array result;

    for (const auto& table_search : table_search_results) {
        Dictionary search_dict;
        search_dict["table"] = table_search.table;
        search_dict["count"] = table_search.count;
        search_dict["matches"] = CArraySearchMatchToVariant(table_search.matches);
        result.append(search_dict);
    }
    return result;
}

Variant DojoArray::CArraySearchMatchToVariant(dojo_bindings::CArraySearchMatch array) {
    std::vector<DOJO::SearchMatch> search_matches(array.data, array.data + array.data_len);
    Array result;

    for (const auto& match : search_matches) {
        Dictionary match_dict;
        match_dict["id"] = match.id;
        match_dict["fields"] = CArraySearchMatchFieldToVariant(match.fields);
        match_dict["score"] = DojoOptionf64::from_native(match.score);

    }

    return result;
}

Variant DojoArray::CArraySearchMatchFieldToVariant(dojo_bindings::CArraySearchMatchField array) {
    std::vector<DOJO::SearchMatchField> match_fields(array.data, array.data + array.data_len);

    Array result;

    for (const auto& field : match_fields) {
        Dictionary field_dict;
        field_dict["key"] = field.key;
        field_dict["value"] = field.value;

    }
    return result;
}

Variant DojoArray::CArrayTokenBalanceToVariant(DOJO::CArrayTokenBalance array)
 {
     std::vector<DOJO::TokenBalance> balances(array.data, array.data + array.data_len);
     Array result;

     for (const auto& balance : balances)
     {
         Dictionary balance_dict;
         balance_dict["balance"] = memnew(U256(balance.balance));
         balance_dict["account_address"] = FieldElement::get_as_string_no_ptr(balance.account_address);
         balance_dict["contract_address"] = FieldElement::get_as_string_no_ptr(balance.contract_address);
         balance_dict["token_id"] = memnew(DojoOptionU256(balance.token_id));

         result.append(balance_dict);
     }

     return result;
 }




 Variant DojoArray::CArrayTokenTransferToVariant(DOJO::CArrayTokenTransfer array)
 {
     std::vector<DOJO::TokenTransfer> transfers(array.data, array.data + array.data_len);
     Array result;

     for (const auto& transfer : transfers)
     {
         Dictionary transfer_dict;
         transfer_dict["id"] = transfer.id;
         transfer_dict["contract_address"] = FieldElement::get_as_string_no_ptr(transfer.contract_address);
         transfer_dict["from_address"] = FieldElement::get_as_string_no_ptr(transfer.from_address);
         transfer_dict["to_address"] = FieldElement::get_as_string_no_ptr(transfer.to_address);
         transfer_dict["amount"] = memnew(U256(transfer.amount));
         transfer_dict["token_id"] = memnew(DojoOptionU256(transfer.token_id));
         transfer_dict["executed_at"] = transfer.executed_at;

         if (transfer.event_id.tag == DOJO::Somec_char)
         {
             transfer_dict["event_id"] = transfer.event_id.some;
         } else {
             transfer_dict["event_id"] = Variant();
         }

         result.append(transfer_dict);
     }

     return result;
 }

 Variant DojoArray::CArrayAchievementTaskToVariant(DOJO::CArrayAchievementTask array)
 {
     std::vector<DOJO::AchievementTask> tasks(array.data, array.data + array.data_len);
     Array result;

     for (const auto& task : tasks)
     {
         Dictionary task_dict;
         task_dict["task_id"] = task.task_id;
         task_dict["description"] = task.description;
         task_dict["total"] = task.total;
         task_dict["total_completions"] = task.total_completions;
         task_dict["completion_rate"] = task.completion_rate;
         task_dict["created_at"] = task.created_at;
         result.append(task_dict);
     }

     return result;
 }