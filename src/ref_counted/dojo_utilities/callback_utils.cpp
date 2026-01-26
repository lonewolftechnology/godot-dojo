#include "ref_counted/dojo_utilities/callback_utils.hpp"

// Helper para std::visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

Dictionary CallbackUtils::entity_to_dict(const dojo::Entity& entity) {
    Dictionary dict;
    const Variant wa = String(entity.world_address.c_str());
    dict["world_address"] = wa;
    const Variant hk = String(entity.hashed_keys.c_str());
    dict["hashed_keys"] = hk;
    const Variant ca = entity.created_at;
    dict["created_at"] = ca;
    const Variant ua = entity.updated_at;
    dict["updated_at"] = ua;
    const Variant ea = entity.executed_at;
    dict["executed_at"] = ea;

    Dictionary models;
    for (const auto& model : entity.models) {
        if (model) {
            const Variant model_val = struct_to_dict(model);
            models[String(model->name.c_str())] = model_val;
        }
    }
    const Variant models_variant = models;
    dict["models"] = models_variant;

    return dict;
}

Dictionary CallbackUtils::event_to_dict(const dojo::Event& event) {
    Dictionary dict;
    const Variant th = String(event.transaction_hash.c_str());
    dict["transaction_hash"] = th;
    
    Array keys;
    for (const auto& k : event.keys) keys.append(String(k.c_str()));
    const Variant keys_variant = keys;
    dict["keys"] = keys_variant;

    Array data;
    for (const auto& d : event.data) data.append(String(d.c_str()));
    const Variant data_variant = data;
    dict["data"] = data_variant;

    return dict;
}

Dictionary CallbackUtils::token_balance_to_dict(const dojo::TokenBalance& balance) {
    Dictionary dict;
    const Variant bal = String(balance.balance.c_str());
    dict["balance"] = bal;
    const Variant aa = String(balance.account_address.c_str());
    dict["account_address"] = aa;
    const Variant ca = String(balance.contract_address.c_str());
    dict["contract_address"] = ca;
    if (balance.token_id.has_value()) {
        const Variant tid = String(balance.token_id.value().c_str());
        dict["token_id"] = tid;
    } else {
        const Variant null_var;
        dict["token_id"] = null_var;
    }
    return dict;
}

Dictionary CallbackUtils::token_to_dict(const dojo::Token& token) {
    Dictionary dict;
    const Variant ca = String(token.contract_address.c_str());
    dict["contract_address"] = ca;
    if (token.token_id.has_value()) {
        const Variant tid = String(token.token_id.value().c_str());
        dict["token_id"] = tid;
    }
    const Variant name = String(token.name.c_str());
    dict["name"] = name;
    const Variant sym = String(token.symbol.c_str());
    dict["symbol"] = sym;
    const Variant dec = token.decimals;
    dict["decimals"] = dec;
    const Variant meta = String(token.metadata.c_str());
    dict["metadata"] = meta;
    if (token.total_supply.has_value()) {
        const Variant ts = String(token.total_supply.value().c_str());
        dict["total_supply"] = ts;
    }
    return dict;
}

Dictionary CallbackUtils::token_contract_to_dict(const dojo::TokenContract& contract) {
    Dictionary dict;
    const Variant ca = String(contract.contract_address.c_str());
    dict["contract_address"] = ca;
    const Variant name = String(contract.name.c_str());
    dict["name"] = name;
    const Variant sym = String(contract.symbol.c_str());
    dict["symbol"] = sym;
    const Variant dec = contract.decimals;
    dict["decimals"] = dec;
    const Variant meta = String(contract.metadata.c_str());
    dict["metadata"] = meta;
    const Variant tmeta = String(contract.token_metadata.c_str());
    dict["token_metadata"] = tmeta;
    if (contract.total_supply.has_value()) {
        const Variant ts = String(contract.total_supply.value().c_str());
        dict["total_supply"] = ts;
    }
    return dict;
}

