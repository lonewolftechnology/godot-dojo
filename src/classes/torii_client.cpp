//
// Created by hazel on 27/05/25.
//

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"

#include "classes/torii_client.h"

#include "tools/logger.h"
#include "variant/ty/dojo_array.h"
#include "resources/queries/dojo_query.h"
#include "resources/queries/dojo_token_query.h"
#include "resources/queries/dojo_token_balance_query.h"
#include "resources/queries/dojo_controller_query.h"
#include "resources/queries/dojo_contract_query.h"

ToriiClient* ToriiClient::singleton = nullptr;

ToriiClient::ToriiClient()
{

    singleton = this;
    client = nullptr;
    is_connected = false;
    world_address = "";
    world = {};
    Logger::debug_extra("ToriiClient", "CONSTRUCTOR CALLED");
}

ToriiClient::~ToriiClient()
{
    disconnect_client(false);
    singleton = nullptr;
    Logger::debug_extra("ToriiClient", "DESTRUCTOR CALLED");
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
        // DOJO::client_free(client);

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
    if (!metadata.is_empty())
    {
        emit_signal("metadata_updated", metadata);
        return true;
    }
    return false;
}

TypedArray<Dictionary> ToriiClient::get_entities(const Ref<DojoQuery>& query) const
{
    Logger::info("Getting entities...");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }

    if (!query.is_valid())
    {
        Logger::error("Invalid DojoQuery object.");
        return {};
    }

    DOJO::Query* native_query_ptr = static_cast<DOJO::Query*>(query->get_native_query());

    DOJO::ResultPageEntity resPageEntities = DOJO::client_entities(client, *native_query_ptr);

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

TypedArray<Dictionary> ToriiClient::get_controllers(Ref<DojoControllerQuery> query)
{
    if (!is_client_connected())
    {
        Logger::error("Client unavailable");
        return {Logger::error_dict("Client unavailable")};
    }

    if (!query.is_valid())
    {
        Logger::error("Invalid DojoControllerQuery object.");
        return {};
    }

    DOJO::ControllerQuery* controller_query_ptr = static_cast<DOJO::ControllerQuery*>(query->get_native_query());

    DOJO::ResultPageController res_controllers = DOJO::client_controllers(client, *controller_query_ptr);

    // Cleanup
    for (size_t i = 0; i < controller_query_ptr->usernames.data_len; ++i)
    {
        delete[] (char*)controller_query_ptr->usernames.data[i];
    }
    delete[] controller_query_ptr->usernames.data;
    delete[] controller_query_ptr->contract_addresses.data;
    if (controller_query_ptr->pagination.cursor.tag == DOJO::COptionc_char_Tag::Somec_char)
    {
        delete[] controller_query_ptr->pagination.cursor.some;
    }
    delete controller_query_ptr;

    if (res_controllers.tag == DOJO::ErrPageController)
    {
        Logger::error(String("Failed to get controllers: ") + GET_DOJO_ERROR(res_controllers));
        DOJO::error_free(&res_controllers.err);
        return {};
    }

    Ref<DojoArray> result_array = memnew(DojoArray(GET_DOJO_OK(res_controllers)));
    return result_array->get_value();
}


Dictionary ToriiClient::get_controller_info(const String& controller_address)
{
    Ref<DojoControllerQuery> query;
    query.instantiate();
    TypedArray<String> addresses;
    addresses.push_back(controller_address);
    query->set_contract_addresses(addresses);

    TypedArray<Dictionary> controllers = get_controllers(query);

    if (!controllers.is_empty())
    {
        return controllers[0];
    }

    return Logger::error_dict("Controller not found");
}

