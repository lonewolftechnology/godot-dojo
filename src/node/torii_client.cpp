#include "node/torii_client.hpp"
#include <list>
#include "ref_counted/dojo_utilities/callback_utils.hpp"
#include "ref_counted/dojo_utilities/clauses/keys.hpp"
#include "tools/logger.h"
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

bool ToriiClient::connect(const String& torii_url, int64_t max_message_size) {
    try {
        CharString url = torii_url.utf8();
        if (max_message_size == -1) {
            client = dojo::ToriiClient::init(url.get_data());
            Logger::success("ToriiClient connected: ", torii_url);
        } else {
            client = dojo::ToriiClient::new_with_config(url.get_data(), (uint64_t)max_message_size);
            Logger::success("ToriiClient connected with config: ", torii_url);
        }
        return true;
    } catch (const std::exception& e) {
        Logger::error("ToriiClient connection failed: ", e.what());
        return false;
    }
}

ToriiClient* ToriiClient::create(const String& torii_url, int64_t max_message_size) {
    ToriiClient* node = memnew(ToriiClient);
    if (!node->connect(torii_url, max_message_size)) {
        memdelete(node);
        return nullptr;
    }
    return node;
}

Dictionary ToriiClient::achievements(const Ref<AchievementQuery>& query) {
    try {
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
        Logger::success("Achievements query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Achievements query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::activities(const Ref<ActivityQuery>& query) {
    try {
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
        Logger::success("Activities query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Activities query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::aggregations(const Ref<AggregationQuery>& query) {
    try {
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
        Logger::success("Aggregations query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Aggregations query failed: ", e.what());
        return Dictionary();
    }
}

TypedArray<Dictionary> ToriiClient::contracts(const Ref<ContractQuery>& query) {
    try {
        std::vector<std::shared_ptr<dojo::Contract>> contracts = client->contracts(query->get_native());
        TypedArray<Dictionary> result;
        for (const auto& contract : contracts) {
            if (contract) {
                result.append(CallbackUtils::contract_to_dict(*contract));
            }
        }
        Logger::success("Contracts query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Contracts query failed: ", e.what());
        return TypedArray<Dictionary>();
    }
}

Dictionary ToriiClient::controllers(const Ref<ControllerQuery>& query) {
    try {
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
        Logger::success("Controllers query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Controllers query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::entities(const Ref<DojoQuery>& query) {
    try {
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
        Logger::success("Entities query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Entities query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::event_messages(const Ref<DojoQuery>& query) {
    try {
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
        Logger::success("Event messages query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Event messages query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::player_achievements(const Ref<PlayerAchievementQuery>& query) {
    try {
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
        Logger::success("Player achievements query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Player achievements query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::search(const Ref<SearchQuery>& query) {
    try {
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
        Logger::success("Search executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Search failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::starknet_events(const Ref<EventQuery>& query) {
    try {
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
        Logger::success("Starknet events query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Starknet events query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::token_balances(const Ref<TokenBalanceQuery>& query) {
    try {
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
        Logger::success("Token balances query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Token balances query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::token_contracts(const Ref<TokenContractQuery>& query) {
    try {
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
        Logger::success("Token contracts query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Token contracts query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::token_transfers(const Ref<TokenTransferQuery>& query) {
    try {
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
        Logger::success("Token transfers query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Token transfers query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::tokens(const Ref<TokenQuery>& query) {
    try {
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
        Logger::success("Tokens query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Tokens query failed: ", e.what());
        return Dictionary();
    }
}

Dictionary ToriiClient::transactions(const Ref<TransactionQuery>& query) {
    try {
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
        Logger::success("Transactions query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Transactions query failed: ", e.what());
        return Dictionary();
    }
}

void ToriiClient::cancel_subscription(uint64_t subscription_id) {
    try {
        client->cancel_subscription(subscription_id);
        Logger::success("Subscription cancelled: ", subscription_id);
    } catch (const std::exception& e) {
        Logger::error("Cancel subscription failed: ", e.what());
    }
}

String ToriiClient::publish_message(const String& message, const PackedStringArray& signature, const String& world_address) {
    try {
        dojo::Message msg;
        CharString msg_str = message.utf8();
        msg.message = msg_str.get_data();
        CharString wa_str = world_address.utf8();
        msg.world_address = wa_str.get_data();

        std::vector<dojo::FieldElement> sig_vec;
        std::vector<std::string> sig_store;
        sig_store.reserve(signature.size());
        for (int i = 0; i < signature.size(); i++) {
            sig_store.push_back(signature[i].utf8().get_data());
        }
        sig_vec.reserve(sig_store.size());
        for (const auto& s : sig_store) sig_vec.push_back(s.c_str());
        msg.signature = sig_vec;

        String res = String(client->publish_message(msg).c_str());
        Logger::success("Message published successfully: ", res);
        return res;
    } catch (const std::exception& e) {
        Logger::error("Publish message failed: ", e.what());
        return String();
    }
}

PackedStringArray ToriiClient::publish_message_batch(const Array& messages) {
    try {
        std::vector<std::shared_ptr<dojo::Message>> msgs;
        std::list<std::string> keep_alive;

        for (int i = 0; i < messages.size(); i++) {
            Dictionary m = messages[i];
            auto msg = std::make_shared<dojo::Message>();
            keep_alive.push_back(String(m["message"]).utf8().get_data());
            msg->message = keep_alive.back().c_str();
            keep_alive.push_back(String(m["world_address"]).utf8().get_data());
            msg->world_address = keep_alive.back().c_str();

            PackedStringArray sig = m["signature"];
            std::vector<dojo::FieldElement> sig_vec;
            for (int j = 0; j < sig.size(); j++) {
                keep_alive.push_back(sig[j].utf8().get_data());
                sig_vec.push_back(keep_alive.back().c_str());
            }
            msg->signature = sig_vec;

            msgs.push_back(msg);
        }

        std::vector<std::string> result = client->publish_message_batch(msgs);
        PackedStringArray ret;
        for (const auto& r : result) {
            ret.append(String(r.c_str()));
        }
        Logger::success("Batch messages published successfully.");
        return ret;
    } catch (const std::exception& e) {
        Logger::error("Publish message batch failed: ", e.what());
        return PackedStringArray();
    }
}

TypedArray<Dictionary> ToriiClient::sql(const String& query) {
    try {
        CharString q = query.utf8();
        std::vector<std::shared_ptr<dojo::SqlRow>> rows = client->sql(q.get_data());
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
        Logger::success("SQL query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("SQL query failed: ", e.what());
        return TypedArray<Dictionary>();
    }
}

uint64_t ToriiClient::subscribe_entity_updates(const Ref<DojoClause> &clause, const PackedStringArray &world_addresses, const Ref<DojoCallback> &callback) {
    try {
        std::vector<dojo::FieldElement> wa;
        std::vector<std::string> wa_store;
        wa_store.reserve(world_addresses.size());
        for (int i = 0; i < world_addresses.size(); i++) {
            wa_store.push_back(world_addresses[i].utf8().get_data());
        }
        wa.reserve(wa_store.size());
        for (const auto& s : wa_store) wa.push_back(s.c_str());

        std::optional<std::shared_ptr<dojo::Clause>> c;
        if (clause.is_valid()) {
            c = std::make_shared<dojo::Clause>(clause->get_native());
        }

        uint64_t id = client->subscribe_entity_updates(c, wa, callback->create_entity_callback());
        Logger::success("Subscribed to entity updates. ID: ", id);
        return id;
    } catch (const std::exception& e) {
        Logger::error("Subscribe entity updates failed: ", e.what());
        return 0;
    }
}

uint64_t ToriiClient::subscribe_event_updates(const Array &keys, const Ref<DojoCallback> &callback) {
    try {
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
        uint64_t id = client->subscribe_event_updates(k, callback->create_event_callback());
        Logger::success("Subscribed to event updates. ID: ", id);
        return id;
    } catch (const std::exception& e) {
        Logger::error("Subscribe event updates failed: ", e.what());
        return 0;
    }
}

uint64_t ToriiClient::subscribe_token_balance_updates(const PackedStringArray &contract_addresses, const PackedStringArray &account_addresses, const PackedStringArray &token_ids, const Ref<DojoCallback> &callback) {
    try {
        std::vector<dojo::FieldElement> ca;
        std::vector<std::string> ca_store;
        ca_store.reserve(contract_addresses.size());
        for (int i = 0; i < contract_addresses.size(); i++) {
            ca_store.push_back(contract_addresses[i].utf8().get_data());
        }
        ca.reserve(ca_store.size());
        for (const auto& s : ca_store) ca.push_back(s.c_str());

        std::vector<dojo::FieldElement> aa;
        std::vector<std::string> aa_store;
        aa_store.reserve(account_addresses.size());
        for (int i = 0; i < account_addresses.size(); i++) {
            aa_store.push_back(account_addresses[i].utf8().get_data());
        }
        aa.reserve(aa_store.size());
        for (const auto& s : aa_store) aa.push_back(s.c_str());

        std::vector<dojo::U256> tids;
        std::vector<std::string> tids_store;
        tids_store.reserve(token_ids.size());
        for (int i = 0; i < token_ids.size(); i++) {
            tids_store.push_back(token_ids[i].utf8().get_data());
        }
        tids.reserve(tids_store.size());
        for (const auto& s : tids_store) tids.push_back(s.c_str());

        uint64_t id = client->subscribe_token_balance_updates(ca, aa, tids, callback->create_token_balance_callback());
        Logger::success("Subscribed to token balance updates. ID: ", id);
        return id;
    } catch (const std::exception& e) {
        Logger::error("Subscribe token balance updates failed: ", e.what());
        return 0;
    }
}

uint64_t ToriiClient::subscribe_token_updates(const PackedStringArray &contract_addresses, const PackedStringArray &token_ids, const Ref<DojoCallback> &callback) {
    try {
        std::vector<dojo::FieldElement> ca;
        std::vector<std::string> ca_store;
        ca_store.reserve(contract_addresses.size());
        for (int i = 0; i < contract_addresses.size(); i++) {
            ca_store.push_back(contract_addresses[i].utf8().get_data());
        }
        ca.reserve(ca_store.size());
        for (const auto& s : ca_store) ca.push_back(s.c_str());

        std::vector<dojo::U256> tids;
        std::vector<std::string> tids_store;
        tids_store.reserve(token_ids.size());
        for (int i = 0; i < token_ids.size(); i++) {
            tids_store.push_back(token_ids[i].utf8().get_data());
        }
        tids.reserve(tids_store.size());
        for (const auto& s : tids_store) tids.push_back(s.c_str());

        uint64_t id = client->subscribe_token_updates(ca, tids, callback->create_token_callback());
        Logger::success("Subscribed to token updates. ID: ", id);
        return id;
    } catch (const std::exception& e) {
        Logger::error("Subscribe token updates failed: ", e.what());
        return 0;
    }
}

uint64_t ToriiClient::subscribe_transaction_updates(const Dictionary &filter, const Ref<DojoCallback> &callback) {
    try {
        std::optional<std::shared_ptr<dojo::TransactionFilter>> f;
        std::list<std::string> keep_alive;
        if (!filter.is_empty()) {
            auto tf = std::make_shared<dojo::TransactionFilter>();

            if (filter.has("transaction_hashes")) {
                PackedStringArray th = filter["transaction_hashes"];
                for (int i = 0; i < th.size(); i++) {
                    keep_alive.push_back(th[i].utf8().get_data());
                    tf->transaction_hashes.push_back(keep_alive.back().c_str());
                }
            }
            if (filter.has("caller_addresses")) {
                PackedStringArray ca = filter["caller_addresses"];
                for (int i = 0; i < ca.size(); i++) {
                    keep_alive.push_back(ca[i].utf8().get_data());
                    tf->caller_addresses.push_back(keep_alive.back().c_str());
                }
            }
            if (filter.has("contract_addresses")) {
                PackedStringArray cta = filter["contract_addresses"];
                for (int i = 0; i < cta.size(); i++) {
                    keep_alive.push_back(cta[i].utf8().get_data());
                    tf->contract_addresses.push_back(keep_alive.back().c_str());
                }
            }
            if (filter.has("entrypoints")) {
                PackedStringArray ep = filter["entrypoints"];
                for (int i = 0; i < ep.size(); i++) {
                    keep_alive.push_back(ep[i].utf8().get_data());
                    tf->entrypoints.push_back(keep_alive.back().c_str());
                }
            }
            if (filter.has("model_selectors")) {
                PackedStringArray ms = filter["model_selectors"];
                for (int i = 0; i < ms.size(); i++) {
                    keep_alive.push_back(ms[i].utf8().get_data());
                    tf->model_selectors.push_back(keep_alive.back().c_str());
                }
            }
            if (filter.has("from_block")) {
                tf->from_block = filter["from_block"];
            }
            if (filter.has("to_block")) {
                tf->to_block = filter["to_block"];
            }

            f = tf;
        }
        uint64_t id = client->subscribe_transaction_updates(f, callback->create_transaction_callback());
        Logger::success("Subscribed to transaction updates. ID: ", id);
        return id;
    } catch (const std::exception& e) {
        Logger::error("Subscribe transaction updates failed: ", e.what());
        return 0;
    }
}

TypedArray<Dictionary> ToriiClient::worlds(const PackedStringArray& world_addresses) {
    try {
        std::vector<dojo::FieldElement> wa;
        std::vector<std::string> wa_store;
        wa_store.reserve(world_addresses.size());
        for (int i = 0; i < world_addresses.size(); i++) {
            wa_store.push_back(world_addresses[i].utf8().get_data());
        }
        wa.reserve(wa_store.size());
        for (const auto& s : wa_store) wa.push_back(s.c_str());

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
        Logger::success("Worlds query executed successfully.");
        return result;
    } catch (const std::exception& e) {
        Logger::error("Worlds query failed: ", e.what());
        return TypedArray<Dictionary>();
    }
}

std::shared_ptr<dojo::ToriiClient> ToriiClient::get_client() const {
    return client;
}

void ToriiClient::_bind_methods() {
    ClassDB::bind_static_method(get_class_static(), D_METHOD("create", "torii_url", "max_message_size"), &ToriiClient::create, DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("connect", "torii_url", "max_message_size"), &ToriiClient::connect, DEFVAL(-1));

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
