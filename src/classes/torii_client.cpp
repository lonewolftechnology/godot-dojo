//
// Created by hazel on 27/05/25.
//

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"

#include "classes/torii_client.h"

#include "tools/logger.h"
#include "variant/ty/dojo_array.h"
#include "resources/queries/dojo_query.h"

ToriiClient* ToriiClient::singleton = nullptr;

ToriiClient::ToriiClient()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        Logger::debug_extra("Torii", "Editor mode, ignoring constructor");
        return;
    }

    singleton = this;
    client = nullptr;
    is_connected = false;
    world_address = "";
    world = {};
    Logger::info("ToriiClient initialized");
}

ToriiClient::~ToriiClient()
{
    disconnect_client(false);
    singleton = nullptr;
    Logger::info("ToriiClient destroyed");
}

ToriiClient* ToriiClient::get_singleton()
{
    return singleton;
}

bool ToriiClient::create_client()
{
    disconnect_client(true);

    if (world_address.is_empty())
    {
        Logger::error("Missing world address");
        emit_signal("client_connected", false);
        return false;
    }

    if (torii_url.is_empty())
    {
        Logger::error("Missing torii url");
        emit_signal("client_connected", false);
        return false;
    }
    FieldElement world_felt(world_address, 32);
    set_world(world_felt);

    Logger::info("Creating Torii client...");
    Logger::info("URL: ", torii_url);
    Logger::info("World Address: ", world_address);

    DOJO::ResultToriiClient resClient = DOJO::client_new(
        torii_url.utf8().get_data(),
        world_felt.get_felt_no_ptr()
    );

    if (resClient.tag == DOJO::ErrToriiClient)
    {
        Logger::error(String("Failed to create Torii client: ") + GET_DOJO_ERROR(resClient));
        emit_signal("client_connected", false);
        return false;
    }

    client = resClient.ok;
    is_connected = true;

    DOJO::client_set_logger(client, [](const char* msg)
    {
        Logger::info("ToriiClient: ", msg);
    });

    Logger::success("Torii client created successfully");
    emit_signal("client_connected", true);
    return true;
}

void ToriiClient::disconnect_client(bool send_signal = true)
{
    if (client != nullptr)
    {
        cancel_all_subscriptions();

        client = nullptr;
        is_connected = false;
        if (send_signal)
        {
            emit_signal("client_disconnected");
        }
        Logger::info("Torii client disconnected");
    }
}

bool ToriiClient::is_client_connected() const
{
    return is_connected && client != nullptr;
}

bool ToriiClient::is_callable_valid() const
{
    return logger_callback.is_valid();
}

void ToriiClient::callable_call(const char* msg) const
{
    if (is_callable_valid())
    {
        (void)logger_callback.call(String(msg));
    }
}

FieldElement ToriiClient::get_world() const
{
    return world;
}

void ToriiClient::set_world(const FieldElement& n_world)
{
    world = n_world.get_felt();
}

Dictionary ToriiClient::get_world_metadata() const
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return Logger::error_dict("Client not connected");
    }

    DOJO::ResultWorld resMetadata = DOJO::client_metadata(client);

    if (resMetadata.tag == DOJO::ErrWorld)
    {
        return Logger::error_dict(String("Failed to get metadata: ") + GET_DOJO_ERROR(resMetadata));
    }

    DOJO::World metadata = GET_DOJO_OK(resMetadata);
    Dictionary result = {};

    TypedArray<Dictionary> models_array = DojoArray(metadata.models).get_value();

    result["models"] = models_array;
    result["world_address"] = world_address;

    Logger::success("Metadata obtained");
    return result;
}

bool ToriiClient::refresh_metadata()
{
    Dictionary metadata = get_world_metadata();
    if (!metadata.is_empty()) {
        emit_signal("metadata_updated", metadata);
        return true;
    }
    return false;
}

TypedArray<Dictionary> ToriiClient::get_entities(const Dictionary& query_params) const
{
    Logger::info("Getting entities...");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }
    Logger::debug("Converting Query");
    DOJO::Query query = create_query_from_dict(query_params);
    DOJO::ResultPageEntity resPageEntities = DOJO::client_entities(client, query);

    if (resPageEntities.tag == DOJO::ErrPageEntity)
    {
        Logger::error(String("Failed to get entities: ") + GET_DOJO_ERROR(resPageEntities));
        return {};
    }

    DOJO::PageEntity pageEntities = resPageEntities.ok;
    TypedArray<Dictionary> result = DojoArray(pageEntities.items).get_value();

    Logger::success("Entities obtained: ", String::num_int64(result.size()));
    return result;
}