Dictionary CallbackUtils::token_transfer_to_dict(const dojo::TokenTransfer& transfer) {
    Dictionary dict;
    const Variant id = String(transfer.id.c_str());
    dict["id"] = id;
    const Variant ca = String(transfer.contract_address.c_str());
    dict["contract_address"] = ca;
    const Variant fa = String(transfer.from_address.c_str());
    dict["from_address"] = fa;
    const Variant ta = String(transfer.to_address.c_str());
    dict["to_address"] = ta;
    const Variant amt = String(transfer.amount.c_str());
    dict["amount"] = amt;
    if (transfer.token_id.has_value()) {
        const Variant tid = String(transfer.token_id.value().c_str());
        dict["token_id"] = tid;
    }
    const Variant ea = transfer.executed_at;
    dict["executed_at"] = ea;
    if (transfer.event_id.has_value()) {
        const Variant eid = String(transfer.event_id.value().c_str());
        dict["event_id"] = eid;
    }
    return dict;
}

Dictionary CallbackUtils::transaction_to_dict(const dojo::Transaction& transaction) {
    Dictionary dict;
    const Variant th = String(transaction.transaction_hash.c_str());
    dict["transaction_hash"] = th;
    const Variant sa = String(transaction.sender_address.c_str());
    dict["sender_address"] = sa;
    const Variant mf = String(transaction.max_fee.c_str());
    dict["max_fee"] = mf;
    const Variant nonce = String(transaction.nonce.c_str());
    dict["nonce"] = nonce;
    const Variant bn = transaction.block_number;
    dict["block_number"] = bn;
    const Variant tt = String(transaction.transaction_type.c_str());
    dict["transaction_type"] = tt;
    const Variant bt = transaction.block_timestamp;
    dict["block_timestamp"] = bt;
    return dict;
}

Dictionary CallbackUtils::contract_to_dict(const dojo::Contract& contract) {
    Dictionary dict;
    const Variant ca = String(contract.contract_address.c_str());
    dict["contract_address"] = ca;
    const Variant ct = (int)contract.contract_type;
    dict["contract_type"] = ct;
    if (contract.head.has_value()) {
        const Variant head = contract.head.value();
        dict["head"] = head;
    }
    if (contract.tps.has_value()) {
        const Variant tps = contract.tps.value();
        dict["tps"] = tps;
    }
    if (contract.last_block_timestamp.has_value()) {
        const Variant lbt = contract.last_block_timestamp.value();
        dict["last_block_timestamp"] = lbt;
    }
    if (contract.last_pending_block_tx.has_value()) {
        const Variant lpbt = String(contract.last_pending_block_tx.value().c_str());
        dict["last_pending_block_tx"] = lpbt;
    }
    const Variant ua = contract.updated_at;
    dict["updated_at"] = ua;
    const Variant cra = contract.created_at;
    dict["created_at"] = cra;
    return dict;
}

Dictionary CallbackUtils::controller_to_dict(const dojo::Controller& controller) {
    Dictionary dict;
    const Variant addr = String(controller.address.c_str());
    dict["address"] = addr;
    const Variant uname = String(controller.username.c_str());
    dict["username"] = uname;
    const Variant dat = controller.deployed_at_timestamp;
    dict["deployed_at_timestamp"] = dat;
    return dict;
}

Dictionary CallbackUtils::achievement_to_dict(const dojo::Achievement& achievement) {
    Dictionary dict;
    const Variant id = String(achievement.id.c_str());
    dict["id"] = id;
    const Variant wa = String(achievement.world_address.c_str());
    dict["world_address"] = wa;
    const Variant ns = String(achievement.namespace_.c_str());
    dict["namespace"] = ns;
    const Variant eid = String(achievement.entity_id.c_str());
    dict["entity_id"] = eid;
    const Variant hidden = achievement.hidden;
    dict["hidden"] = hidden;
    const Variant idx = achievement.index;
    dict["index"] = idx;
    const Variant pts = achievement.points;
    dict["points"] = pts;
    const Variant start = String(achievement.start.c_str());
    dict["start"] = start;
    const Variant end = String(achievement.end.c_str());
    dict["end"] = end;
    const Variant grp = String(achievement.group.c_str());
    dict["group"] = grp;
    const Variant icon = String(achievement.icon.c_str());
    dict["icon"] = icon;
    const Variant title = String(achievement.title.c_str());
    dict["title"] = title;
    const Variant desc = String(achievement.description.c_str());
    dict["description"] = desc;

    Array tasks;
    for (const auto& task : achievement.tasks) {
        if (task) {
            Dictionary t;
            const Variant tid = String(task->task_id.c_str());
            t["task_id"] = tid;
            const Variant tdesc = String(task->description.c_str());
            t["description"] = tdesc;
            const Variant tot = task->total;
            t["total"] = tot;
            const Variant tc = task->total_completions;
            t["total_completions"] = tc;
            const Variant cr = task->completion_rate;
            t["completion_rate"] = cr;
            const Variant tca = task->created_at;
            t["created_at"] = tca;
            tasks.append(t);
        }
    }
    const Variant tasks_variant = tasks;
    dict["tasks"] = tasks_variant;

    if (achievement.data.has_value()) {
        const Variant data = String(achievement.data.value().c_str());
        dict["data"] = data;
    }
    const Variant tc = achievement.total_completions;
    dict["total_completions"] = tc;
    const Variant cr = achievement.completion_rate;
    dict["completion_rate"] = cr;
    const Variant ca = achievement.created_at;
    dict["created_at"] = ca;
    const Variant ua = achievement.updated_at;
    dict["updated_at"] = ua;
    return dict;
}