TypedArray<Dictionary> ToriiClient::get_tokens(const Ref<DojoTokenQuery>& query) const
{
    Logger::info("Getting tokens...");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }

    if (!query.is_valid())
    {
        Logger::error("Invalid DojoTokenQuery object.");
        return {};
    }

    DOJO::TokenQuery* token_query_ptr = static_cast<DOJO::TokenQuery*>(query->get_native_query());

    DOJO::ResultPageToken result = DOJO::client_tokens(
        client,
        *token_query_ptr
    );

    // Cleanup
    delete[] token_query_ptr->contract_addresses.data;
    delete[] token_query_ptr->token_ids.data;
    if (token_query_ptr->pagination.cursor.tag == DOJO::COptionc_char_Tag::Somec_char)
    {
        delete[] token_query_ptr->pagination.cursor.some;
    }
    delete token_query_ptr;

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
        }
        else
        {
            token_dict["token_id"] = Variant();
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
            token_dict["metadata"] = Variant();
        }

        result_array.push_back(token_dict);
    }

    Logger::success("Tokens obtained: ", String::num_int64(result_array.size()));
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_token_balances(const Ref<DojoTokenBalanceQuery>& query) const
{
    Logger::info("Getting token balances...");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }

    if (!query.is_valid())
    {
        Logger::error("Invalid DojoTokenBalanceQuery object.");
        return {};
    }

    DOJO::TokenBalanceQuery* balance_query_ptr = static_cast<DOJO::TokenBalanceQuery*>(query->get_native_query());

    DOJO::ResultPageTokenBalance result = DOJO::client_token_balances(
        client,
        *balance_query_ptr
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
        }
        else
        {
            balance_dict["token_id"] = Variant();
        }
        balance_dict["balance"] = String::num_uint64(balance.balance.data[0]);
        balance_dict["account_address"] = FieldElement::get_as_string(&balance.account_address);
        balance_dict["contract_address"] = FieldElement::get_as_string(&balance.contract_address);

        result_array.push_back(balance_dict);
    }

    Logger::success("Token balances obtained: ", String::num_int64(result_array.size()));
    return result_array;
}

TypedArray<Dictionary> ToriiClient::get_token_collections(const Ref<DojoTokenBalanceQuery>& query) const
{
    Logger::info("Getting token collections...");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }

    if (!query.is_valid())
    {
        Logger::error("Invalid DojoTokenBalanceQuery object.");
        return {};
    }

    DOJO::TokenBalanceQuery* balance_query_ptr = static_cast<DOJO::TokenBalanceQuery*>(query->get_native_query());

    DOJO::ResultPageTokenCollection result = DOJO::client_token_collections(
        client,
        *balance_query_ptr
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
            collection_dict["metadata"] = Variant();
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
    }
    else
    {
        token_dict["token_id"] = Variant();
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
        token_dict["metadata"] = Variant();
    }

    Logger::success("Token info obtained");
    return token_dict;
}

void ToriiClient::cancel_all_subscriptions()
{
    Logger::info("Cancelling all subscriptions");

    for (int i = 0; i < subscriptions.size(); i++)
    {
        Ref<DojoSubscription> subscription = subscriptions[i];
        if (subscription.is_valid())
        {
            subscription->cancel();
        }
    }

    subscriptions.clear();

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
    }
    else
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
    }
    else
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

void entity_state_update_callback_wrapper(DOJO::Entity entity)
{
    Logger::info("entity_state_update_callback_wrapper");
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_entity_state_update_callback.is_valid())
    {
        Dictionary entity_data;
        entity_data["hashed_keys"] = FieldElement::get_as_string(&entity.hashed_keys);
        entity_data["models"] = DojoArray(entity.models).get_value();
        entity_data["created_at"] = static_cast<int64_t>(entity.created_at);
        entity_data["updated_at"] = static_cast<int64_t>(entity.updated_at);
        entity_data["executed_at"] = static_cast<int64_t>(entity.executed_at);
        (void)singleton->on_entity_state_update_callback.call(entity_data);
    }
}

