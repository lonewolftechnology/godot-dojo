//
// Created by hazel on 27/05/25.
//

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"

#include "classes/torii_client.h"

#include "ref_counted/options/option_u256.h"
#include "tools/logger.h"
#include "variant/ty/dojo_array.h"
#include "tools/dojo_helper.h"
#include "types/big_int.h"

ToriiClient *ToriiClient::singleton = nullptr;

ToriiClient::ToriiClient() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    singleton = this;
    client = nullptr;
    is_connected = false;
    Logger::debug_extra("ToriiClient", "CONSTRUCTOR CALLED");
}

ToriiClient::~ToriiClient() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    disconnect_client(false);
    singleton = nullptr;
    Logger::debug_extra("ToriiClient", "DESTRUCTOR CALLED");
}

ToriiClient *ToriiClient::get_singleton() {
    return singleton;
}

bool ToriiClient::create_client(const String &p_url, const TypedArray<String> &p_addresses) {
    if (p_addresses.is_empty()) {
        world_addresses = get_worlds();
    } else {
        world_addresses = p_addresses;
    }

    if (p_url.is_empty()) {
        torii_url = get_url();
    } else {
        torii_url = p_url;
    }

#ifdef WEB_ENABLED
    if (is_web_client_initialized) {
        Logger::info("Web client already initialized.");
        call_deferred("emit_signal", "client_connected", true);
        return true;
    }

    Dictionary opts;
    opts["toriiUrl"] = torii_url;
    opts["worldAddress"] = world_address;

    Logger::debug_extra("ToriiClient", "Creating Torii client...");

    Callable callback = Callable(this, "_on_client_created");
    Array args;
    args.push_back(opts);
    DojoBridge::call_async("toriiclient_new", args, callback);
    return true; // The actual connection status will be updated in the callback.
#else
    disconnect_client(true);

    Logger::info("Creating Torii client...");
    Logger::info("URL: ", torii_url);

    DOJO::ResultToriiClient resClient = DOJO::client_new(
        torii_url.utf8().get_data()
    );

    if (resClient.tag == DOJO::ErrToriiClient) {
        Logger::error(String("Failed to create Torii client: ") + GET_DOJO_ERROR(resClient));
        call_deferred("emit_signal", "client_connected", false);
        return false;
    }

    client = resClient.ok;
    is_connected = true;

    DOJO::client_set_logger(client, [](const char *msg) {
        Logger::info("ToriiClient: ", msg);
    });

    Logger::success("Torii client created successfully");
    call_deferred("emit_signal", "client_connected", true);
    return true;
#endif
}

void ToriiClient::disconnect_client(bool send_signal = true) {
#ifdef WEB_ENABLED
    is_web_client_initialized = false;
    is_connected = false;
    if (send_signal) {
        call_deferred("emit_signal", "client_disconnected");
    }
#else
    if (client != nullptr) {
        cancel_all_subscriptions();
        // DOJO::client_free(client);

        client = nullptr;
        is_connected = false;
        if (send_signal) {
            call_deferred("emit_signal", "client_disconnected");
        }
        Logger::info("Torii client disconnected");
    }
#endif
}

bool ToriiClient::is_client_connected() const {
#ifdef WEB_ENABLED
    return is_connected;
#else
    return is_connected && client != nullptr;
#endif
}

bool ToriiClient::is_callable_valid() const {
    return logger_callback.is_valid();
}

void ToriiClient::callable_call(const char *msg) const {
    if (is_callable_valid()) {
        singleton->call_deferred("logger_callback", String(msg));
    }
}

TypedArray<Dictionary> ToriiClient::get_worlds_metadata(const TypedArray<String> &p_addresses) {
#ifdef WEB_ENABLED
    if (!is_client_connected()) {
        Logger::error("Web client not connected");
        return Logger::error_dict("Web client not connected");
    }
    Logger::info("Not implemented");
    return Dictionary(); // Return empty, result will be in a signal.
#else
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return {};
    }
    if (p_addresses.is_empty()) {
        world_addresses = get_worlds();
    }
    DOJO::CArrayFieldElement worlds_array = DojoArray::CFieldElementArrayHelper(p_addresses).c_array;
    DOJO::ResultCArrayWorld resMetadata = DOJO::client_worlds(client, worlds_array.data, worlds_array.data_len);

    if (resMetadata.tag == DOJO::ErrCArrayWorld) {
        Logger::error("Failed to get worlds metadata: "), GET_DOJO_ERROR(resMetadata);
        return {};
    }

    DOJO::CArrayWorld worlds = GET_DOJO_OK(resMetadata);
    TypedArray<Dictionary> result = DojoArray::CArrayWorldToVariant(worlds);

    Logger::success_extra("ToriiClient", "Worlds obtained");
    return result;
#endif
}

bool ToriiClient::refresh_metadata(const TypedArray<String>& p_world_addresses ) {
    TypedArray<Dictionary> metadata = get_worlds_metadata(p_world_addresses);
    if (!metadata.is_empty()) {
        call_deferred("emit_signal", "metadata_updated", metadata);
        return true;
    }
    return false;
}

TypedArray<Dictionary> ToriiClient::get_entities(const Ref<DojoQuery> &query) {
#ifdef WEB_ENABLED
    Logger::info("Getting entities (Web)...");
    Logger::info("Not implemented");
    return {}; // Return empty, result will be in a signal.
#endif

    Logger::info("Getting entities...");
    if (!_is_ready_for_query(query)) {
        return {};
    }
    DOJO::Query *native_query_ptr = static_cast<DOJO::Query *>(query->get_native_query());

    DOJO::ResultPageEntity result_page_entity = DOJO::client_entities(client, *native_query_ptr);

    if (result_page_entity.tag == DOJO::ErrPageEntity) {
        Logger::error(String("Failed to get entities: ") + GET_DOJO_ERROR(result_page_entity));
        return {};
    }

    DOJO::PageEntity page_entities = result_page_entity.ok;
    TypedArray<Dictionary> result = DojoArray::CArrayEntityToVariant(page_entities.items);

    Logger::success_extra("ToriiClient", "Entities obtained: ", result.size());
    return result;
}

