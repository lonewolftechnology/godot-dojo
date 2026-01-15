#include "ref_counted/dojo_utilities/callback_utils.hpp"

// Helper para std::visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

Dictionary CallbackUtils::entity_to_dict(const dojo::Entity& entity) {
    Dictionary dict;
    dict["world_address"] = String(entity.world_address.c_str());
    dict["hashed_keys"] = String(entity.hashed_keys.c_str());
    dict["created_at"] = entity.created_at;
    dict["updated_at"] = entity.updated_at;
    dict["executed_at"] = entity.executed_at;

    Dictionary models;
    for (const auto& model : entity.models) {
        if (model) {
            models[String(model->name.c_str())] = struct_to_dict(model);
        }
    }
    dict["models"] = models;

    return dict;
}

Dictionary CallbackUtils::event_to_dict(const dojo::Event& event) {
    Dictionary dict;
    dict["transaction_hash"] = String(event.transaction_hash.c_str());
    
    Array keys;
    for (const auto& k : event.keys) keys.append(String(k.c_str()));
    dict["keys"] = keys;

    Array data;
    for (const auto& d : event.data) data.append(String(d.c_str()));
    dict["data"] = data;

    return dict;
}

Dictionary CallbackUtils::token_balance_to_dict(const dojo::TokenBalance& balance) {
    Dictionary dict;
    dict["balance"] = String(balance.balance.c_str());
    dict["account_address"] = String(balance.account_address.c_str());
    dict["contract_address"] = String(balance.contract_address.c_str());
    if (balance.token_id.has_value()) {
        dict["token_id"] = String(balance.token_id.value().c_str());
    } else {
        dict["token_id"] = Variant();
    }
    return dict;
}

Dictionary CallbackUtils::token_to_dict(const dojo::Token& token) {
    Dictionary dict;
    dict["contract_address"] = String(token.contract_address.c_str());
    if (token.token_id.has_value()) {
        dict["token_id"] = String(token.token_id.value().c_str());
    }
    dict["name"] = String(token.name.c_str());
    dict["symbol"] = String(token.symbol.c_str());
    dict["decimals"] = token.decimals;
    dict["metadata"] = String(token.metadata.c_str());
    if (token.total_supply.has_value()) {
        dict["total_supply"] = String(token.total_supply.value().c_str());
    }
    return dict;
}

Dictionary CallbackUtils::token_contract_to_dict(const dojo::TokenContract& contract) {
    Dictionary dict;
    dict["contract_address"] = String(contract.contract_address.c_str());
    dict["name"] = String(contract.name.c_str());
    dict["symbol"] = String(contract.symbol.c_str());
    dict["decimals"] = contract.decimals;
    dict["metadata"] = String(contract.metadata.c_str());
    dict["token_metadata"] = String(contract.token_metadata.c_str());
    if (contract.total_supply.has_value()) {
        dict["total_supply"] = String(contract.total_supply.value().c_str());
    }
    return dict;
}

Dictionary CallbackUtils::token_transfer_to_dict(const dojo::TokenTransfer& transfer) {
    Dictionary dict;
    dict["id"] = String(transfer.id.c_str());
    dict["contract_address"] = String(transfer.contract_address.c_str());
    dict["from_address"] = String(transfer.from_address.c_str());
    dict["to_address"] = String(transfer.to_address.c_str());
    dict["amount"] = String(transfer.amount.c_str());
    if (transfer.token_id.has_value()) {
        dict["token_id"] = String(transfer.token_id.value().c_str());
    }
    dict["executed_at"] = transfer.executed_at;
    if (transfer.event_id.has_value()) {
        dict["event_id"] = String(transfer.event_id.value().c_str());
    }
    return dict;
}