TypedArray<Dictionary> ToriiClient::get_contollers_by_usernames(const TypedArray<String>& usernames)
{
    return get_controllers(usernames,Array());
}

TypedArray<Dictionary> ToriiClient::get_contollers_by_addresses(const TypedArray<String>& addresses)
{
    return get_controllers(Array(), addresses);
}

TypedArray<Dictionary> ToriiClient::get_controllers(const TypedArray<String>& usernames = Array(), const TypedArray<String>& addresses = Array())
{
    if (!is_client_connected())
    {
        Logger::error("Client unavailable");
        return {Logger::error_dict("Client unavailable")};
    }

    if (addresses.is_empty())
    {
        Logger::info("Addresses is empty, fetching all controllers");
    }
    DojoArray::CStringArrayHelper usernames_helper(usernames);

    DojoArray::CFieldElementArrayHelper addresses_helper(addresses);

    DOJO::ControllerQuery controller_query = {};

    controller_query.pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;
    controller_query.pagination.limit.tag = DOJO::COptionu32_Tag::Noneu32;
    controller_query.pagination.order_by = {nullptr, 0};
    controller_query.pagination.direction = DOJO::PaginationDirection::Forward;

    controller_query.usernames = usernames_helper.c_array;
    controller_query.contract_addresses = addresses_helper.c_array;

    DOJO::ResultPageController res_controllers = DOJO::client_controllers(client, controller_query);

    if (res_controllers.tag == DOJO::ErrPageController) {
        Logger::error(String("Failed to get controllers: ") + GET_DOJO_ERROR(res_controllers));
        DOJO::error_free(&res_controllers.err);
        return {};
    }

    Ref<DojoArray> result_array = memnew(DojoArray(GET_DOJO_OK(res_controllers)));
    return result_array->get_value();
}


Dictionary ToriiClient::get_controller_info(const String& controller_address)
{
    TypedArray<Dictionary> controllers = get_controllers(Array(), {controller_address});

    for (int i = 0; i < controllers.size(); i++)
    {
        Dictionary controller = controllers[i];
        if (controller.get("address", "") == controller_address)
        {
            return controller;
        }
    }

    return Logger::error_dict("Controller not found");
}