void event_message_update_callback_wrapper(DOJO::Entity entity)
{
    Logger::info("event_message_update_callback_wrapper");
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_event_message_update_callback.is_valid())
    {
        Dictionary message_data;
        message_data["hashed_keys"] = FieldElement::get_as_string(&entity.hashed_keys);
        message_data["models"] = DojoArray(entity.models).get_value();
        message_data["created_at"] = static_cast<int64_t>(entity.created_at);
        message_data["updated_at"] = static_cast<int64_t>(entity.updated_at);
        message_data["executed_at"] = static_cast<int64_t>(entity.executed_at);
        (void)singleton->on_event_message_update_callback.call(message_data);
    }
}

void starknet_event_callback_wrapper(DOJO::Event event)
{
    Logger::info("starknet_event_callback_wrapper");
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_starknet_event_callback.is_valid())
    {
        Dictionary event_data;
        event_data["keys"] = DojoArray(event.keys).get_value();
        event_data["data"] = DojoArray(event.data).get_value();
        event_data["transaction_hash"] = FieldElement::get_as_string(&event.transaction_hash);
        (void)singleton->on_starknet_event_callback.call(event_data);
    }
}

void transaction_callback_wrapper(DOJO::Transaction transaction)
{
    Logger::info("transaction_callback_wrapper");
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_transaction_callback.is_valid())
    {
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
        for (size_t i = 0; i < transaction.calls.data_len; ++i)
        {
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

void token_update_callback_wrapper(DOJO::Token token)
{
    Logger::info("token_update_callback_wrapper");
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_token_update_callback.is_valid())
    {
        Dictionary token_dict;
        token_dict["contract_address"] = FieldElement::get_as_string(&token.contract_address);
        if (token.token_id.tag == DOJO::SomeU256)
        {
            token_dict["token_id"] = String::num_uint64(token.token_id.some.data[0]);
        }
        else
        {
            token_dict["token_id"] = Variant();
        }
        token_dict["name"] = String(token.name);
        token_dict["symbol"] = String(token.symbol);
        token_dict["decimals"] = token.decimals;
        if (token.metadata)
        {
            token_dict["metadata"] = String(token.metadata);
        }
        else
        {
            token_dict["metadata"] = Variant();
        }
        (void)singleton->on_token_update_callback.call(token_dict);
    }
}

void contract_update_callback_wrapper(DOJO::Contract contract_update)
{
    Logger::info("contract_update_callback_wrapper");
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_contract_update_callback.is_valid())
    {
        Dictionary update_dict;
        update_dict["contract_address"] = FieldElement::get_as_string(&contract_update.contract_address);
        update_dict["contract_type"] = static_cast<int>(contract_update.contract_type);
        if (contract_update.head.tag == DOJO::Someu64)
        {
            update_dict["head"] = static_cast<int64_t>(contract_update.head.some);
        }
        if (contract_update.tps.tag == DOJO::Someu64)
        {
            update_dict["tps"] = static_cast<int64_t>(contract_update.tps.some);
        }
        if (contract_update.last_block_timestamp.tag == DOJO::Someu64)
        {
            update_dict["last_block_timestamp"] = static_cast<int64_t>(contract_update.last_block_timestamp.some);
        }
        if (contract_update.last_pending_block_tx.tag == DOJO::SomeFieldElement)
        {
            update_dict["last_pending_block_tx"] = FieldElement::get_as_string(&contract_update.last_pending_block_tx.some);
        }
        update_dict["updated_at"] = static_cast<int64_t>(contract_update.updated_at);
        update_dict["created_at"] = static_cast<int64_t>(contract_update.created_at);

        (void)singleton->on_contract_update_callback.call(update_dict);
    }
}