Dictionary CallbackUtils::transaction_to_dict(const dojo::Transaction& transaction) {
    Dictionary dict;
    dict["transaction_hash"] = String(transaction.transaction_hash.c_str());
    dict["sender_address"] = String(transaction.sender_address.c_str());
    dict["max_fee"] = String(transaction.max_fee.c_str());
    dict["nonce"] = String(transaction.nonce.c_str());
    dict["block_number"] = transaction.block_number;
    dict["transaction_type"] = String(transaction.transaction_type.c_str());
    dict["block_timestamp"] = transaction.block_timestamp;
    return dict;
}

Dictionary CallbackUtils::contract_to_dict(const dojo::Contract& contract) {
    Dictionary dict;
    dict["contract_address"] = String(contract.contract_address.c_str());
    dict["contract_type"] = (int)contract.contract_type;
    if (contract.head.has_value()) dict["head"] = contract.head.value();
    if (contract.tps.has_value()) dict["tps"] = contract.tps.value();
    if (contract.last_block_timestamp.has_value()) dict["last_block_timestamp"] = contract.last_block_timestamp.value();
    if (contract.last_pending_block_tx.has_value()) dict["last_pending_block_tx"] = String(contract.last_pending_block_tx.value().c_str());
    dict["updated_at"] = contract.updated_at;
    dict["created_at"] = contract.created_at;
    return dict;
}

Dictionary CallbackUtils::controller_to_dict(const dojo::Controller& controller) {
    Dictionary dict;
    dict["address"] = String(controller.address.c_str());
    dict["username"] = String(controller.username.c_str());
    dict["deployed_at_timestamp"] = controller.deployed_at_timestamp;
    return dict;
}

Dictionary CallbackUtils::achievement_to_dict(const dojo::Achievement& achievement) {
    Dictionary dict;
    dict["id"] = String(achievement.id.c_str());
    dict["world_address"] = String(achievement.world_address.c_str());
    dict["namespace"] = String(achievement.namespace_.c_str());
    dict["entity_id"] = String(achievement.entity_id.c_str());
    dict["hidden"] = achievement.hidden;
    dict["index"] = achievement.index;
    dict["points"] = achievement.points;
    dict["start"] = String(achievement.start.c_str());
    dict["end"] = String(achievement.end.c_str());
    dict["group"] = String(achievement.group.c_str());
    dict["icon"] = String(achievement.icon.c_str());
    dict["title"] = String(achievement.title.c_str());
    dict["description"] = String(achievement.description.c_str());

    Array tasks;
    for (const auto& task : achievement.tasks) {
        if (task) {
            Dictionary t;
            t["task_id"] = String(task->task_id.c_str());
            t["description"] = String(task->description.c_str());
            t["total"] = task->total;
            t["total_completions"] = task->total_completions;
            t["completion_rate"] = task->completion_rate;
            t["created_at"] = task->created_at;
            tasks.append(t);
        }
    }
    dict["tasks"] = tasks;

    if (achievement.data.has_value()) dict["data"] = String(achievement.data.value().c_str());
    dict["total_completions"] = achievement.total_completions;
    dict["completion_rate"] = achievement.completion_rate;
    dict["created_at"] = achievement.created_at;
    dict["updated_at"] = achievement.updated_at;
    return dict;
}

Dictionary CallbackUtils::player_achievement_entry_to_dict(const dojo::PlayerAchievementEntry& entry) {
    Dictionary dict;
    dict["player_address"] = String(entry.player_address.c_str());

    if (entry.stats) {
        Dictionary stats;
        stats["total_points"] = entry.stats->total_points;
        stats["completed_achievements"] = entry.stats->completed_achievements;
        stats["total_achievements"] = entry.stats->total_achievements;
        stats["completion_percentage"] = entry.stats->completion_percentage;
        if (entry.stats->last_achievement_at.has_value()) stats["last_achievement_at"] = entry.stats->last_achievement_at.value();
        stats["created_at"] = entry.stats->created_at;
        stats["updated_at"] = entry.stats->updated_at;
        dict["stats"] = stats;
    }

    Array achievements;
    for (const auto& prog : entry.achievements) {
        if (prog) {
            Dictionary p;
            if (prog->achievement) p["achievement"] = achievement_to_dict(*prog->achievement);

            Array task_progress;
            for (const auto& tp : prog->task_progress) {
                if (tp) {
                    Dictionary t;
                    t["task_id"] = String(tp->task_id.c_str());
                    t["count"] = tp->count;
                    t["completed"] = tp->completed;
                    task_progress.append(t);
                }
            }
            p["task_progress"] = task_progress;

            p["completed"] = prog->completed;
            p["progress_percentage"] = prog->progress_percentage;
            achievements.append(p);
        }
    }
    dict["achievements"] = achievements;

    return dict;
}