TypedArray<Dictionary> ToriiClient::get_tokens(const Dictionary& query_params) const
{
    Logger::info("Getting tokens...");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }

    // Extract parameters from query_params
    String cursor = query_params.get("cursor", "");

    // Extract contract addresses if provided
    Array contract_addresses_array = query_params.get("contract_addresses", Array());
    std::vector<DOJO::FieldElement> contract_addresses_vec;
    DOJO::FieldElement* contract_addresses_ptr = nullptr;
    size_t contract_addresses_len = 0;

    if (!contract_addresses_array.is_empty())
    {
        for (int i = 0; i < contract_addresses_array.size(); i++)
        {
            String address = contract_addresses_array[i];
            FieldElement felt(address, 32);
            contract_addresses_vec.push_back(felt.get_felt_no_ptr());
        }
        contract_addresses_ptr = contract_addresses_vec.data();
        contract_addresses_len = contract_addresses_vec.size();
    }

    // Extract token IDs if provided
    Array token_ids_array = query_params.get("token_ids", Array());
    std::vector<DOJO::U256> token_ids_vec;
    DOJO::U256* token_ids_ptr = nullptr;
    size_t token_ids_len = 0;

    if (!token_ids_array.is_empty())
    {
        for (int i = 0; i < token_ids_array.size(); i++)
        {
            // Assuming token IDs are provided as strings
            String token_id = token_ids_array[i];
            DOJO::U256 u256_token_id = {};
            u256_token_id.data[0] = token_id.to_int();
            token_ids_vec.push_back(u256_token_id);
        }
        token_ids_ptr = token_ids_vec.data();
        token_ids_len = token_ids_vec.size();
    }

    DOJO::TokenQuery token_query = {};
    token_query.pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;
    token_query.pagination.limit.tag = DOJO::COptionu32_Tag::Noneu32;
    token_query.pagination.order_by = {nullptr, 0};
    token_query.pagination.direction = DOJO::PaginationDirection::Forward;

    token_query.contract_addresses = {contract_addresses_ptr, contract_addresses_len};
    token_query.token_ids = {token_ids_ptr, token_ids_len};

    DOJO::ResultPageToken result = DOJO::client_tokens(
        client,
        token_query
    );

    if (result.tag == DOJO::ErrPageToken)
    {
        Logger::error(String("Error getting tokens: ") + GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageToken tokens = GET_DOJO_OK(result);
    TypedArray<Dictionary> result_array;

    for (size_t i = 0; i < tokens.items.data_len; i++)
    {
        DOJO::Token token = tokens.items.data[i];
        Dictionary token_dict;
        if (token.token_id.tag == DOJO::SomeU256)
        {
            token_dict["token_id"] = String::num_uint64(token.token_id.some.data[0]);
        }else
        {
            token_dict["token_id"] = nullptr;
        }

        token_dict["contract_address"] = FieldElement::get_as_string(&token.contract_address);
        token_dict["name"] = String(token.name);
        token_dict["symbol"] = String(token.symbol);
        token_dict["decimals"] = token.decimals;

        if (token.metadata != nullptr)
        {
            token_dict["metadata"] = String(token.metadata);
        }
        else
        {
            token_dict["metadata"] = "";
        }

        result_array.push_back(token_dict);
    }

    Logger::success("Tokens obtained: ", String::num_int64(result_array.size()));
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_token_balances(const String& account_address) const
{
    Logger::info("Getting token balances for account: ", account_address);
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }

    FieldElement account_felt(account_address, 32);
    DOJO::FieldElement* account_addresses = account_felt.get_felt();
    size_t account_addresses_len = 1;

    DOJO::FieldElement* contract_addresses = nullptr;
    size_t contract_addresses_len = 0;
    DOJO::U256* token_ids = nullptr;
    size_t token_ids_len = 0;

    DOJO::TokenBalanceQuery balance_query = {};
    balance_query.pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;
    balance_query.pagination.limit.tag = DOJO::COptionu32_Tag::Noneu32;
    balance_query.pagination.order_by = {nullptr, 0};
    balance_query.pagination.direction = DOJO::PaginationDirection::Forward;

    balance_query.contract_addresses = {contract_addresses, contract_addresses_len};
    balance_query.account_addresses = {account_addresses, account_addresses_len};
    balance_query.token_ids = {token_ids, token_ids_len};

    DOJO::ResultPageTokenBalance result = DOJO::client_token_balances(
        client,
        balance_query
    );

    if (result.tag == DOJO::ErrPageTokenBalance)
    {
        Logger::error(String("Error getting token balances: ") + GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageTokenBalance balances = result.ok;
    TypedArray<Dictionary> result_array;

    for (size_t i = 0; i < balances.items.data_len; i++)
    {
        DOJO::TokenBalance balance = balances.items.data[i];
        Dictionary balance_dict;
        if (balance.token_id.tag == DOJO::SomeU256)
        {
            balance_dict["token_id"] = String::num_uint64(balance.token_id.some.data[0]);
        }else
        {
            balance_dict["token_id"] = nullptr;
        }
        balance_dict["balance"] = String::num_uint64(balance.balance.data[0]);
        balance_dict["account_address"] = FieldElement::get_as_string(&balance.account_address);
        balance_dict["contract_address"] = FieldElement::get_as_string(&balance.contract_address);

        result_array.push_back(balance_dict);
    }

    Logger::success("Token balances obtained: ", String::num_int64(result_array.size()));
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_token_collections() const
{
    Logger::info("Getting token collections...");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }

    DOJO::FieldElement* contract_addresses = nullptr;
    size_t contract_addresses_len = 0;
    DOJO::FieldElement* account_addresses = nullptr;
    size_t account_addresses_len = 0;
    DOJO::U256* token_ids = nullptr;
    size_t token_ids_len = 0;

    DOJO::TokenBalanceQuery balance_query = {};
    balance_query.pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;
    balance_query.pagination.limit.tag = DOJO::COptionu32_Tag::Noneu32;
    balance_query.pagination.order_by = {nullptr, 0};
    balance_query.pagination.direction = DOJO::PaginationDirection::Forward;

    balance_query.contract_addresses = {contract_addresses, contract_addresses_len};
    balance_query.account_addresses = {account_addresses, account_addresses_len};
    balance_query.token_ids = {token_ids, token_ids_len};

    DOJO::ResultPageTokenCollection result = DOJO::client_token_collections(
        client,
        balance_query
    );

    if (result.tag == DOJO::ErrPageTokenCollection)
    {
        Logger::error(String("Error getting token collections: ") + GET_DOJO_ERROR(result));
        return {};
    }

    DOJO::PageTokenCollection collections = result.ok;
    TypedArray<Dictionary> result_array;

    for (size_t i = 0; i < collections.items.data_len; i++)
    {
        DOJO::TokenCollection collection = collections.items.data[i];
        Dictionary collection_dict;

        collection_dict["contract_address"] = FieldElement::get_as_string(&collection.contract_address);
        collection_dict["name"] = String(collection.name);
        collection_dict["symbol"] = String(collection.symbol);
        collection_dict["decimals"] = collection.decimals;
        collection_dict["count"] = collection.count;

        if (collection.metadata != nullptr)
        {
            collection_dict["metadata"] = String(collection.metadata);
        }
        else
        {
            collection_dict["metadata"] = "";
        }

        result_array.push_back(collection_dict);
    }

    Logger::success("Token collections obtained: ", String::num_int64(result_array.size()));
    return result_array;
}

Dictionary ToriiClient::get_token_info(const String& token_address) const
{
    Logger::info("Getting token info for: ", token_address);
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return Logger::error_dict("Client not connected");
    }

    FieldElement token_felt(token_address, 32);
    DOJO::FieldElement* contract_addresses = token_felt.get_felt();
    size_t contract_addresses_len = 1;

    DOJO::U256* token_ids = nullptr;
    size_t token_ids_len = 0;

    DOJO::TokenQuery token_query = {};
    token_query.pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;
    token_query.pagination.limit.tag = DOJO::COptionu32_Tag::Noneu32;
    token_query.pagination.order_by = {nullptr, 0};
    token_query.pagination.direction = DOJO::PaginationDirection::Forward;

    token_query.contract_addresses = {contract_addresses, contract_addresses_len};
    token_query.token_ids = {token_ids, token_ids_len};

    DOJO::ResultPageToken result = DOJO::client_tokens(
        client,
        token_query
    );

    if (result.tag == DOJO::ErrPageToken)
    {
        Logger::error(String("Error getting token info: ") + GET_DOJO_ERROR(result));
        return Logger::error_dict("Error getting token info");
    }

    // Convert the result to a Dictionary
    DOJO::PageToken tokens = result.ok;

    if (tokens.items.data_len == 0)
    {
        return Logger::error_dict("Token not found");
    }

    DOJO::Token token = tokens.items.data[0];
    Dictionary token_dict;
    token_dict["contract_address"] = FieldElement::get_as_string(&token.contract_address);
    if (token.token_id.tag == DOJO::SomeU256)
    {
        token_dict["token_id"] = String::num_uint64(token.token_id.some.data[0]);
    }else
    {
        token_dict["token_id"] = nullptr;
    }
    token_dict["name"] = String(token.name);
    token_dict["symbol"] = String(token.symbol);
    token_dict["decimals"] = token.decimals;

    if (token.metadata != nullptr)
    {
        token_dict["metadata"] = String(token.metadata);
    }
    else
    {
        token_dict["metadata"] = "";
    }

    Logger::success("Token info obtained");
    return token_dict;
}

void ToriiClient::cancel_all_subscriptions()
{
    Logger::info("Cancelling all subscriptions");

    for (int i = 0; i < event_subscriptions.size(); i++)
    {
        Ref<DojoSubscription> subscription = event_subscriptions[i];
        if (subscription.is_valid()) {
            subscription->cancel();
        }
    }

    event_subscriptions.clear();

    Logger::success("All subscriptions cancelled");
}

bool ToriiClient::publish_message(const String& message_data, const Array& signature_felts)
{
    Logger::info("publish_message is experimental");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return false;
    }

    DOJO::FieldElement sign_key = DOJO::signing_key_new();
    DOJO::FieldElement hash = DOJO::verifying_key_new(sign_key);
    DOJO::ResultSignature resSig = DOJO::signing_key_sign(sign_key, hash);
    if (resSig.tag == DOJO::ErrSignature)
    {
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

    DOJO::ResultFieldElement result = DOJO::client_publish_message(
        client,
        message
    );

    if (result.tag == DOJO::ErrFieldElement)
    {
        Logger::error(String("Failed to publish message: ") + GET_DOJO_ERROR(result));
        return false;
    }

    FieldElement msg_hash(GET_DOJO_OK(result));
    Logger::success("Message published: ", msg_hash.to_string());
    emit_signal("message_published", msg_hash.to_string());

    return true;
}

bool ToriiClient::publish_typed_message(const Dictionary& typed_data, const Array& signature_felts)
{
    String json_data = JSON::stringify(typed_data);
    return publish_message(json_data, signature_felts);
}

void logger_callback_wrapper(const char* msg)
{
    Logger::debug_extra("Torii LOG", msg);
    ToriiClient* _singleton = ToriiClient::get_singleton();
    if (_singleton->is_callable_valid())
    {
        _singleton->callable_call(msg);
    }
}

void ToriiClient::set_logger_callback(const Callable& p_logger_callback)
{
    {
        if (!is_client_connected()) return;
        logger_callback = p_logger_callback;
        DOJO::client_set_logger(client, logger_callback_wrapper);
    }
}

Dictionary ToriiClient::get_client_info() const
{
    Dictionary info = {};
    info["connected"] = is_connected;
    info["torii_url"] = torii_url;
    info["world_address"] = world_address;
    return info;
}

Dictionary ToriiClient::get_connection_status() const
{
    Dictionary status = {};
    status["connected"] = is_connected;
    status["client_exists"] = (client != nullptr);
    status["torii_url"] = torii_url;
    status["world_address"] = world_address;
    return status;
}

DOJO::Query ToriiClient::create_query_from_dict(const Dictionary& query_params)
{
    DOJO::Query query = {};
    Logger::info("Query Creation");

    DOJO::Pagination pagination = {};
    Logger::info("Pagination Creation");
    Dictionary pagination_dict = query_params.get("pagination", {});

    if (query_params.has("limit"))
    {
        pagination.limit.tag = DOJO::Someu32;
        pagination.limit.some = query_params.get("limit", 10);
    }else
    {
        pagination.limit.tag = DOJO::Noneu32;
    }

    String cursor = pagination_dict.get("cursor", "");
    if (cursor.is_empty())
    {
        pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;
    }
    else
    {
        pagination.cursor.tag = DOJO::COptionc_char_Tag::Somec_char;
        pagination.cursor.some = cursor.utf8().get_data();
    }
    // TODO: Ver una forma "mas elegante"
    Logger::debug(pagination_dict.get("direction", "NO HABIA"));
    if (pagination_dict.get("direction", "forward") == "backward")
    {
        pagination.direction = DOJO::PaginationDirection::Backward;
    }
    else
    {
        pagination.direction = DOJO::PaginationDirection::Forward;
    }

    DOJO::CArrayOrderBy orderBy = {};
    Logger::info("OrderBy Creation");
    Array orderBy_array = pagination_dict.get("order_by", {});
    if (!orderBy_array.is_empty())
    {
        orderBy.data_len = orderBy_array.size();
        orderBy.data = new DOJO::OrderBy[orderBy.data_len];
        for (int i = 0; i < orderBy_array.size(); i++)
        {
            Dictionary data = orderBy_array[i];
            String field = data.get("field", "");
            uint32_t direction = data.get("order_direction", 0);
            auto order_direction = static_cast<DOJO::OrderDirection>(direction);
            // Before 1.6.0 OrderBy had  model_name and member_name, now is field.
            // TODO: Check how it actually works
            orderBy.data[i] = {
                field.utf8().get_data(), order_direction
            };
        }
    }
    pagination.order_by = orderBy;

    query.pagination = pagination;

    query.clause.tag = DOJO::COptionClause_Tag::NoneClause;
    query.no_hashed_keys = query_params.get("no_hashed_keys", false);
    query.historical = query_params.get("historical", false);

    DOJO::CArrayc_char models = {};
    Array models_array = query_params.get("models", {});
    if (!models_array.is_empty())
    {
        models.data_len = models_array.size();
        for (int i = 0; i < models_array.size(); i++)
        {
            String model_name = models_array[i];
            models.data[i] = model_name.utf8().get_data();
        }
    }

    query.models = models;

    return query;
}

DOJO::Pagination ToriiClient::create_pagination_from_dict(const Dictionary& pagination_params)
{
    DOJO::Pagination pagination = {};

    if (pagination_params.has("limit"))
    {
        pagination.limit.tag = DOJO::Someu32;
        pagination.limit.some = pagination_params.get("limit", 10);
    }else
    {
        pagination.limit.tag = DOJO::Noneu32;
    }

    pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;

    String direction = pagination_params.get("direction", "forward");
    pagination.direction = (direction == "backward")
                               ? DOJO::PaginationDirection::Backward
                               : DOJO::PaginationDirection::Forward;

    DOJO::CArrayOrderBy orderBy = {};
    pagination.order_by = orderBy;

    return pagination;
}

// Static subscription callbacks

static void entity_state_update_callback_wrapper(DOJO::FieldElement entity_id, DOJO::CArrayStruct models) {
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_entity_state_update_callback.is_valid()) {
        Dictionary entity_data;
        entity_data["id"] = FieldElement::get_as_string(&entity_id);
        entity_data["models"] = DojoArray(models).get_value();
        (void)singleton->on_entity_state_update_callback.call(entity_data);
    }
}

static void event_message_update_callback_wrapper(DOJO::FieldElement entity_id, DOJO::CArrayStruct models) {
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_event_message_update_callback.is_valid()) {
        Dictionary message_data;
        message_data["id"] = FieldElement::get_as_string(&entity_id);
        message_data["models"] = DojoArray(models).get_value();
        (void)singleton->on_event_message_update_callback.call(message_data);
    }
}