void token_balance_update_callback_wrapper(DOJO::TokenBalance token_balance)
{
    Logger::info("token_balance_update_callback_wrapper");
    ToriiClient* singleton = ToriiClient::get_singleton();
    if (singleton && singleton->on_token_balance_update_callback.is_valid())
    {
        Dictionary balance_dict;
        balance_dict["balance"] = String::num_uint64(token_balance.balance.data[0]);
        balance_dict["account_address"] = FieldElement::get_as_string(&token_balance.account_address);
        balance_dict["contract_address"] = FieldElement::get_as_string(&token_balance.contract_address);
        if (token_balance.token_id.tag == DOJO::SomeU256)
        {
            balance_dict["token_id"] = String::num_uint64(token_balance.token_id.some.data[0]);
        }
        else
        {
            balance_dict["token_id"] = Variant();
        }
        (void)singleton->on_token_balance_update_callback.call(balance_dict);
    }
}

// Subscription method implementations

void ToriiClient::on_entity_state_update(const Callable& callback, const Ref<EntitySubscription>& subscription)
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid())
    {
        Logger::error("Invalid entity subscription resource");
        return;
    }

    on_entity_state_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::ResultSubscription result = DOJO::client_on_entity_state_update(
        client, subscription->get_native_clause(), entity_state_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription)
    {
        String error_msg = "Failed to subscribe to entity state updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
        on_entity_state_update_callback = Callable();
    }
    else
    {
        Logger::success("Subscribed to entity state updates");
        emit_signal("subscription_created", "entity_state_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_event_message_update(const Callable& callback, const Ref<MessageSubscription>& subscription)
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid())
    {
        Logger::error("Invalid message subscription resource");
        return;
    }

    on_event_message_update_callback = callback;
    subscription->set_callback(callback);
    DOJO::ResultSubscription result = DOJO::client_on_event_message_update(
        client, subscription->get_native_clause(), event_message_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription)
    {
        String error_msg = "Failed to subscribe to event message updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
        on_event_message_update_callback = Callable();
    }
    else
    {
        Logger::success("Subscribed to event message updates");
        emit_signal("subscription_created", "event_message_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_starknet_event(const Callable& callback, const Ref<StarknetSubscription>& subscription)
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid())
    {
        Logger::error("Invalid starknet subscription resource");
        return;
    }

    this->on_starknet_event_callback = callback;
    subscription->set_callback(callback);
    DOJO::ResultSubscription result = DOJO::client_on_starknet_event(client, subscription->get_native_clause(), 1,
                                                                     starknet_event_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription)
    {
        String error_msg = "Failed to subscribe to starknet events: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
        this->on_starknet_event_callback = Callable();
    }
    else
    {
        Logger::success("Subscribed to starknet events");
        emit_signal("subscription_created", "starknet_event");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_transaction(const Callable& callback, const Ref<TransactionSubscription>& subscription)
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid())
    {
        Logger::error("Invalid transaction subscription resource");
        return;
    }

    this->on_transaction_callback = callback;
    subscription->set_callback(callback);
    DOJO::ResultSubscription result = DOJO::client_on_transaction(client, subscription->get_native_filter(),
                                                                  transaction_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription)
    {
        String error_msg = "Failed to subscribe to transactions: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
        this->on_transaction_callback = Callable();
    }
    else
    {
        Logger::success("Subscribed to transactions");
        emit_signal("subscription_created", "transaction");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_token_update(const Callable& callback, const Ref<TokenSubscription>& subscription)
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid())
    {
        Logger::error("Invalid token subscription resource");
        return;
    }

    on_token_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::CArrayFieldElement contracts = subscription->get_native_contract_addresses();
    DOJO::CArrayU256 token_ids = subscription->get_native_token_ids();

    DOJO::ResultSubscription result = DOJO::client_on_token_update(client, contracts.data, contracts.data_len,
                                                                   token_ids.data, token_ids.data_len,
                                                                   token_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription)
    {
        String error_msg = "Failed to subscribe to token updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
        on_token_update_callback = Callable();
    }
    else
    {
        Logger::success("Subscribed to token updates");
        emit_signal("subscription_created", "token_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_contract_update(const Callable& callback, const Ref<ContractSubscription>& subscription)
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid())
    {
        Logger::error("Invalid contract subscription resource");
        return;
    }

    on_contract_update_callback = callback;
    subscription->set_callback(callback);

    DOJO::ResultSubscription result = DOJO::on_contract_update(client, subscription->get_native_contract_address(),
                                                              contract_update_callback_wrapper);

    if (result.tag == DOJO::ErrSubscription)
    {
        String error_msg = "Failed to subscribe to contract updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
        on_contract_update_callback = Callable();
    }
    else
    {
        Logger::success("Subscribed to contract updates");
        emit_signal("subscription_created", "contract_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::on_token_balance_update(const Callable& callback, const Ref<TokenBalanceSubscription>& subscription)
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return;
    }
    if (!subscription.is_valid())
    {
        Logger::error("Invalid token balance subscription resource");
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

    if (result.tag == DOJO::ErrSubscription)
    {
        String error_msg = "Failed to subscribe to token balance updates: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
        on_token_balance_update_callback = Callable();
    }
    else
    {
        Logger::success("Subscribed to token balance updates");
        emit_signal("subscription_created", "token_balance_update");
        subscription->set_subscription(result.ok);
        subscriptions.append(subscription);
    }
}

void ToriiClient::update_subscription(const Ref<DojoSubscription>& subscription, const Callable& callback)
{
    using SubType = DojoSubscription::Type;
    Logger::info("Updating Subscription[color=Green]", subscription->get_name(), "[/color]");

    switch (subscription->get_type())
    {
    case SubType::ENTITY:
        update_entity_subscription(subscription, callback);
        break;
    case SubType::EVENT:
        update_event_message_subscription(subscription, callback);
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
        break;
    }
}

void ToriiClient::update_entity_subscription(const Ref<EntitySubscription>& subscription, const Callable& callback)
{
    DOJO::Resultbool result = DOJO::client_update_entity_subscription(client, subscription->get_subscription(),
                                                                      subscription->get_native_clause());
    if (result.tag == DOJO::Errbool)
    {
        String error_msg = "Failed to update entity subscription: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
    }
    else
    {
        Logger::success("Updated entity subscription");
        subscription->update_callback(callback);
    }
}

void ToriiClient::update_event_message_subscription(const Ref<MessageSubscription>& subscription,
                                                    const Callable& callback)
{
    DOJO::Resultbool result = DOJO::client_update_event_message_subscription(
        client, subscription->get_subscription(), subscription->get_native_clause());
    if (result.tag == DOJO::Errbool)
    {
        String error_msg = "Failed to update event message subscription: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
    }
    else
    {
        Logger::success("Updated event message subscription");
        subscription->update_callback(callback);
    }
}

void ToriiClient::update_starknet_event_subscription(const Ref<StarknetSubscription>& subscription,
                                                     const Callable& callback)
{
    Logger::warning("update_starknet_event_subscription not implemented on dojo.c side");
}

void ToriiClient::update_transaction_subscription(const Ref<TransactionSubscription>& subscription,
                                                  const Callable& callback)
{
    Logger::warning("update_transaction_subscription not implemented on dojo.c side");
}

void ToriiClient::update_token_subscription(const Ref<TokenSubscription>& subscription, const Callable& callback)
{
    Logger::warning("update_token_subscription not implemented on dojo.c side");
}

void ToriiClient::update_contract_subscription(const Ref<ContractSubscription>& subscription, const Callable& callback)
{
    Logger::warning("update_contract_subscription not implemented on dojo.c side");
}

void ToriiClient::update_token_balance_subscription(const Ref<TokenBalanceSubscription>& subscription,
                                                    const Callable& callback)
{
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
    if (result.tag == DOJO::Errbool)
    {
        String error_msg = "Failed to update token balance subscription: " + String(GET_DOJO_ERROR(result));
        Logger::error(error_msg);
        emit_signal("subscription_error", error_msg);
    }
    else
    {
        Logger::success("Updated token balance subscription");
        subscription->update_callback(callback);
    }
}