Dictionary CallbackUtils::activity_to_dict(const dojo::Activity& activity) {
    Dictionary dict;
    dict["id"] = String(activity.id.c_str());
    dict["world_address"] = String(activity.world_address.c_str());
    dict["namespace"] = String(activity.namespace_.c_str());
    dict["caller_address"] = String(activity.caller_address.c_str());
    dict["session_start"] = activity.session_start;
    dict["session_end"] = activity.session_end;
    dict["action_count"] = activity.action_count;

    Array actions;
    for (const auto& action : activity.actions) {
        if (action) {
            Dictionary a;
            a["action_name"] = String(action->action_name.c_str());
            a["count"] = action->count;
            actions.append(a);
        }
    }
    dict["actions"] = actions;

    dict["updated_at"] = activity.updated_at;
    return dict;
}

Dictionary CallbackUtils::aggregation_entry_to_dict(const dojo::AggregationEntry& entry) {
    Dictionary dict;
    dict["id"] = String(entry.id.c_str());
    dict["aggregator_id"] = String(entry.aggregator_id.c_str());
    dict["entity_id"] = String(entry.entity_id.c_str());
    dict["value"] = String(entry.value.c_str());
    dict["display_value"] = String(entry.display_value.c_str());
    dict["position"] = entry.position;
    dict["model_id"] = String(entry.model_id.c_str());
    dict["created_at"] = entry.created_at;
    dict["updated_at"] = entry.updated_at;
    return dict;
}

Dictionary CallbackUtils::table_search_results_to_dict(const dojo::TableSearchResults& results) {
    Dictionary dict;
    dict["table"] = String(results.table.c_str());
    dict["count"] = results.count;

    Array matches;
    for (const auto& match : results.matches) {
        if (match) {
            Dictionary m;
            m["id"] = String(match->id.c_str());

            Array fields;
            for (const auto& field : match->fields) {
                if (field) {
                    Dictionary f;
                    f["key"] = String(field->key.c_str());
                    f["value"] = String(field->value.c_str());
                    fields.append(f);
                }
            }
            m["fields"] = fields;

            if (match->score.has_value()) m["score"] = match->score.value();
            matches.append(m);
        }
    }
    dict["matches"] = matches;

    return dict;
}

Variant CallbackUtils::ty_to_variant(const std::shared_ptr<dojo::Ty>& ty) {
    if (!ty) return Variant();

    return std::visit(overloaded {
        [](const dojo::Ty::kPrimitive& v) { return primitive_to_variant(v.value); },
        [](const dojo::Ty::kStruct& v) { return Variant(struct_to_dict(v.value)); },
        [](const dojo::Ty::kEnum& v) { return Variant(enum_to_dict(v.value)); },
        [](const dojo::Ty::kTuple& v) {
            Array arr;
            for (const auto& item : v.values) arr.append(ty_to_variant(item));
            return Variant(arr);
        },
        [](const dojo::Ty::kArray& v) {
            Array arr;
            for (const auto& item : v.values) arr.append(ty_to_variant(item));
            return Variant(arr);
        },
        [](const dojo::Ty::kFixedSizeArray& v) {
            Array arr;
            if (v.value) {
                for (const auto& item : v.value->array) arr.append(ty_to_variant(item));
            }
            return Variant(arr);
        },
        [](const dojo::Ty::kByteArray& v) { return Variant(String(v.value.c_str())); }
    }, ty->get_variant());
}