static void starknet_event_callback_wrapper(DOJO::Event event) {
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_starknet_event_callback.is_valid()) {
        Dictionary event_data;
        event_data["keys"] = DojoArray(event.keys).get_value();
        event_data["data"] = DojoArray(event.data).get_value();
        event_data["transaction_hash"] = FieldElement::get_as_string(&event.transaction_hash);
        (void)singleton->on_starknet_event_callback.call(event_data);
    }
}

static void transaction_callback_wrapper(DOJO::Transaction transaction) {
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_transaction_callback.is_valid()) {
        Dictionary tx_dict;
        tx_dict["transaction_hash"] = FieldElement::get_as_string(&transaction.transaction_hash);
        tx_dict["sender_address"] = FieldElement::get_as_string(&transaction.sender_address);
        tx_dict["calldata"] = DojoArray(transaction.calldata).get_value();
        tx_dict["max_fee"] = FieldElement::get_as_string(&transaction.max_fee);
        tx_dict["signature"] = DojoArray(transaction.signature).get_value();
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
            call_dict["calldata"] = DojoArray(call.calldata).get_value();
            call_dict["call_type"] = static_cast<int>(call.call_type);
            call_dict["caller_address"] = FieldElement::get_as_string(&call.caller_address);
            calls_array.push_back(call_dict);
        }
        tx_dict["calls"] = calls_array;

        tx_dict["unique_models"] = DojoArray(transaction.unique_models).get_value();
        (void)singleton->on_transaction_callback.call(tx_dict);
    }
}

