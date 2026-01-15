#include "node/torii_client.hpp"
#include "ref_counted/dojo_utilities/callback_utils.hpp"
#include "ref_counted/dojo_utilities/clauses/keys.hpp"
#include "ref_counted/queries/achievement.hpp"
#include "ref_counted/queries/activity.hpp"
#include "ref_counted/queries/aggregation.hpp"
#include "ref_counted/queries/contract.hpp"
#include "ref_counted/queries/controller.hpp"
#include "ref_counted/queries/entity.hpp"
#include "ref_counted/queries/event.hpp"
#include "ref_counted/queries/player_achievement.hpp"
#include "ref_counted/queries/search.hpp"
#include "ref_counted/queries/token.hpp"
#include "ref_counted/queries/token_balance.hpp"
#include "ref_counted/queries/token_contract.hpp"
#include "ref_counted/queries/token_transfer.hpp"
#include "ref_counted/queries/transaction.hpp"

using namespace godot;

ToriiClient::ToriiClient() {}
ToriiClient::~ToriiClient() {}

ToriiClient* ToriiClient::create(const String& torii_url) {
    ToriiClient* node = memnew(ToriiClient);
    node->client = dojo::ToriiClient::init(torii_url.utf8().get_data());
    return node;
}

ToriiClient* ToriiClient::create_with_config(const String& torii_url, uint64_t max_message_size) {
    ToriiClient* node = memnew(ToriiClient);
    node->client = dojo::ToriiClient::new_with_config(torii_url.utf8().get_data(), max_message_size);
    return node;
}