Variant CallbackUtils::primitive_to_variant(const std::shared_ptr<dojo::Primitive>& primitive) {
    if (!primitive) return Variant();

    return std::visit(overloaded {
        [](const dojo::Primitive::kI8& v) { return Variant(v.value); },
        [](const dojo::Primitive::kI16& v) { return Variant(v.value); },
        [](const dojo::Primitive::kI32& v) { return Variant(v.value); },
        [](const dojo::Primitive::kI64& v) { return Variant(v.value); },
        [](const dojo::Primitive::kI128& v) { 
             PackedByteArray bytes;
             bytes.resize(v.value.size());
             memcpy(bytes.ptrw(), v.value.data(), v.value.size());
             return Variant(bytes);
        },
        [](const dojo::Primitive::kU8& v) { return Variant(v.value); },
        [](const dojo::Primitive::kU16& v) { return Variant(v.value); },
        [](const dojo::Primitive::kU32& v) { return Variant(v.value); },
        [](const dojo::Primitive::kU64& v) { return Variant(v.value); },
        [](const dojo::Primitive::kU128& v) {
             PackedByteArray bytes;
             bytes.resize(v.value.size());
             memcpy(bytes.ptrw(), v.value.data(), v.value.size());
             return Variant(bytes);
        },
        [](const dojo::Primitive::kU256& v) { return Variant(String(v.value.c_str())); },
        [](const dojo::Primitive::kBool& v) { return Variant(v.value); },
        [](const dojo::Primitive::kFelt252& v) { return Variant(String(v.value.c_str())); },
        [](const dojo::Primitive::kClassHash& v) { return Variant(String(v.value.c_str())); },
        [](const dojo::Primitive::kContractAddress& v) { return Variant(String(v.value.c_str())); },
        [](const dojo::Primitive::kEthAddress& v) { return Variant(String(v.value.c_str())); }
    }, primitive->get_variant());
}

Dictionary CallbackUtils::struct_to_dict(const std::shared_ptr<dojo::Struct>& struct_ptr) {
    Dictionary dict;
    if (!struct_ptr) return dict;
    
    for (const auto& member : struct_ptr->children) {
        if (member) {
            dict[String(member->name.c_str())] = ty_to_variant(member->ty);
        }
    }
    return dict;
}

Dictionary CallbackUtils::enum_to_dict(const std::shared_ptr<dojo::EnumType>& enum_ptr) {
    Dictionary dict;
    if (!enum_ptr) return dict;

    dict["name"] = String(enum_ptr->name.c_str());
    dict["option_index"] = enum_ptr->option;
    
    if (enum_ptr->option < enum_ptr->options.size()) {
        auto opt = enum_ptr->options[enum_ptr->option];
        if (opt) {
            dict["option"] = String(opt->name.c_str());
            if (opt->ty) {
                dict["value"] = ty_to_variant(opt->ty);
            }
        }
    }
    return dict;
}

Dictionary CallbackUtils::sql_value_to_dict(const dojo::SqlValue& value) {
    Dictionary dict;
    std::visit(overloaded {
        [&](const dojo::SqlValue::kText& v) {
            dict["type"] = "Text";
            dict["value"] = String(v.value.c_str());
        },
        [&](const dojo::SqlValue::kInteger& v) {
            dict["type"] = "Integer";
            dict["value"] = v.value;
        },
        [&](const dojo::SqlValue::kReal& v) {
            dict["type"] = "Real";
            dict["value"] = v.value;
        },
        [&](const dojo::SqlValue::kBlob& v) {
            dict["type"] = "Blob";
            PackedByteArray bytes;
            bytes.resize(v.value.size());
            memcpy(bytes.ptrw(), v.value.data(), v.value.size());
            dict["value"] = bytes;
        },
        [&](const dojo::SqlValue::kNull& v) {
            dict["type"] = "Null";
            dict["value"] = Variant();
        }
    }, value.get_variant());
    return dict;
}