static void token_update_callback_wrapper(DOJO::Token token) {
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_token_update_callback.is_valid()) {
        Dictionary token_dict;
        token_dict["contract_address"] = FieldElement::get_as_string(&token.contract_address);
        if (token.token_id.tag == DOJO::SomeU256) {
            token_dict["token_id"] = String::num_uint64(token.token_id.some.data[0]);
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
        (void)singleton->on_token_update_callback.call(token_dict);
    }
}

static void indexer_update_callback_wrapper(DOJO::IndexerUpdate indexer_update) {
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_indexer_update_callback.is_valid()) {
        Dictionary update_dict;
        update_dict["head"] = indexer_update.head;
        update_dict["tps"] = indexer_update.tps;
        update_dict["last_block_timestamp"] = indexer_update.last_block_timestamp;
        update_dict["contract_address"] = FieldElement::get_as_string(&indexer_update.contract_address);
        (void)singleton->on_indexer_update_callback.call(update_dict);
    }
}

static void token_balance_update_callback_wrapper(DOJO::TokenBalance token_balance) {
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_token_balance_update_callback.is_valid()) {
        Dictionary balance_dict;
        balance_dict["balance"] = String::num_uint64(token_balance.balance.data[0]);
        balance_dict["account_address"] = FieldElement::get_as_string(&token_balance.account_address);
        balance_dict["contract_address"] = FieldElement::get_as_string(&token_balance.contract_address);
        if (token_balance.token_id.tag == DOJO::SomeU256) {
            balance_dict["token_id"] = String::num_uint64(token_balance.token_id.some.data[0]);
        } else {
            balance_dict["token_id"] = Variant();
        }
        (void)singleton->on_token_balance_update_callback.call(balance_dict);
    }
}