Dictionary CallbackUtils::player_achievement_entry_to_dict(const dojo::PlayerAchievementEntry& entry) {
    Dictionary dict;
    const Variant pa = String(entry.player_address.c_str());
    dict["player_address"] = pa;

    if (entry.stats) {
        Dictionary stats;
        const Variant tp = entry.stats->total_points;
        stats["total_points"] = tp;
        const Variant ca = entry.stats->completed_achievements;
        stats["completed_achievements"] = ca;
        const Variant ta = entry.stats->total_achievements;
        stats["total_achievements"] = ta;
        const Variant cp = entry.stats->completion_percentage;
        stats["completion_percentage"] = cp;
        if (entry.stats->last_achievement_at.has_value()) {
            const Variant laa = entry.stats->last_achievement_at.value();
            stats["last_achievement_at"] = laa;
        }
        const Variant sca = entry.stats->created_at;
        stats["created_at"] = sca;
        const Variant sua = entry.stats->updated_at;
        stats["updated_at"] = sua;
        const Variant stats_variant = stats;
        dict["stats"] = stats_variant;
    }

    Array achievements;
    for (const auto& prog : entry.achievements) {
        if (prog) {
            Dictionary p;
            if (prog->achievement) {
                const Variant achievement_variant = achievement_to_dict(*prog->achievement);
                p["achievement"] = achievement_variant;
            }

            Array task_progress;
            for (const auto& tp : prog->task_progress) {
                if (tp) {
                    Dictionary t;
                    const Variant tid = String(tp->task_id.c_str());
                    t["task_id"] = tid;
                    const Variant cnt = tp->count;
                    t["count"] = cnt;
                    const Variant comp = tp->completed;
                    t["completed"] = comp;
                    task_progress.append(t);
                }
            }
            const Variant task_progress_variant = task_progress;
            p["task_progress"] = task_progress_variant;

            const Variant pcomp = prog->completed;
            p["completed"] = pcomp;
            const Variant pp = prog->progress_percentage;
            p["progress_percentage"] = pp;
            achievements.append(p);
        }
    }
    const Variant achievements_variant = achievements;
    dict["achievements"] = achievements_variant;

    return dict;
}

Dictionary CallbackUtils::activity_to_dict(const dojo::Activity& activity) {
    Dictionary dict;
    const Variant id = String(activity.id.c_str());
    dict["id"] = id;
    const Variant wa = String(activity.world_address.c_str());
    dict["world_address"] = wa;
    const Variant ns = String(activity.namespace_.c_str());
    dict["namespace"] = ns;
    const Variant ca = String(activity.caller_address.c_str());
    dict["caller_address"] = ca;
    const Variant ss = activity.session_start;
    dict["session_start"] = ss;
    const Variant se = activity.session_end;
    dict["session_end"] = se;
    const Variant ac = activity.action_count;
    dict["action_count"] = ac;

    Array actions;
    for (const auto& action : activity.actions) {
        if (action) {
            Dictionary a;
            const Variant an = String(action->action_name.c_str());
            a["action_name"] = an;
            const Variant cnt = action->count;
            a["count"] = cnt;
            actions.append(a);
        }
    }
    const Variant actions_variant = actions;
    dict["actions"] = actions_variant;

    const Variant ua = activity.updated_at;
    dict["updated_at"] = ua;
    return dict;
}