TypedArray<Dictionary> ToriiClient::get_controllers(Ref<DojoControllerQuery> query) {
    if (!_is_ready_for_query(query)) {
        return {};
    }
    DOJO::ControllerQuery *controller_query_ptr = static_cast<DOJO::ControllerQuery *>(query->get_native_query());

    DOJO::ResultPageController res_controllers = DOJO::client_controllers(client, *controller_query_ptr);

    // Cleanup
    for (size_t i = 0; i < controller_query_ptr->usernames.data_len; ++i) {
        delete[] (char *) controller_query_ptr->usernames.data[i];
    }
    delete[] controller_query_ptr->usernames.data;
    delete[] controller_query_ptr->contract_addresses.data;
    if (controller_query_ptr->pagination.cursor.tag == DOJO::COptionc_char_Tag::Somec_char) {
        delete[] controller_query_ptr->pagination.cursor.some;
    }
    delete controller_query_ptr;

    if (res_controllers.tag == DOJO::ErrPageController) {
        Logger::error(String("Failed to get controllers: ") + GET_DOJO_ERROR(res_controllers));
        DOJO::error_free(&res_controllers.err);
        return {};
    }

    return DojoArray::PageControllerToVariant(GET_DOJO_OK(res_controllers));
}


Dictionary ToriiClient::get_controller_info(const String &controller_address) {
    Ref<DojoControllerQuery> query;
    query.instantiate();
    TypedArray<String> addresses;
    addresses.push_back(controller_address);
    query->set_contract_addresses(addresses);

    TypedArray<Dictionary> controllers = get_controllers(query);

    if (!controllers.is_empty()) {
        return controllers[0];
    }

    return Logger::error_dict("Controller not found");
}