// Subscription method implementations

void ToriiClient::on_entity_state_update(const Callable& callback, Ref<EntitySubscription> subscription) {
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid()) {
        Logger::error("Invalid entity subscription resource");
        return;
    }

    this->on_entity_state_update_callback = callback;
    DOJO::ResultSubscription result = DOJO::client_on_entity_state_update(client, subscription->get_native_clause(), entity_state_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        Logger::error(String("Failed to subscribe to entity state updates: ") + GET_DOJO_ERROR(result));
        this->on_entity_state_update_callback = Callable();
    } else {
        Logger::success("Subscribed to entity state updates");
        subscription->set_subscription(result.ok);
        event_subscriptions.append(subscription);
    }
}

void ToriiClient::on_event_message_update(const Callable& callback, Ref<MessageSubscription> subscription) {
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return;
    }
     if (!subscription.is_valid()) {
        Logger::error("Invalid message subscription resource");
        return;
    }

    this->on_event_message_update_callback = callback;
    DOJO::ResultSubscription result = DOJO::client_on_event_message_update(client, subscription->get_native_clause(), event_message_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        Logger::error(String("Failed to subscribe to event message updates: ") + GET_DOJO_ERROR(result));
        this->on_event_message_update_callback = Callable();
    } else {
        Logger::success("Subscribed to event message updates");
        subscription->set_subscription(result.ok);
        event_subscriptions.append(subscription);
    }
}