Dictionary CallbackUtils::aggregation_entry_to_dict(const dojo::AggregationEntry& entry) {
    Dictionary dict;
    const Variant id = String(entry.id.c_str());
    dict["id"] = id;
    const Variant aid = String(entry.aggregator_id.c_str());
    dict["aggregator_id"] = aid;
    const Variant eid = String(entry.entity_id.c_str());
    dict["entity_id"] = eid;
    const Variant val = String(entry.value.c_str());
    dict["value"] = val;
    const Variant dv = String(entry.display_value.c_str());
    dict["display_value"] = dv;
    const Variant pos = entry.position;
    dict["position"] = pos;
    const Variant mid = String(entry.model_id.c_str());
    dict["model_id"] = mid;
    const Variant ca = entry.created_at;
    dict["created_at"] = ca;
    const Variant ua = entry.updated_at;
    dict["updated_at"] = ua;
    return dict;
}

Dictionary CallbackUtils::table_search_results_to_dict(const dojo::TableSearchResults& results) {
    Dictionary dict;
    const Variant tbl = String(results.table.c_str());
    dict["table"] = tbl;
    const Variant cnt = results.count;
    dict["count"] = cnt;

    Array matches;
    for (const auto& match : results.matches) {
        if (match) {
            Dictionary m;
            const Variant mid = String(match->id.c_str());
            m["id"] = mid;

            Array fields;
            for (const auto& field : match->fields) {
                if (field) {
                    Dictionary f;
                    const Variant k = String(field->key.c_str());
                    f["key"] = k;
                    const Variant v = String(field->value.c_str());
                    f["value"] = v;
                    fields.append(f);
                }
            }
            const Variant fields_variant = fields;
            m["fields"] = fields_variant;

            if (match->score.has_value()) {
                const Variant score = match->score.value();
                m["score"] = score;
            }
            matches.append(m);
        }
    }
    const Variant matches_variant = matches;
    dict["matches"] = matches_variant;

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
            const Variant member_variant = ty_to_variant(member->ty);
            dict[String(member->name.c_str())] = member_variant;
        }
    }
    return dict;
}

Dictionary CallbackUtils::enum_to_dict(const std::shared_ptr<dojo::EnumType>& enum_ptr) {
    Dictionary dict;
    if (!enum_ptr) return dict;

    const Variant name = String(enum_ptr->name.c_str());
    dict["name"] = name;
    const Variant opt_idx = enum_ptr->option;
    dict["option_index"] = opt_idx;
    
    if (enum_ptr->option < enum_ptr->options.size()) {
        auto opt = enum_ptr->options[enum_ptr->option];
        if (opt) {
            const Variant opt_name = String(opt->name.c_str());
            dict["option"] = opt_name;
            if (opt->ty) {
                const Variant value_variant = ty_to_variant(opt->ty);
                dict["value"] = value_variant;
            }
        }
    }
    return dict;
}

Dictionary CallbackUtils::sql_value_to_dict(const dojo::SqlValue& value) {
    Dictionary dict;
    std::visit(overloaded {
        [&](const dojo::SqlValue::kText& v) {
            const Variant t = "Text";
            dict["type"] = t;
            const Variant val = String(v.value.c_str());
            dict["value"] = val;
        },
        [&](const dojo::SqlValue::kInteger& v) {
            const Variant t = "Integer";
            dict["type"] = t;
            const Variant val = v.value;
            dict["value"] = val;
        },
        [&](const dojo::SqlValue::kReal& v) {
            const Variant t = "Real";
            dict["type"] = t;
            const Variant val = v.value;
            dict["value"] = val;
        },
        [&](const dojo::SqlValue::kBlob& v) {
            const Variant t = "Blob";
            dict["type"] = t;
            PackedByteArray bytes;
            bytes.resize(v.value.size());
            memcpy(bytes.ptrw(), v.value.data(), v.value.size());
            const Variant bytes_variant = bytes;
            dict["value"] = bytes_variant;
        },
        [&](const dojo::SqlValue::kNull& v) {
            const Variant t = "Null";
            dict["type"] = t;
            const Variant null_var;
            dict["value"] = null_var;
        }
    }, value.get_variant());
    return dict;
}