TypedArray<Dictionary> ToriiClient::get_tokens(const Ref<DojoTokenQuery> &query) const {
    Logger::info("Getting tokens...");
    if (!_is_ready_for_query(query)) {
        return {};
    }
    DOJO::TokenQuery *token_query_ptr = static_cast<DOJO::TokenQuery *>(query->get_native_query());

    DOJO::ResultPageToken result = DOJO::client_tokens(
        client,
        *token_query_ptr
    );

    // Cleanup
    delete[] token_query_ptr->contract_addresses.data;
    delete[] token_query_ptr->token_ids.data;
    if (token_query_ptr->pagination.cursor.tag == DOJO::COptionc_char_Tag::Somec_char) {
        delete[] token_query_ptr->pagination.cursor.some;
    }
    delete token_query_ptr;

    if (result.tag == DOJO::ErrPageToken) {
        Logger::error(String("Error getting tokens: ") + GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageToken tokens = GET_DOJO_OK(result);
    TypedArray<Dictionary> result_array = DojoArray::CArrayTokenToVariant(tokens.items);

    Logger::success_extra("ToriiClient", "Tokens obtained: ", result_array.size());
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_token_balances(const Ref<DojoTokenBalanceQuery> &query) const {
    Logger::info("Getting token balances...");
    if (!_is_ready_for_query(query)) {
        return {};
    }
    DOJO::TokenBalanceQuery *balance_query_ptr = static_cast<DOJO::TokenBalanceQuery *>(query->get_native_query());

    DOJO::ResultPageTokenBalance result = DOJO::client_token_balances(
        client,
        *balance_query_ptr
    );

    if (result.tag == DOJO::ErrPageTokenBalance) {
        Logger::error("Error getting token balances: ", GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageTokenBalance balances = result.ok;
    TypedArray<Dictionary> result_array = DojoArray::CArrayTokenBalanceToVariant(balances.items);

    Logger::success_extra("ToriiClient", "Token Balances obtained: ", result_array.size());
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_token_transfers(const Ref<DojoTokenTransferQuery> &query) const {
    Logger::info("Getting token Tranfsers");
    if (!_is_ready_for_query(query)) {
        return {};
    }
    DOJO::TokenTransferQuery *token_transfer_query_ptr = static_cast<DOJO::TokenTransferQuery *>(query->
        get_native_query());

    DOJO::ResultPageTokenTransfer result = DOJO::client_token_transfers(client, *token_transfer_query_ptr);

    if (result.tag == DOJO::ErrPageTokenTransfer) {
        Logger::error("Error getting token transfers: ", GET_DOJO_ERROR(result));
        return {};
    }
    DOJO::PageTokenTransfer transfers = result.ok;
    TypedArray<Dictionary> result_array = DojoArray::CArrayTokenTransferToVariant(transfers.items);

    Logger::success_extra("ToriiClient", "Token Transfers obtained: ", result_array.size());
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_token_collections(const Ref<DojoContractQuery> &query) const {
    Logger::info("Getting token collections...");
    if (!_is_ready_for_query(query)) {
        return {};
    }
    DOJO::TokenContractQuery *balance_query_ptr = static_cast<DOJO::TokenContractQuery *>(query->get_native_query());

    DOJO::ResultPageTokenContract result = DOJO::client_token_contracts(
        client,
        *balance_query_ptr
    );

    if (result.tag == DOJO::ErrPageTokenContract) {
        Logger::error("Error getting token collections: ", GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageTokenContract collections = result.ok;
    TypedArray<Dictionary> result_array = DojoArray::CArrayTokenContractToVariant(collections.items);

    Logger::success_extra("ToriiClient", "Token collections obtained: ", result_array.size());
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_token_info(const Ref<DojoTokenQuery> &query) const {
    Logger::info("Getting token info");
    if (!_is_ready_for_query(query)) {
        return {};
    }

    DOJO::TokenQuery *token_query = static_cast<DOJO::TokenQuery *>(query->get_native_query());

    DOJO::ResultPageToken result = DOJO::client_tokens(
        client,
        *token_query
    );

    if (result.tag == DOJO::ErrPageToken) {
        Logger::error(String("Error getting token info: ") + GET_DOJO_ERROR(result));
        return {};
    }

    // Convert the result to a Dictionary
    DOJO::PageToken tokens = result.ok;
    TypedArray<Dictionary> result_array = DojoArray::CArrayTokenToVariant(tokens.items);
    Logger::success_extra("ToriiClient", "Token info obtained:");
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_aggregations(const Ref<DojoAggregationQuery> &query) const {
    Logger::info("Getting Aggregations...");
    if (!_is_ready_for_query(query)) {
        return {};
    }
    DOJO::AggregationQuery *aggregation_query_ptr = static_cast<DOJO::AggregationQuery *>(query->get_native_query());

    DOJO::ResultPageAggregationEntry result = DOJO::client_aggregations(
        client,
        *aggregation_query_ptr
    );

    if (result.tag == DOJO::ErrPageAggregationEntry) {
        Logger::error("Error getting aggregations: ", GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageAggregationEntry aggregations = result.ok;
    TypedArray<Dictionary> result_array = DojoArray::CArrayAggregationEntryToVariant(aggregations.items);

    Logger::success_extra("ToriiClient", "Aggregations obtained: ", result_array.size());

    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_activities(const Ref<DojoActivityQuery> &query) const {
    Logger::info("Getting Activities...");
    if (!_is_ready_for_query(query)) {
        return {};
    }
    DOJO::ActivityQuery *activity_query_ptr = static_cast<DOJO::ActivityQuery *>(query->get_native_query());

    DOJO::ResultPageActivity result = DOJO::client_activities(
        client,
        *activity_query_ptr
    );

    if (result.tag == DOJO::ErrPageActivity) {
        Logger::error("Error getting aggregations: ", GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageActivity activities = result.ok;
    TypedArray<Dictionary> result_array = DojoArray::CArrayActivityToVariant(activities.items);

    Logger::success_extra("ToriiClient", "Activities obtained: ", result_array.size());
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_achivements(const Ref<DojoAchievementQuery> &query) const {
    Logger::info("Getting Achievements");
    if (!_is_ready_for_query(query)) {
        return {};
    }

    DOJO::AchievementQuery *achivements_query_ptr = static_cast<DOJO::AchievementQuery *>(query->get_native_query());
    DOJO::ResultPageAchievement result = DOJO::client_achievements(client, *achivements_query_ptr);

    if (result.tag == DOJO::ErrPageAchievement) {
        Logger::error("Failed getting achievements: ", GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageAchievement achievements = result.ok;
    TypedArray<Dictionary> result_array = DojoArray::CArrayAchievementToVariant(achievements.items);

    Logger::success_extra("ToriiClient", "Achievements obtained: ", result_array.size());
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_player_achivements(const Ref<DojoPlayerAchievementQuery> &query) const {
    Logger::info("Getting Player Achievements");
    if (!_is_ready_for_query(query)) {
        return {};
    }

    DOJO::PlayerAchievementQuery* player_achievement_query_ptr = static_cast<DOJO::PlayerAchievementQuery *>(query->get_native_query());;
    DOJO::ResultPagePlayerAchievementEntry result = DOJO::client_player_achievements(client, *player_achievement_query_ptr);

    if (result.tag == DOJO::ErrPagePlayerAchievementEntry) {
        Logger::error("Failed getting player achievements: ", GET_DOJO_ERROR(result));
    }

    DOJO::PagePlayerAchievementEntry player_achievements = result.ok;
    TypedArray<Dictionary> result_array = DojoArray::CArrayPlayerAchievementEntryToVariant(player_achievements.items);

    Logger::success_extra("ToriiClient", "Player Achievements obtained: ", result_array.size());
    return result_array;
}


void ToriiClient::cancel_all_subscriptions() {
    Logger::info("Cancelling all subscriptions");

    for (int i = 0; i < subscriptions.size(); i++) {
        Ref<DojoSubscription> subscription = subscriptions[i];
        if (subscription.is_valid()) {
            subscription->cancel();
        }
    }

    subscriptions.clear();

    Logger::success_extra("ToriiClient", "All subscriptions cancelled");
}

bool ToriiClient::publish_message(const String &message_data, const Array &signature_felts) {
    Logger::info("publish_message is experimental");
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return false;
    }

    DOJO::FieldElement sign_key = DOJO::signing_key_new();
    DOJO::FieldElement hash = DOJO::verifying_key_new(sign_key);
    DOJO::ResultSignature resSig = DOJO::signing_key_sign(sign_key, hash);
    if (resSig.tag == DOJO::ErrSignature) {
        Logger::error(String("Failed to sign message: ") + GET_DOJO_ERROR(resSig));
        return false;
    }
    Logger::success_extra("Message", message_data);
    DOJO::Signature signature = GET_DOJO_OK(resSig);
    DOJO::FieldElement cfelts[2];
    cfelts[0] = signature.r;
    cfelts[1] = signature.s;


    std::vector<DOJO::FieldElement> felts = FieldElement::create_array(signature_felts);
    DOJO::Message message = {
        message_data.utf8().get_data(),
        {
            cfelts,
            2
        }
    };

    DOJO::Resultc_char result = DOJO::client_publish_message(
        client,
        message
    );

    if (result.tag == DOJO::Errc_char) {
        Logger::error(String("Failed to publish message: ") + GET_DOJO_ERROR(result));
        return false;
    }

    String result_message = GET_DOJO_OK(result);
    Logger::success_extra("ToriiClient", "Message published: ", result_message);
    call_deferred("emit_signal", "message_published", result_message);

    return true;
}

bool ToriiClient::publish_typed_message(const Dictionary &typed_data, const Array &signature_felts) {
    String json_data = JSON::stringify(typed_data);
    return publish_message(json_data, signature_felts);
}

void logger_callback_wrapper(const char *msg) {
    Logger::debug_extra("Torii LOG", msg);
    ToriiClient *_singleton = ToriiClient::get_singleton();
    if (_singleton->is_callable_valid()) {
        _singleton->callable_call(msg);
    }
}

void ToriiClient::set_logger_callback(const Callable &p_logger_callback) {
    {
        if (!is_client_connected()) return;
        logger_callback = p_logger_callback;
        DOJO::client_set_logger(client, logger_callback_wrapper);
    }
}

TypedArray<String> ToriiClient::get_worlds() {
    if (world_addresses.is_empty()) {
        Logger::debug_extra("ToriiClient", "No worlds addresses were setted, fetching ProjectSettings");
        TypedArray<String> setting_worlds = DojoHelpers::get_torii_setting("worlds");
        if (setting_worlds.is_empty()) {
            Logger::warning("ToriiClient", " Could find world addresses");
            return {};
        }
        return setting_worlds;
    }
    return world_addresses;
}

String ToriiClient::get_url() {
    if (torii_url.is_empty()) {
        Logger::debug_extra("ToriiClient", "No Torii Url were setted, fetching ProjectSettings");
        String p_url = DojoHelpers::get_torii_setting("torii_url");
        if (p_url.is_empty()) {
            Logger::error("Torii Url not found");
            call_deferred("emit_signal", "client_connected", false);
            return "";
        }
        return p_url;
    }
    return torii_url;
}

Dictionary ToriiClient::get_client_info() const {
    Dictionary info = {};
    info["connected"] = is_connected;
    info["torii_url"] = torii_url;
    info["worlds"] = world_addresses;
    return info;
}

Dictionary ToriiClient::get_connection_status() const {
    Dictionary status = {};
    status["connected"] = is_connected;
    status["client_exists"] = (client != nullptr);
    status["torii_url"] = torii_url;
    status["worlds"] = world_addresses;
    return status;
}

// Static subscription callbacks

void entity_state_update_callback_wrapper(DOJO::Entity entity) {
    Logger::info("entity_state_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_entity_state_update_callback.is_valid()) {
        Dictionary entity_data;
        entity_data["hashed_keys"] = FieldElement::get_as_string(&entity.hashed_keys);
        entity_data["models"] = DojoArray::CArrayStructToVariant(entity.models);
        entity_data["created_at"] = static_cast<int64_t>(entity.created_at);
        entity_data["updated_at"] = static_cast<int64_t>(entity.updated_at);
        entity_data["executed_at"] = static_cast<int64_t>(entity.executed_at);
        singleton->on_entity_state_update_callback.call_deferred(entity_data);
    }
}

void event_message_update_callback_wrapper(DOJO::Entity entity) {
    Logger::info("event_message_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_event_message_update_callback.is_valid()) {
        Dictionary message_data;
        message_data["hashed_keys"] = FieldElement::get_as_string(&entity.hashed_keys);
        message_data["models"] = DojoArray::CArrayStructToVariant(entity.models);
        message_data["created_at"] = static_cast<int64_t>(entity.created_at);
        message_data["updated_at"] = static_cast<int64_t>(entity.updated_at);
        message_data["executed_at"] = static_cast<int64_t>(entity.executed_at);
        singleton->on_event_message_update_callback.call_deferred(message_data);
    }
}

void starknet_event_callback_wrapper(DOJO::Event event) {
    Logger::info("starknet_event_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_starknet_event_callback.is_valid()) {
        Dictionary event_data;
        event_data["keys"] = DojoArray::CArrayFieldElementToVariant(event.keys);
        event_data["data"] = DojoArray::CArrayFieldElementToVariant(event.data);
        event_data["transaction_hash"] = FieldElement::get_as_string(&event.transaction_hash);
        singleton->on_starknet_event_callback.call_deferred(event_data);
    }
}

void transaction_callback_wrapper(DOJO::Transaction transaction) {
    Logger::info("transaction_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_transaction_callback.is_valid()) {
        Dictionary tx_dict;
        tx_dict["transaction_hash"] = FieldElement::get_as_string(&transaction.transaction_hash);
        tx_dict["sender_address"] = FieldElement::get_as_string(&transaction.sender_address);
        tx_dict["calldata"] = DojoArray::CArrayFieldElementToVariant(transaction.calldata);
        tx_dict["max_fee"] = FieldElement::get_as_string(&transaction.max_fee);
        tx_dict["signature"] = DojoArray::CArrayFieldElementToVariant(transaction.signature);
        tx_dict["nonce"] = FieldElement::get_as_string(&transaction.nonce);
        tx_dict["block_number"] = static_cast<int64_t>(transaction.block_number);
        tx_dict["transaction_type"] = String(transaction.transaction_type);
        tx_dict["block_timestamp"] = static_cast<int64_t>(transaction.block_timestamp);

        TypedArray<Dictionary> calls_array;
        for (size_t i = 0; i < transaction.calls.data_len; ++i) {
            DOJO::TransactionCall call = transaction.calls.data[i];
            Dictionary call_dict;
            call_dict["contract_address"] = FieldElement::get_as_string(&call.contract_address);
            call_dict["entrypoint"] = String(call.entrypoint);
            call_dict["calldata"] = DojoArray::CArrayFieldElementToVariant(call.calldata);
            call_dict["call_type"] = static_cast<int>(call.call_type);
            call_dict["caller_address"] = FieldElement::get_as_string(&call.caller_address);
            calls_array.push_back(call_dict);
        }
        tx_dict["calls"] = calls_array;

        tx_dict["unique_models"] = DojoArray::CArrayFieldElementToVariant(transaction.unique_models);
        singleton->on_transaction_callback.call_deferred(tx_dict);
    }
}

void token_update_callback_wrapper(DOJO::Token token) {
    Logger::info("token_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_token_update_callback.is_valid()) {
        Dictionary token_dict;
        token_dict["contract_address"] = FieldElement::get_as_string(&token.contract_address);
        if (token.token_id.tag == DOJO::SomeU256) {
            token_dict["token_id"] = memnew(U256(token.token_id.some));
        } else {
            token_dict["token_id"] = Variant();
        }
        token_dict["name"] = String(token.name);
        token_dict["symbol"] = String(token.symbol);
        token_dict["decimals"] = token.decimals;
        if (token.metadata) {
            token_dict["metadata"] = String(token.metadata);
        } else {
            token_dict["metadata"] = Variant();
        }
        singleton->on_token_update_callback.call_deferred(token_dict);
    }
}

void contract_update_callback_wrapper(DOJO::Contract contract_update) {
    Logger::info("contract_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_contract_update_callback.is_valid()) {
        Dictionary update_dict;
        update_dict["contract_address"] = FieldElement::get_as_string(&contract_update.contract_address);
        update_dict["contract_type"] = static_cast<int>(contract_update.contract_type);
        if (contract_update.head.tag == DOJO::Someu64) {
            update_dict["head"] = static_cast<int64_t>(contract_update.head.some);
        }
        if (contract_update.tps.tag == DOJO::Someu64) {
            update_dict["tps"] = static_cast<int64_t>(contract_update.tps.some);
        }
        if (contract_update.last_block_timestamp.tag == DOJO::Someu64) {
            update_dict["last_block_timestamp"] = static_cast<int64_t>(contract_update.last_block_timestamp.some);
        }
        if (contract_update.last_pending_block_tx.tag == DOJO::SomeFieldElement) {
            update_dict["last_pending_block_tx"] = FieldElement::get_as_string(
                &contract_update.last_pending_block_tx.some);
        }
        update_dict["updated_at"] = static_cast<int64_t>(contract_update.updated_at);
        update_dict["created_at"] = static_cast<int64_t>(contract_update.created_at);

        singleton->on_contract_update_callback.call_deferred(update_dict);
    }
}

void token_balance_update_callback_wrapper(DOJO::TokenBalance token_balance) {
    Logger::info("token_balance_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_token_balance_update_callback.is_valid()) {
        Dictionary balance_dict;
        balance_dict["balance"] = memnew(U256(token_balance.balance));
        balance_dict["account_address"] = FieldElement::get_as_string(&token_balance.account_address);
        balance_dict["contract_address"] = FieldElement::get_as_string(&token_balance.contract_address);
        if (token_balance.token_id.tag == DOJO::SomeU256) {
            balance_dict["token_id"] = memnew(U256(token_balance.token_id.some));
        } else {
            balance_dict["token_id"] = Variant();
        }
        singleton->on_token_balance_update_callback.call_deferred(balance_dict);
    }
}

void token_transfer_update_callback_wrapper(DOJO::TokenTransfer token_transfer) {
    Logger::info("token_transfer_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_token_transfer_update_callback.is_valid()) {
        Dictionary transfer_dict;
        transfer_dict["id"] = token_transfer.id;
        transfer_dict["contract_address"] = FieldElement::get_as_string_no_ptr(token_transfer.contract_address);
        transfer_dict["from_address"] = FieldElement::get_as_string_no_ptr(token_transfer.from_address);
        transfer_dict["to_address"] = FieldElement::get_as_string_no_ptr(token_transfer.to_address);
        Ref<U256> amount = memnew(U256(token_transfer.amount));
        transfer_dict["amount"] = amount;
        Ref<OptionU256> option_u256 = memnew(OptionU256(token_transfer.token_id));
        transfer_dict["option_token_id"] = option_u256;
        transfer_dict["executed_at"] = token_transfer.executed_at;
        Ref<OptionChar> option_char = memnew(OptionChar(token_transfer.event_id));
        transfer_dict["even_id"] = option_char;
        singleton->on_token_transfer_update_callback.call_deferred(transfer_dict);
    }
}

void on_aggregation_update_callback_wrapper(DOJO::AggregationEntry entry) {
    Logger::info("on_aggregation_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_aggregation_update_callback.is_valid()) {
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

        singleton->on_aggregation_update_callback.call_deferred(entry_dict);
    }
}

void on_activity_update_callback_wrapper(DOJO::Activity activity) {
    Logger::info("on_activity_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_activity_update_callback.is_valid()) {
        Dictionary activity_dict;

        activity_dict["id"] = activity.id;
        activity_dict["world_address"] = FieldElement::get_as_string(&activity.world_address);
        activity_dict["namespace_"] = activity.namespace_;
        activity_dict["caller_address"] = FieldElement::get_as_string(&activity.caller_address);
        activity_dict["session_start"] = activity.session_start;
        activity_dict["session_end"] = activity.session_end;
        activity_dict["action_count"] = activity.action_count;
        activity_dict["actions"] = DojoArray::CArrayActionCountToVariant(activity.actions);
        activity_dict["updated_at"] = activity.updated_at;

        singleton->on_activity_update_callback.call_deferred(activity_dict);
    }
}

void on_achievement_progression_update_callback_wrapper(DOJO::AchievementProgression achievement_progression) {
    Logger::info("on_achievement_progression_update_callback_wrapper");
    ToriiClient *singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_achievement_progression_update_callback.is_valid()) {
        Dictionary progress_dict;

        progress_dict["id"] = achievement_progression.id;
        progress_dict["achievement_id"] = achievement_progression.achievement_id;
        progress_dict["task_id"] = achievement_progression.task_id;
        progress_dict["world_address"] = FieldElement::get_as_string(&achievement_progression.world_address);
        progress_dict["namespace_"] = achievement_progression.namespace_;
        progress_dict["player_id"] = FieldElement::get_as_string(&achievement_progression.player_id);
        progress_dict["count"] = achievement_progression.count;
        progress_dict["completed"] = achievement_progression.completed;
        progress_dict["completed_at"] = OptionU64::from_native(achievement_progression.completed_at) ;
        progress_dict["created_at"] = achievement_progression.created_at;
        progress_dict["updated_at"] = achievement_progression.updated_at;

        singleton->on_achievement_progression_update_callback.call_deferred(progress_dict);
    }

}

// Subscription method implementations

void ToriiClient::on_entity_state_update(const Callable &callback, const Ref<EntitySubscription> &subscription) {
#ifdef WEB_ENABLED
    // TODO: Add _is_ready_for_subscription check for web
    on_entity_state_update_callback = callback;
    Callable internal_callback = Callable(this, "_on_entity_state_update_emitted");

    Dictionary s;
    s["clause"] = subscription->get_clause(); // Assuming get_clause() returns a Dictionary or similar

    Array args;
    args.push_back(s);
    DojoBridge::call_async("toriiclient_onEntityStateUpdate", args, internal_callback);
    Logger::success_extra("ToriiClient", "Subscribed to entity state updates (Web)");
#else
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    on_entity_state_update_callback = callback;
    subscription->set_callback(callback);
    DOJO::CArrayFieldElement world_addresses = subscription->get_native_world_addresses();

    DOJO::ResultSubscription result = DOJO::client_on_entity_state_update(
        client, subscription->get_native_clause(), world_addresses.data, world_addresses.data_len,
        entity_state_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to subscribe to entity state updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_entity_state_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Subscribed to entity state updates");
        call_deferred("emit_signal", "subscription_created", "entity_state_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
#endif
}

void ToriiClient::on_event_message_update(const Callable &callback, const Ref<MessageSubscription> &subscription) {
#ifdef WEB_ENABLED
    // TODO: Add _is_ready_for_subscription check for web
    on_event_message_update_callback = callback;
    Callable internal_callback = Callable(this, "_on_event_message_update_emitted");

    Dictionary s;
    Variant clause = subscription->get_clause();
    if (clause.get_type() != Variant::NIL) {
        s["clause"] = clause;
    }

    Array args;
    args.push_back(s);
    DojoBridge::call_async("toriiclient_onEventMessageUpdated", args, internal_callback);
    Logger::success_extra("ToriiClient", "Subscribed to event message updates (Web)");
#else
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    on_event_message_update_callback = callback;
    subscription->set_callback(callback);
    DOJO::CArrayFieldElement world_addresses = subscription->get_native_world_addresses();

    DOJO::ResultSubscription result = DOJO::client_on_event_message_update(
        client, subscription->get_native_clause(), world_addresses.data, world_addresses.data_len,
        event_message_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to subscribe to event message updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_event_message_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Subscribed to event message updates");
        call_deferred("emit_signal", "subscription_created", "event_message_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
#endif
}

void ToriiClient::on_starknet_event(const Callable &callback, const Ref<StarknetSubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    this->on_starknet_event_callback = callback;
    subscription->set_callback(callback);
    DOJO::ResultSubscription result = DOJO::client_on_starknet_event(client, subscription->get_native_clause(), 1,
                                                                     starknet_event_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to subscribe to starknet events: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        this->on_starknet_event_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Subscribed to starknet events");
        call_deferred("emit_signal", "subscription_created", "starknet_event");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_transaction(const Callable &callback, const Ref<TransactionSubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    this->on_transaction_callback = callback;
    subscription->set_callback(callback);
    DOJO::ResultSubscription result = DOJO::client_on_transaction(client, subscription->get_native_filter(),
                                                                  transaction_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to subscribe to transactions: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        this->on_transaction_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Subscribed to transactions");
        call_deferred("emit_signal", "subscription_created", "transaction");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_token_update(const Callable &callback, const Ref<TokenSubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    on_token_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::CArrayFieldElement contracts = subscription->get_native_contract_addresses();
    DOJO::CArrayU256 token_ids = subscription->get_native_token_ids();

    DOJO::ResultSubscription result = DOJO::client_on_token_update(client, contracts.data, contracts.data_len,
                                                                   token_ids.data, token_ids.data_len,
                                                                   token_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to subscribe to token updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_token_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Subscribed to token updates");
        call_deferred("emit_signal", "subscription_created", "token_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_contract_update(const Callable &callback, const Ref<ContractSubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    on_contract_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::ResultSubscription result = DOJO::on_contract_update(client, subscription->get_native_contract_address(),
                                                               contract_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to subscribe to contract updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_contract_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Subscribed to contract updates");
        call_deferred("emit_signal", "subscription_created", "contract_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_token_balance_update(const Callable &callback, const Ref<TokenBalanceSubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    on_token_balance_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::CArrayFieldElement contracts = subscription->get_native_contract_addresses();
    DOJO::CArrayFieldElement accounts = subscription->get_native_account_addresses();
    DOJO::CArrayU256 token_ids = subscription->get_native_token_ids();

    DOJO::ResultSubscription result = DOJO::client_on_token_balance_update(
        client, contracts.data, contracts.data_len, accounts.data, accounts.data_len, token_ids.data,
        token_ids.data_len, token_balance_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to subscribe to token balance updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_token_balance_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Subscribed to token balance updates");
        call_deferred("emit_signal", "subscription_created", "token_balance_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_token_transfer_update(const Callable &callback,
                                           const Ref<TokenTransferSubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    on_token_transfer_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::CArrayFieldElement contracts = subscription->get_native_contract_addresses();
    DOJO::CArrayFieldElement accounts = subscription->get_native_account_addresses();
    DOJO::CArrayU256 token_ids = subscription->get_native_token_ids();

    DOJO::ResultSubscription result = DOJO::client_on_token_transfer_update(
        client,
        contracts.data,
        contracts.data_len,
        accounts.data,
        accounts.data_len,
        token_ids.data,
        token_ids.data_len,
        token_transfer_update_callback_wrapper
    );

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to suscribe to token transfer updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_token_transfer_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Suscribed to token transfer updates");
        call_deferred("emit_signal", "subscription_created", "token_transfer_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_aggregation_update(const Callable &callback, const Ref<AggregationSubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    on_aggregation_update_callback = callback;
    subscription->set_callback(callback);
    DOJO::CArrayc_char aggregator_ids = subscription->get_native_aggregator_id();
    DOJO::CArrayc_char entity_ids = subscription->get_native_entity_ids();

    DOJO::ResultSubscription result = DOJO::client_on_aggregation_update(
        client,
        aggregator_ids.data,
        aggregator_ids.data_len,
        entity_ids.data,
        entity_ids.data_len,
        on_aggregation_update_callback_wrapper
    );

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to suscribe to aggregation updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_token_transfer_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Suscribed to aggregation updates");
        call_deferred("emit_signal", "subscription_created", "aggregation_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_activity_update(const Callable &callback, const Ref<ActivitySubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    on_activity_update_callback = callback;
    subscription->set_callback(callback);
    DOJO::CArrayFieldElement world_addresses = subscription->get_native_world_addresses();
    DOJO::CArrayc_char namespaces = subscription->get_native_namespaces();
    DOJO::CArrayFieldElement caller_addresses = subscription->get_native_caller_addresses();

    DOJO::ResultSubscription result = DOJO::client_on_activity_update(
        client,
        world_addresses.data,
        world_addresses.data_len,
        namespaces.data,
        namespaces.data_len,
        caller_addresses.data,
        caller_addresses.data_len,
        on_activity_update_callback_wrapper
    );

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to suscribe to activity updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_token_transfer_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Suscribed to activity updates");
        call_deferred("emit_signal", "subscription_created", "activity_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_achievement_progression_update(const Callable &callback,
                                                    const Ref<AchievementProgressionSubscription> &subscription) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }

    on_achievement_progression_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::CArrayFieldElement world_addresses = subscription->get_native_world_addresses();
    DOJO::CArrayc_char namespaces = subscription->get_native_namespaces();
    DOJO::CArrayFieldElement player_addresses = subscription->get_native_player_addresses();
    DOJO::CArrayc_char achievements_ids = subscription->get_native_achievements_ids();


    DOJO::ResultSubscription result = DOJO::client_on_achievement_progression_update(
        client,
        world_addresses.data,
        world_addresses.data_len,
        namespaces.data,
        namespaces.data_len,
        player_addresses.data,
        player_addresses.data_len,
        achievements_ids.data,
        achievements_ids.data_len,
        on_achievement_progression_update_callback_wrapper
    );

    if (result.tag == DOJO::ErrSubscription) {
        String error_msg = "Failed to suscribe to achievement progression updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
        on_achievement_progression_update_callback = Callable();
    } else {
        Logger::success_extra("ToriiClient", "Suscribed to achievement progression updates");
        call_deferred("emit_signal", "subscription_created", "achievement_progression_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }



}


bool ToriiClient::_is_ready_for_query(const Ref<Resource> &query) const {
    if (!is_client_connected()) {
        Logger::error("Client not connected. Cannot execute query.");
        return false;
    }
    if (!query.is_valid()) {
        Logger::error("Invalid query object provided. It's null or invalid.");
        return false;
    }
    return true;
}

bool ToriiClient::_is_ready_for_subscription(const Ref<DojoSubscription> &subscription) const {
    if (!is_client_connected()) {
        Logger::error("Client not connected. Cannot create subscription.");
        return false;
    }
    if (!subscription.is_valid()) {
        Logger::error("Invalid subscription resource provided. It's null or invalid.");
        return false;
    }
    return true;
}

void ToriiClient::update_subscription(const Ref<DojoSubscription> &subscription, const Callable &callback) {
    using SubType = DojoSubscription::Type;
    Logger::info("Updating Subscription[color=Green]", subscription->get_name(), "[/color]");
    switch (subscription->get_type()) {
        case SubType::EVENT:
            update_event_message_subscription(subscription, callback);
            break;
        case SubType::ENTITY:
            update_entity_subscription(subscription, callback);
            break;
        case SubType::CONTRACT:
            update_contract_subscription(subscription, callback);
            break;
        case SubType::TOKEN:
            update_token_subscription(subscription, callback);
            break;
        case SubType::TOKEN_BALANCE:
            update_token_balance_subscription(subscription, callback);
            break;
        case SubType::TRANSACTION:
            update_transaction_subscription(subscription, callback);
            break;
        case SubType::STARKNET:
            update_starknet_event_subscription(subscription, callback);
            break;
        default:
            Logger::error(subscription->get_name(),
                          " doesnt support updating its subscription or is not implemented yet.");
            break;
    }
}

void ToriiClient::update_entity_subscription(const Ref<EntitySubscription> &subscription, const Callable &callback) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    DOJO::CArrayFieldElement world_addresses = subscription->get_native_world_addresses();

    DOJO::Resultbool result = DOJO::client_update_entity_subscription(
        client,
        subscription->get_subscription(),
        subscription->get_native_clause(),
        world_addresses.data,
        world_addresses.data_len
    );
    if (result.tag == DOJO::Errbool) {
        String error_msg = "Failed to update entity subscription: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
    } else {
        Logger::success_extra("ToriiClient", "Updated entity subscription");
        subscription->update_callback(callback);
    }
}
#ifdef WEB_ENABLED
// Web-specific callbacks
void ToriiClient::_on_get_entities_completed(const Variant &result) {
    if (result.get_type() == Variant::Type::NIL) {
        Logger::error("Failed to get entities from web bridge.");
        call_deferred("emit_signal", "entities_received", Array());
        return;
    }

    Logger::success_extra("ToriiClient", "Entities received from web bridge.");
    call_deferred("emit_signal", "entities_received", result);
}

void ToriiClient::_on_get_world_metadata_completed(const Variant &result) {
    if (result.get_type() == Variant::Type::NIL) {
        Logger::error("Failed to get metadata from web bridge.");
        call_deferred("emit_signal", "metadata_updated", Dictionary());
        return;
    }

    Logger::success_extra("ToriiClient", "Metadata received from web bridge.");
    call_deferred("emit_signal", "metadata_updated", result);
}

void ToriiClient::_on_entity_state_update_emitted(const Variant &entity_data) {
    // This is called by the JS bridge on each update. Now, call the user's callback.
    if (on_entity_state_update_callback.is_valid()) {
        on_entity_state_update_callback.call(entity_data);
    }
}

void ToriiClient::_on_event_message_update_emitted(const Variant &message_data) {
    // This is called by the JS bridge on each update. Now, call the user's callback.
    if (on_event_message_update_callback.is_valid()) {
        on_event_message_update_callback.call(message_data);
    }
}

void ToriiClient::_on_client_created(const Variant &result) {
    Logger::debug_extra("ToriiClient", "_on_client_created callback executed.");
    bool success = result.get_type() != Variant::Type::NIL;

    if (success) {
        is_web_client_initialized = true;
        is_connected = true;
        Logger::success_extra("ToriiClient", "Web client created successfully.");
    } else {
        is_web_client_initialized = false;
        is_connected = false;
        Logger::error("Failed to create web client. Result was null.");
    }

    call_deferred("emit_signal", "client_connected", success);
}
#endif

void ToriiClient::update_event_message_subscription(const Ref<MessageSubscription> &subscription,
                                                    const Callable &callback) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    DOJO::CArrayFieldElement world_addresses = subscription->get_native_world_addresses();

    DOJO::Resultbool result = DOJO::client_update_event_message_subscription(
        client,
        subscription->get_subscription(),
        subscription->get_native_clause(),
        world_addresses.data,
        world_addresses.data_len
    );
    if (result.tag == DOJO::Errbool) {
        String error_msg = "Failed to update event message subscription: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
    } else {
        Logger::success_extra("ToriiClient", "Updated event message subscription");
        subscription->update_callback(callback);
    }
}

void ToriiClient::update_starknet_event_subscription(const Ref<StarknetSubscription> &subscription,
                                                     const Callable &callback) {
    Logger::warning("update_starknet_event_subscription not implemented on dojo.c side");
}

void ToriiClient::update_transaction_subscription(const Ref<TransactionSubscription> &subscription,
                                                  const Callable &callback) {
    Logger::warning("update_transaction_subscription not implemented on dojo.c side");
}

void ToriiClient::update_token_subscription(const Ref<TokenSubscription> &subscription, const Callable &callback) {
    Logger::warning("update_token_subscription not implemented on dojo.c side");
}

void ToriiClient::update_contract_subscription(const Ref<ContractSubscription> &subscription,
                                               const Callable &callback) {
    Logger::warning("update_contract_subscription not implemented on dojo.c side");
}

void ToriiClient::update_token_balance_subscription(const Ref<TokenBalanceSubscription> &subscription,
                                                    const Callable &callback) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    DOJO::CArrayFieldElement contracts = subscription->get_native_contract_addresses();
    DOJO::CArrayFieldElement accounts = subscription->get_native_account_addresses();
    DOJO::CArrayU256 token_ids = subscription->get_native_token_ids();

    DOJO::Resultbool result = DOJO::client_update_token_balance_subscription(
        client,
        subscription->get_subscription(),
        contracts.data,
        contracts.data_len,
        accounts.data,
        accounts.data_len,
        token_ids.data, token_ids.data_len
    );
    if (result.tag == DOJO::Errbool) {
        String error_msg = "Failed to update token balance subscription: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
    } else {
        Logger::success_extra("ToriiClient", "Updated token balance subscription");
        subscription->update_callback(callback);
    }
}

void ToriiClient::update_token_transfer_subscription(const Ref<TokenTransferSubscription> &subscription,
                                                     const Callable &callback) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    DOJO::CArrayFieldElement contracts = subscription->get_native_contract_addresses();
    DOJO::CArrayFieldElement accounts = subscription->get_native_account_addresses();
    DOJO::CArrayU256 token_ids = subscription->get_native_token_ids();

    DOJO::Resultbool result = DOJO::client_update_token_transfer_subscription(
        client,
        subscription->get_subscription(),
        contracts.data,
        contracts.data_len,
        accounts.data,
        accounts.data_len,
        token_ids.data,
        token_ids.data_len
    );
    if (result.tag == DOJO::Errbool) {
        String error_msg = "Failed to update token transfer subscription: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        call_deferred("emit_signal", "subscription_error", error_msg);
    } else {
        Logger::success_extra("ToriiClient", "Updated token transfer subscription");
        subscription->update_callback(callback);
    }
}

void ToriiClient::update_aggregation_subscription(const Ref<AggregationSubscription> &subscription,
                                                  const Callable &callback) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    DOJO::CArrayc_char aggregator_ids = subscription->get_native_aggregator_id();
    DOJO::CArrayc_char entity_ids = subscription->get_native_entity_ids();

    DOJO::Resultbool result = DOJO::client_update_aggregation_subscription(
        client,
        subscription->get_subscription(),
        aggregator_ids.data,
        aggregator_ids.data_len,
        entity_ids.data,
        entity_ids.data_len
    );
    if (result.tag == DOJO::Errbool) {
        Logger::error("Failed to update Aggregation Subscription", GET_DOJO_ERROR(result));
        call_deferred("emit_signal", "subscription_error", GET_DOJO_ERROR(result));
    } else {
        Logger::success_extra("ToriiClient", "Updated aggregations subscription");
        subscription->update_callback(callback);
    }
}

void ToriiClient::update_activity_subscription(const Ref<ActivitySubscription> &subscription,
                                               const Callable &callback) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }
    DOJO::CArrayc_char namespaces = subscription->get_native_namespaces();
    DOJO::CArrayFieldElement world_addresses = subscription->get_native_world_addresses();
    DOJO::CArrayFieldElement caller_addresses = subscription->get_native_caller_addresses();

    DOJO::Resultbool result = DOJO::client_update_activity_subscription(
        client,
        subscription->get_subscription(),
        world_addresses.data,
        world_addresses.data_len,
        namespaces.data,
        namespaces.data_len,
        caller_addresses.data,
        caller_addresses.data_len
    );

    if (result.tag == DOJO::Errbool) {
        Logger::error("Failed to update Activities Subscription", GET_DOJO_ERROR(result));
        call_deferred("emit_signal", "subscription_error", GET_DOJO_ERROR(result));
    } else {
        Logger::success_extra("ToriiClient", "Updated Activities subscription");
        subscription->update_callback(callback);
    }
}

void ToriiClient::update_achievement_progression(const Ref<AchievementProgressionSubscription> &subscription,
                                                 const Callable &callback) {
    if (!_is_ready_for_subscription(subscription)) {
        return;
    }

    on_achievement_progression_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::CArrayFieldElement world_addresses = subscription->get_native_world_addresses();
    DOJO::CArrayc_char namespaces = subscription->get_native_namespaces();
    DOJO::CArrayFieldElement player_addresses = subscription->get_native_player_addresses();
    DOJO::CArrayc_char achievements_ids = subscription->get_native_achievements_ids();


    DOJO::Resultbool result = DOJO::client_update_achievement_progression_subscription(
        client,
        subscription->get_subscription(),
        world_addresses.data,
        world_addresses.data_len,
        namespaces.data,
        namespaces.data_len,
        player_addresses.data,
        player_addresses.data_len,
        achievements_ids.data,
        achievements_ids.data_len
    );
    if (result.tag == DOJO::Errbool) {
        Logger::error("Failed to update Achievement Progression Subscription", GET_DOJO_ERROR(result));
        call_deferred("emit_signal", "subscription_error", GET_DOJO_ERROR(result));
    } else {
        Logger::success_extra("ToriiClient", "Updated Achievement Progression subscription");
        subscription->update_callback(callback);
    }
}