Dictionary ToriiClient::achievements(const Ref<AchievementQuery>& query) {
    dojo::PageAchievement page = client->achievements(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::achievement_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::activities(const Ref<ActivityQuery>& query) {
    dojo::PageActivity page = client->activities(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::activity_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::aggregations(const Ref<AggregationQuery>& query) {
    dojo::PageAggregationEntry page = client->aggregations(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::aggregation_entry_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

TypedArray<Dictionary> ToriiClient::contracts(const Ref<ContractQuery>& query) {
    std::vector<std::shared_ptr<dojo::Contract>> contracts = client->contracts(query->get_native());
    TypedArray<Dictionary> result;
    for (const auto& contract : contracts) {
        if (contract) {
            result.append(CallbackUtils::contract_to_dict(*contract));
        }
    }
    return result;
}

Dictionary ToriiClient::controllers(const Ref<ControllerQuery>& query) {
    dojo::PageController page = client->controllers(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::controller_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::entities(const Ref<DojoQuery>& query) {
    dojo::PageEntity page = client->entities(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& entity : page.items) {
        if (entity) {
            items.append(CallbackUtils::entity_to_dict(*entity));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::event_messages(const Ref<DojoQuery>& query) {
    dojo::PageEntity page = client->event_messages(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& entity : page.items) {
        if (entity) {
            items.append(CallbackUtils::entity_to_dict(*entity));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::player_achievements(const Ref<PlayerAchievementQuery>& query) {
    dojo::PagePlayerAchievement page = client->player_achievements(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::player_achievement_entry_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::search(const Ref<SearchQuery>& query) {
    dojo::SearchResponse response = client->search(query->get_native());
    Dictionary result;

    result["total"] = response.total;

    Array results;
    for (const auto& item : response.results) {
        if (item) {
            results.append(CallbackUtils::table_search_results_to_dict(*item));
        }
    }
    result["results"] = results;

    return result;
}

Dictionary ToriiClient::starknet_events(const Ref<EventQuery>& query) {
    dojo::PageEvent page = client->starknet_events(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::event_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::token_balances(const Ref<TokenBalanceQuery>& query) {
    dojo::PageTokenBalance page = client->token_balances(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::token_balance_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::token_contracts(const Ref<TokenContractQuery>& query) {
    dojo::PageTokenContract page = client->token_contracts(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::token_contract_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::token_transfers(const Ref<TokenTransferQuery>& query) {
    dojo::PageTokenTransfer page = client->token_transfers(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::token_transfer_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::tokens(const Ref<TokenQuery>& query) {
    dojo::PageToken page = client->tokens(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::token_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

Dictionary ToriiClient::transactions(const Ref<TransactionQuery>& query) {
    dojo::PageTransaction page = client->transactions(query->get_native());
    Dictionary result;

    Array items;
    for (const auto& item : page.items) {
        if (item) {
            items.append(CallbackUtils::transaction_to_dict(*item));
        }
    }
    result["items"] = items;

    if (page.next_cursor.has_value()) {
        result["next_cursor"] = String(page.next_cursor.value().c_str());
    } else {
        result["next_cursor"] = Variant();
    }

    return result;
}

void ToriiClient::cancel_subscription(uint64_t subscription_id) {
    client->cancel_subscription(subscription_id);
}

String ToriiClient::publish_message(const String& message, const PackedStringArray& signature, const String& world_address) {
    dojo::Message msg;
    msg.message = message.utf8().get_data();
    msg.world_address = world_address.utf8().get_data();

    std::vector<dojo::FieldElement> sig_vec;
    for (int i = 0; i < signature.size(); i++) {
        sig_vec.push_back(signature[i].utf8().get_data());
    }
    msg.signature = sig_vec;

    return String(client->publish_message(msg).c_str());
}

PackedStringArray ToriiClient::publish_message_batch(const Array& messages) {
    std::vector<std::shared_ptr<dojo::Message>> msgs;
    for (int i = 0; i < messages.size(); i++) {
        Dictionary m = messages[i];
        auto msg = std::make_shared<dojo::Message>();
        msg->message = String(m["message"]).utf8().get_data();
        msg->world_address = String(m["world_address"]).utf8().get_data();

        PackedStringArray sig = m["signature"];
        std::vector<dojo::FieldElement> sig_vec;
        for (int j = 0; j < sig.size(); j++) {
            sig_vec.push_back(sig[j].utf8().get_data());
        }
        msg->signature = sig_vec;

        msgs.push_back(msg);
    }

    std::vector<std::string> result = client->publish_message_batch(msgs);
    PackedStringArray ret;
    for (const auto& r : result) {
        ret.append(String(r.c_str()));
    }
    return ret;
}

TypedArray<Dictionary> ToriiClient::sql(const String& query) {
    std::vector<std::shared_ptr<dojo::SqlRow>> rows = client->sql(query.utf8().get_data());
    TypedArray<Dictionary> result;

    for (const auto& row : rows) {
        Dictionary r;
        for (const auto& field : row->fields) {
            if (field && field->value) {
                r[String(field->name.c_str())] = CallbackUtils::sql_value_to_dict(*field->value);
            }
        }
        result.append(r);
    }
    return result;
}

uint64_t ToriiClient::subscribe_entity_updates(const Ref<DojoClause> &clause, const PackedStringArray &world_addresses, const Ref<DojoCallback> &callback) {
    std::vector<dojo::FieldElement> wa;
    for (int i = 0; i < world_addresses.size(); i++) {
        wa.push_back(world_addresses[i].utf8().get_data());
    }

    std::optional<std::shared_ptr<dojo::Clause>> c;
    if (clause.is_valid()) {
        c = std::make_shared<dojo::Clause>(clause->get_native());
    }

    return client->subscribe_entity_updates(c, wa, callback->create_entity_callback());
}

uint64_t ToriiClient::subscribe_event_updates(const Array &keys, const Ref<DojoCallback> &callback) {
    std::vector<std::shared_ptr<dojo::KeysClause>> k;
    for (int i = 0; i < keys.size(); i++) {
        Ref<KeysClause> kc = keys[i];
        if (kc.is_valid()) {
            // Extract the native KeysClause from the wrapper
            dojo::Clause native_clause_variant = kc->get_native();
            // The variant holds a shared_ptr<KeysClause> in the kKeys variant
            if (std::holds_alternative<dojo::Clause::kKeys>(native_clause_variant.get_variant())) {
                auto keys_variant = std::get<dojo::Clause::kKeys>(native_clause_variant.get_variant());
                k.push_back(keys_variant.clause);
            }
        }
    }
    return client->subscribe_event_updates(k, callback->create_event_callback());
}

uint64_t ToriiClient::subscribe_token_balance_updates(const PackedStringArray &contract_addresses, const PackedStringArray &account_addresses, const PackedStringArray &token_ids, const Ref<DojoCallback> &callback) {
    std::vector<dojo::FieldElement> ca;
    for (int i = 0; i < contract_addresses.size(); i++) ca.push_back(contract_addresses[i].utf8().get_data());

    std::vector<dojo::FieldElement> aa;
    for (int i = 0; i < account_addresses.size(); i++) aa.push_back(account_addresses[i].utf8().get_data());

    std::vector<dojo::U256> tids;
    for (int i = 0; i < token_ids.size(); i++) tids.push_back(token_ids[i].utf8().get_data());

    return client->subscribe_token_balance_updates(ca, aa, tids, callback->create_token_balance_callback());
}

uint64_t ToriiClient::subscribe_token_updates(const PackedStringArray &contract_addresses, const PackedStringArray &token_ids, const Ref<DojoCallback> &callback) {
    std::vector<dojo::FieldElement> ca;
    for (int i = 0; i < contract_addresses.size(); i++) ca.push_back(contract_addresses[i].utf8().get_data());

    std::vector<dojo::U256> tids;
    for (int i = 0; i < token_ids.size(); i++) tids.push_back(token_ids[i].utf8().get_data());

    return client->subscribe_token_updates(ca, tids, callback->create_token_callback());
}

uint64_t ToriiClient::subscribe_transaction_updates(const Dictionary &filter, const Ref<DojoCallback> &callback) {
    std::optional<std::shared_ptr<dojo::TransactionFilter>> f;
    if (!filter.is_empty()) {
        auto tf = std::make_shared<dojo::TransactionFilter>();

        if (filter.has("transaction_hashes")) {
            PackedStringArray th = filter["transaction_hashes"];
            for (int i = 0; i < th.size(); i++) tf->transaction_hashes.push_back(th[i].utf8().get_data());
        }
        if (filter.has("caller_addresses")) {
            PackedStringArray ca = filter["caller_addresses"];
            for (int i = 0; i < ca.size(); i++) tf->caller_addresses.push_back(ca[i].utf8().get_data());
        }
        if (filter.has("contract_addresses")) {
            PackedStringArray cta = filter["contract_addresses"];
            for (int i = 0; i < cta.size(); i++) tf->contract_addresses.push_back(cta[i].utf8().get_data());
        }
        if (filter.has("entrypoints")) {
            PackedStringArray ep = filter["entrypoints"];
            for (int i = 0; i < ep.size(); i++) tf->entrypoints.push_back(ep[i].utf8().get_data());
        }
        if (filter.has("model_selectors")) {
            PackedStringArray ms = filter["model_selectors"];
            for (int i = 0; i < ms.size(); i++) tf->model_selectors.push_back(ms[i].utf8().get_data());
        }
        if (filter.has("from_block")) {
            tf->from_block = filter["from_block"];
        }
        if (filter.has("to_block")) {
            tf->to_block = filter["to_block"];
        }

        f = tf;
    }
    return client->subscribe_transaction_updates(f, callback->create_transaction_callback());
}

TypedArray<Dictionary> ToriiClient::worlds(const PackedStringArray& world_addresses) {
    std::vector<dojo::FieldElement> wa;
    for (int i = 0; i < world_addresses.size(); i++) {
        wa.push_back(world_addresses[i].utf8().get_data());
    }

    std::vector<std::shared_ptr<dojo::World>> worlds = client->worlds(wa);
    TypedArray<Dictionary> result;
    for (const auto& w : worlds) {
        Dictionary world_dict;
        world_dict["world_address"] = String(w->world_address.c_str());

        Dictionary models;
        for (const auto& model : w->models) {
            if (model) {
                Dictionary m;
                m["namespace"] = String(model->namespace_.c_str());
                m["name"] = String(model->name.c_str());
                m["class_hash"] = String(model->class_hash.c_str());
                m["contract_address"] = String(model->contract_address.c_str());
                m["packed_size"] = model->packed_size;
                m["unpacked_size"] = model->unpacked_size;
                m["layout"] = String(model->layout.c_str());
                if (model->schema) {
                    m["schema"] = CallbackUtils::ty_to_variant(model->schema);
                }
                models[String(model->name.c_str())] = m;
            }
        }
        world_dict["models"] = models;

        result.append(world_dict);
    }
    return result;
}

std::shared_ptr<dojo::ToriiClient> ToriiClient::get_client() const {
    return client;
}

void ToriiClient::_bind_methods() {
    ClassDB::bind_static_method(get_class_static(), D_METHOD("create", "torii_url"), &ToriiClient::create);
    ClassDB::bind_static_method(get_class_static(), D_METHOD("create_with_config", "torii_url", "max_message_size"), &ToriiClient::create_with_config);

    ClassDB::bind_method(D_METHOD("achievements", "query"), &ToriiClient::achievements);
    ClassDB::bind_method(D_METHOD("activities", "query"), &ToriiClient::activities);
    ClassDB::bind_method(D_METHOD("aggregations", "query"), &ToriiClient::aggregations);
    ClassDB::bind_method(D_METHOD("contracts", "query"), &ToriiClient::contracts);
    ClassDB::bind_method(D_METHOD("controllers", "query"), &ToriiClient::controllers);
    ClassDB::bind_method(D_METHOD("entities", "query"), &ToriiClient::entities);
    ClassDB::bind_method(D_METHOD("event_messages", "query"), &ToriiClient::event_messages);
    ClassDB::bind_method(D_METHOD("player_achievements", "query"), &ToriiClient::player_achievements);
    ClassDB::bind_method(D_METHOD("search", "query"), &ToriiClient::search);
    ClassDB::bind_method(D_METHOD("starknet_events", "query"), &ToriiClient::starknet_events);
    ClassDB::bind_method(D_METHOD("token_balances", "query"), &ToriiClient::token_balances);
    ClassDB::bind_method(D_METHOD("token_contracts", "query"), &ToriiClient::token_contracts);
    ClassDB::bind_method(D_METHOD("token_transfers", "query"), &ToriiClient::token_transfers);
    ClassDB::bind_method(D_METHOD("tokens", "query"), &ToriiClient::tokens);
    ClassDB::bind_method(D_METHOD("transactions", "query"), &ToriiClient::transactions);

    ClassDB::bind_method(D_METHOD("cancel_subscription", "subscription_id"), &ToriiClient::cancel_subscription);
    ClassDB::bind_method(D_METHOD("publish_message", "message", "signature", "world_address"), &ToriiClient::publish_message);
    ClassDB::bind_method(D_METHOD("publish_message_batch", "messages"), &ToriiClient::publish_message_batch);
    ClassDB::bind_method(D_METHOD("sql", "query"), &ToriiClient::sql);

    ClassDB::bind_method(D_METHOD("subscribe_entity_updates", "clause", "world_addresses", "callback"), &ToriiClient::subscribe_entity_updates);
    ClassDB::bind_method(D_METHOD("subscribe_event_updates", "keys", "callback"), &ToriiClient::subscribe_event_updates);
    ClassDB::bind_method(D_METHOD("subscribe_token_balance_updates", "contract_addresses", "account_addresses", "token_ids", "callback"), &ToriiClient::subscribe_token_balance_updates);
    ClassDB::bind_method(D_METHOD("subscribe_token_updates", "contract_addresses", "token_ids", "callback"), &ToriiClient::subscribe_token_updates);
    ClassDB::bind_method(D_METHOD("subscribe_transaction_updates", "filter", "callback"), &ToriiClient::subscribe_transaction_updates);

    ClassDB::bind_method(D_METHOD("worlds", "world_addresses"), &ToriiClient::worlds);
}