void ToriiClient::on_starknet_event(const Callable& callback, Ref<StarknetSubscription> subscription) {
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid()) {
        Logger::error("Invalid starknet subscription resource");
        return;
    }

    this->on_starknet_event_callback = callback;
    DOJO::ResultSubscription result = DOJO::client_on_starknet_event(client, subscription->get_native_clause(), 1, starknet_event_callback_wrapper);

     if (result.tag == DOJO::ErrSubscription) {
        Logger::error(String("Failed to subscribe to starknet events: ") + GET_DOJO_ERROR(result));
        this->on_starknet_event_callback = Callable();
    } else {
        Logger::success("Subscribed to starknet events");
        subscription->set_subscription(result.ok);
        event_subscriptions.append(subscription);
    }
}

void ToriiClient::on_transaction(const Callable& callback, Ref<TransactionSubscription> subscription) {
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid()) {
        Logger::error("Invalid transaction subscription resource");
        return;
    }

    this->on_transaction_callback = callback;
    DOJO::ResultSubscription result = DOJO::client_on_transaction(client, subscription->get_native_filter(), transaction_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        Logger::error(String("Failed to subscribe to transactions: ") + GET_DOJO_ERROR(result));
        this->on_transaction_callback = Callable();
    } else {
        Logger::success("Subscribed to transactions");
        subscription->set_subscription(result.ok);
        event_subscriptions.append(subscription);
    }
}

void ToriiClient::on_token_update(const Callable& callback, Ref<TokenSubscription> subscription) {
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid()) {
        Logger::error("Invalid token subscription resource");
        return;
    }

    this->on_token_update_callback = callback;
    DOJO::CArrayFieldElement contracts = subscription->get_native_contract_addresses();
    DOJO::CArrayU256 token_ids = subscription->get_native_token_ids();

    DOJO::ResultSubscription result = DOJO::client_on_token_update(client, contracts.data, contracts.data_len, token_ids.data, token_ids.data_len, token_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        Logger::error(String("Failed to subscribe to token updates: ") + GET_DOJO_ERROR(result));
        this->on_token_update_callback = Callable();
    } else {
        Logger::success("Subscribed to token updates");
        subscription->set_subscription(result.ok);
        event_subscriptions.append(subscription);
    }
}

void ToriiClient::on_indexer_update(const Callable& callback, Ref<IndexerSubscription> subscription) {
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid()) {
        Logger::error("Invalid indexer subscription resource");
        return;
    }

    this->on_indexer_update_callback = callback;
    DOJO::ResultSubscription result = DOJO::on_indexer_update(client, subscription->get_native_contract_address(), indexer_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        Logger::error(String("Failed to subscribe to indexer updates: ") + GET_DOJO_ERROR(result));
        this->on_indexer_update_callback = Callable();
    } else {
        Logger::success("Subscribed to indexer updates");
        subscription->set_subscription(result.ok);
        event_subscriptions.append(subscription);
    }
}

void ToriiClient::on_token_balance_update(const Callable& callback, Ref<TokenBalanceSubscription> subscription) {
    if (!is_client_connected()) {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid()) {
        Logger::error("Invalid token balance subscription resource");
        return;
    }

    this->on_token_balance_update_callback = callback;
    DOJO::CArrayFieldElement contracts = subscription->get_native_contract_addresses();
    DOJO::CArrayFieldElement accounts = subscription->get_native_account_addresses();
    DOJO::CArrayU256 token_ids = subscription->get_native_token_ids();

    DOJO::ResultSubscription result = DOJO::client_on_token_balance_update(client, contracts.data, contracts.data_len, accounts.data, accounts.data_len, token_ids.data, token_ids.data_len, token_balance_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription) {
        Logger::error(String("Failed to subscribe to token balance updates: ") + GET_DOJO_ERROR(result));
        this->on_token_balance_update_callback = Callable();
    } else {
        Logger::success("Subscribed to token balance updates");
        subscription->set_subscription(result.ok);
        event_subscriptions.append(subscription);
    }
}

void ToriiClient::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_client"), &ToriiClient::create_client);
    ClassDB::bind_method(D_METHOD("disconnect_client", "send_signal"), &ToriiClient::disconnect_client);
    ClassDB::bind_method(D_METHOD("is_client_connected"), &ToriiClient::is_client_connected);

    ClassDB::bind_method(D_METHOD("get_world_metadata"), &ToriiClient::get_world_metadata);
    ClassDB::bind_method(D_METHOD("refresh_metadata"), &ToriiClient::refresh_metadata);

    ClassDB::bind_method(D_METHOD("get_entities", "query_params"), &ToriiClient::get_entities, DEFVAL(Dictionary()));
    ClassDB::bind_method(D_METHOD("get_controllers", "usernames", "addresses"), &ToriiClient::get_controllers, DEFVAL(Array()), DEFVAL(Array()));
    ClassDB::bind_method(D_METHOD("get_controller_info", "controller_address"), &ToriiClient::get_controller_info);

    ClassDB::bind_method(D_METHOD("get_tokens", "query_params"), &ToriiClient::get_tokens, DEFVAL(Dictionary()));
    ClassDB::bind_method(D_METHOD("get_token_balances", "account_address"), &ToriiClient::get_token_balances);
    ClassDB::bind_method(D_METHOD("get_token_collections"), &ToriiClient::get_token_collections);
    ClassDB::bind_method(D_METHOD("get_token_info", "token_address"), &ToriiClient::get_token_info);

    ClassDB::bind_method(D_METHOD("on_entity_state_update", "callback", "subscription"), &ToriiClient::on_entity_state_update);
    ClassDB::bind_method(D_METHOD("on_event_message_update", "callback", "subscription"), &ToriiClient::on_event_message_update);
    ClassDB::bind_method(D_METHOD("on_starknet_event", "callback", "subscription"), &ToriiClient::on_starknet_event);
    ClassDB::bind_method(D_METHOD("on_transaction", "callback", "subscription"), &ToriiClient::on_transaction);
    ClassDB::bind_method(D_METHOD("on_token_update", "callback", "subscription"), &ToriiClient::on_token_update);
    ClassDB::bind_method(D_METHOD("on_indexer_update", "callback", "subscription"), &ToriiClient::on_indexer_update);
    ClassDB::bind_method(D_METHOD("on_token_balance_update", "callback", "subscription"), &ToriiClient::on_token_balance_update);

    ClassDB::bind_method(D_METHOD("cancel_all_subscriptions"), &ToriiClient::cancel_all_subscriptions);

    ClassDB::bind_method(D_METHOD("publish_message", "message_data", "signature_felts"), &ToriiClient::publish_message);
    ClassDB::bind_method(D_METHOD("publish_typed_message", "typed_data", "signature_felts"), &ToriiClient::publish_typed_message);

    ClassDB::bind_method(D_METHOD("get_client_info"), &ToriiClient::get_client_info);
    ClassDB::bind_method(D_METHOD("get_connection_status"), &ToriiClient::get_connection_status);

    ADD_SIGNAL(MethodInfo("client_connected", PropertyInfo(Variant::BOOL, "success")));
    ADD_SIGNAL(MethodInfo("client_disconnected"));
    ADD_SIGNAL(MethodInfo("entity_updated", PropertyInfo(Variant::DICTIONARY, "entity_data")));
    ADD_SIGNAL(MethodInfo("event_received", PropertyInfo(Variant::DICTIONARY, "event_data")));
    ADD_SIGNAL(MethodInfo("subscription_error", PropertyInfo(Variant::STRING, "error_message")));
    ADD_SIGNAL(MethodInfo("subscription_created", PropertyInfo(Variant::STRING, "subscription_name")));
    ADD_SIGNAL(MethodInfo("metadata_updated", PropertyInfo(Variant::DICTIONARY, "metadata")));
    ADD_SIGNAL(MethodInfo("message_published", PropertyInfo(Variant::STRING, "message_hash")));
    ADD_SIGNAL(MethodInfo("transaction_confirmed", PropertyInfo(Variant::STRING, "transaction_hash")));
    ADD_SIGNAL(MethodInfo("token_balance_updated", PropertyInfo(Variant::DICTIONARY, "balance_data")));

    BIND_ENUM_CONSTANT(ASC);
    BIND_ENUM_CONSTANT(DESC);
    BIND_ENUM_CONSTANT(FORWARD);
    BIND_ENUM_CONSTANT(BACKWARD);

    ClassDB::bind_method(D_METHOD("get_torii_url"), &ToriiClient::get_torii_url);
    ClassDB::bind_method(D_METHOD("set_torii_url", "torii_url"), &ToriiClient::set_torii_url);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "torii_url"), "set_torii_url", "get_torii_url");

    ClassDB::bind_method(D_METHOD("set_world_address", "world_address"), &ToriiClient::set_world_address);
    ClassDB::bind_method(D_METHOD("get_world_address"), &ToriiClient::get_world_address);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "world_address"), "set_world_address", "get_world_address");

    ClassDB::bind_method(D_METHOD("set_logger_callback", "logger_callback"), &ToriiClient::set_logger_callback);
    ClassDB::bind_method(D_METHOD("get_logger_callback"), &ToriiClient::get_logger_callback);
    ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "logger_callback"), "set_logger_callback", "get_logger_callback");

    ClassDB::bind_method(D_METHOD("get_events"), &ToriiClient::get_events);
    ClassDB::bind_method(D_METHOD("set_events", "events"), &ToriiClient::set_events);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "events"), "set_events", "get_events");
}
