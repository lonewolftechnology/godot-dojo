//
// Created by hazel on 27/05/25.
//

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"

#include "classes/torii_client.h"

#include "tools/dojo_helper.h"

#include "tools/logger.h"
#include "classes/event_subscription.h"
#include "variant/ty/dojo_array.h"
#include "variant/ty/primitive.h"

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
    DOJO::ResultFieldElement test_felt = DOJO::cairo_short_string_to_felt("WP_GODOT_DEMO_ROOKIE");
    if (test_felt.tag == DOJO::ErrFieldElement)
    {
        Logger::error("Error al crear FieldElement: ", GET_DOJO_ERROR(test_felt));
        return;
    }
    else
    {
        Logger::success("FieldElement creado exitosamente");
        Logger::debug(FieldElement::get_as_string(&GET_DOJO_OK(test_felt)));
    }
    Logger::info("ToriiClient inicializado");
}

ToriiClient::~ToriiClient()
{
    disconnect_client(false);
    singleton = nullptr;
    Logger::info("ToriiClient destruido");
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
    FieldElement world(world_address, 32);
    set_world(world);

    Logger::info("Creando cliente Torii...");
    Logger::info("URL: ", torii_url);
    Logger::info("World Address: ", world_address);

    DOJO::ResultToriiClient resClient = DOJO::client_new(
        torii_url.utf8().get_data(),
        world.get_felt_no_ptr()
    );

    if (resClient.tag == DOJO::ErrToriiClient)
    {
        Logger::error("Error al crear cliente Torii: ", GET_DOJO_ERROR(resClient));
        emit_signal("client_connected", false);
        return false;
    }

    client = resClient.ok;
    is_connected = true;

    DOJO::client_set_logger(client, [](const char* msg)
    {
        Logger::info("ToriiClient: ", msg);
    });

    Logger::success("Cliente Torii creado exitosamente");
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
        Logger::info("Cliente Torii desconectado");
    }
}

bool ToriiClient::is_client_connected() const
{
    return is_connected && client != nullptr;
}

bool ToriiClient::is_calable_valid()
{
    return logger_callback.is_valid();
}

void ToriiClient::callable_call(const char* msg)
{
    if (is_calable_valid())
    {
        logger_callback.call(String(msg));
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

Dictionary ToriiClient::get_world_metadata()
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return Logger::error_dict("Client not connected");
    }

    DOJO::ResultWorld resMetadata = DOJO::client_metadata(client);

    if (resMetadata.tag == DOJO::ErrWorld)
    {
        return Logger::error_dict("Error al obtener metadatos: ", GET_DOJO_ERROR(resMetadata));
    }

    DOJO::World metadata = GET_DOJO_OK(resMetadata);
    Dictionary result = {};

    TypedArray<Dictionary> models_array = DojoArray(metadata.models).get_value();

    result["models"] = models_array;
    result["world_address"] = world_address;

    Logger::success("Metadatos obtenidos");
    emit_signal("metadata_updated", result);

    return result;
}

bool ToriiClient::refresh_metadata()
{
    Dictionary metadata = get_world_metadata();
    return !metadata.is_empty();
}

TypedArray<Dictionary> ToriiClient::get_entities(const Dictionary& query_params)
{
    Logger::info("Obteniendo entidades...");
    if (!is_client_connected())
    {
        Logger::error("Cliente no conectado");
        return {};
    }
    Logger::debug("Converting Query");
    DOJO::Query query = create_query_from_dict(query_params);
    DOJO::ResultPageEntity resPageEntities = DOJO::client_entities(client, query);

    if (resPageEntities.tag == DOJO::ErrPageEntity)
    {
        Logger::error("Error al obtener entidades: ", GET_DOJO_ERROR(resPageEntities));
        return {};
    }

    DOJO::PageEntity pageEntities = resPageEntities.ok;
    TypedArray<Dictionary> result = DojoArray(pageEntities.items).get_value();

    Logger::success("Entidades obtenidas: ", String::num_int64(result.size()));
    return result;
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
    Logger::debug_extra("Torii", addresses);
    std::vector<DOJO::FieldElement> contract_addresses_vec = FieldElement::create_array(addresses);
    Logger::debug_extra("CLIENT", "Address size: ", contract_addresses_vec.size());

    // Null for now to retrive all controllers
    DOJO::ControllerQuery controller_query = {};

    // Inicializar correctamente la paginación
    controller_query.pagination.cursor.tag = DOJO::COptionc_char_Tag::Nonec_char;
    controller_query.pagination.limit.tag = DOJO::COptionu32_Tag::Noneu32;
    controller_query.pagination.order_by = {nullptr, 0};
    controller_query.pagination.direction = DOJO::PaginationDirection::Forward;

    // Inicializar correctamente los CArrays
    controller_query.contract_addresses = {contract_addresses_vec.data(), contract_addresses_vec.size()};
    Logger::debug_extra("Usernames", usernames);
    DojoArray::CStringArrayHelper username(usernames);
    controller_query.usernames = username.c_array;


    DOJO::ResultPageController resControllers = DOJO::client_controllers(
        client, controller_query);

    if (resControllers.tag == DOJO::ErrPageController)
    {
        Logger::error("Error obtaining controllers: ", GET_DOJO_ERROR(resControllers));
        return {Logger::error_dict("Error obtaining controllers: ", GET_DOJO_ERROR(resControllers))};
    }

    DOJO::PageController controllers = GET_DOJO_OK(resControllers);
    TypedArray<Dictionary> result = DojoArray(controllers).get_value();

    Logger::success_extra("CLIENT", "Controllers obtained: ", String::num_int64(result.size()));
    return result;
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

TypedArray<Dictionary> ToriiClient::get_tokens(const Dictionary& query_params)
{
    Logger::info("Getting tokens...");
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return {};
    }

    // Extract parameters from query_params
    uint32_t limit = query_params.get("limit", 10);
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
        Logger::error("Error getting tokens: ", GET_DOJO_ERROR(result));
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

TypedArray<Dictionary> ToriiClient::get_token_balances(const String& account_address)
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
        Logger::error("Error getting token balances: ", GET_DOJO_ERROR(result));
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

TypedArray<Dictionary> ToriiClient::get_token_collections()
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
        Logger::error("Error getting token collections: ", GET_DOJO_ERROR(result));
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

Dictionary ToriiClient::get_token_info(const String& token_address)
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
        Logger::error("Error getting token info: ", GET_DOJO_ERROR(result));
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


bool ToriiClient::create_entity_subscription(const Callable& callback, const Dictionary& filter_params)
{
    if (!is_client_connected())
    {
        Logger::error("Cliente no conectado");
        return false;
    }

    Logger::custom("Entity Sub", "Creating entity subscription...");

    Ref entity_subscription = memnew(EventSubscription());
    entity_subscription.instantiate();
    entity_subscription->set_name("entity");
    Logger::info("EventSubscription instance created successfully at: ", entity_subscription,
                 entity_subscription->get_name());

    DOJO::COptionClause event_clause = {};
    event_clause.tag = DOJO::COptionClause_Tag::NoneClause;

    bool result = entity_subscription->setup(this, event_clause, callback);

    if (result)
    {
        Logger::success("Entity subscription created successfully");
        emit_signal("subscription_created", entity_subscription->get_name());
        event_subscriptions.append(entity_subscription);
    }
    else
    {
        Logger::error("Failed to setup entity subscription");
    }

    return result;
}

bool ToriiClient::create_event_subscription(const Callable& callback, const Dictionary& filter_params)
{
    if (!is_client_connected())
    {
        Logger::error("Cliente no conectado");
        return false;
    }

    Logger::custom("Event Sub", "Creating Event subscription...");

    Ref event_subscription = memnew(EventSubscription());;
    event_subscription.instantiate();
    event_subscription->set_name("event");

    Logger::info("EventSubscription instance created successfully at: ", event_subscription,
                 event_subscription->get_name());

    DOJO::COptionClause event_clause = {};
    event_clause.tag = DOJO::COptionClause_Tag::NoneClause;

    bool result = event_subscription->setup(this, event_clause, callback);

    if (result)
    {
        Logger::success("Entity subscription created successfully");
        emit_signal("subscription_created", event_subscription->get_name());
        event_subscriptions.append(event_subscription);
    }
    else
    {
        Logger::error("Failed to setup entity subscription");
    }
    return result;
}

bool ToriiClient::create_token_subscription(const Callable& callback, const String& account_address)
{
    if (!is_client_connected())
    {
        Logger::error("Client not connected");
        return false;
    }

    Logger::custom("Token Sub", "Creating token subscription for: ", account_address);

    Ref event_subscription = memnew(EventSubscription());
    event_subscription.instantiate();
    event_subscription->set_name("token");

    Logger::info("TokenSubscription instance created successfully at: ", event_subscription,
                 event_subscription->get_name());

    // Convert account address to FieldElement
    FieldElement account_felt(account_address, 32);

    // Create empty arrays for contract addresses and token IDs
    DOJO::FieldElement* contract_addresses = nullptr;
    size_t contract_addresses_len = 0;
    DOJO::FieldElement* account_addresses = nullptr;
    size_t account_addresses_len = 0;
    DOJO::U256* token_ids = nullptr;
    size_t token_ids_len = 0;

    // If account address is provided, add it to the filter
    if (!account_address.is_empty())
    {
        account_addresses = account_felt.get_felt();
        account_addresses_len = 1;
    }

    // Create token balance subscription
    DOJO::ResultSubscription result_sub = DOJO::client_on_token_balance_update(
        client,
        contract_addresses, contract_addresses_len,
        account_addresses, account_addresses_len,
        token_ids, token_ids_len,
        [](DOJO::TokenBalance token_balance)
        {
            // Convert token balance to Dictionary
            Dictionary balance_dict;
            balance_dict["balance"] = String::num_uint64(token_balance.balance.data[0]);
            if (token_balance.token_id.tag == DOJO::SomeU256)
            {
                balance_dict["token_id"] = String::num_uint64(token_balance.token_id.some.data[0]);
            }else
            {
                balance_dict["token_id"] = nullptr;
            }
            balance_dict["account_address"] = FieldElement::get_as_string(&token_balance.account_address);
            balance_dict["contract_address"] = FieldElement::get_as_string(&token_balance.contract_address);

            singleton->emit_signal("token_balance_updated", balance_dict);
        }
    );

    if (result_sub.tag == DOJO::ErrSubscription)
    {
        Logger::error("Failed to create token subscription: ", GET_DOJO_ERROR(result_sub));
        return false;
    }

    // Store subscription
    event_subscription->set_subscription(result_sub.ok);
    event_subscriptions.append(event_subscription);

    Logger::success("Token subscription created successfully");
    emit_signal("subscription_created", event_subscription->get_name());

    return true;
}

void ToriiClient::cancel_all_subscriptions()
{
    Logger::info("Cancelling all subscriptions");

    // Iterate through all subscriptions and cancel them
    for (int i = 0; i < event_subscriptions.size(); i++)
    {
        Ref<EventSubscription> subscription = event_subscriptions[i];
        if (subscription.is_valid())
        {
            Logger::debug("[TODO] Cancelling subscription: ", subscription->get_name());
            // subscription->cancel(); // TODO
        }
    }

    // Clear the subscriptions array
    event_subscriptions.clear();

    Logger::success("All subscriptions cancelled");
}

bool ToriiClient::publish_message(const String& message_data, const Array& signature_felts)
{
    Logger::info("publish_message is experimental");
    if (!is_client_connected())
    {
        Logger::error("Cliente no conectado");
        return false;
    }

    DOJO::FieldElement sign_key = DOJO::signing_key_new();
    DOJO::FieldElement hash = DOJO::verifying_key_new(sign_key);
    DOJO::ResultSignature resSig = DOJO::signing_key_sign(sign_key, hash);
    if (resSig.tag == DOJO::ErrSignature)
    {
        Logger::error("Error al firmar mensaje: ", GET_DOJO_ERROR(resSig));
        return false;
    }
    Logger::success_extra("Message", message_data);
    DOJO::Signature signature = GET_DOJO_OK(resSig);
    DOJO::FieldElement* cfelts = static_cast<DOJO::FieldElement*>(malloc(sizeof(DOJO::FieldElement) * 2));
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
        Logger::error("Error al publicar mensaje: ", GET_DOJO_ERROR(result));
        return false;
    }

    FieldElement msg_hash(GET_DOJO_OK(result));
    Logger::success("Mensaje publicado: ", msg_hash.to_string());
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
    if (_singleton->is_calable_valid())
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
        int _limit = query_params.get("limit", 10);
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
            DOJO::OrderDirection order_direction = static_cast<DOJO::OrderDirection>(direction);
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
        int _limit = pagination_params.get("limit", 10);
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

// Dictionary ToriiClient::token_to_dictionary(const DOJO::Token& token) const {
//     Dictionary token_dict = {};
//
//     // token_dict["address"] = ...;
//     // token_dict["name"] = ...;
//     // token_dict["symbol"] = ...;
//
//     return token_dict;
// }

// void ToriiClient::on_entity_update_internal(DOJO::FieldElement entity_id, DOJO::CArrayStruct models) {
//     Dictionary entity_data = {};
//     FieldElement felt_id(&entity_id);
//     entity_data["id"] = felt_id.to_string();
//
//
//     emit_signal("entity_updated", entity_data);
// }
//
// void ToriiClient::on_event_update_internal(DOJO::Event event) {
//     Dictionary event_data = {};
//
//     emit_signal("event_received", event_data);
// }
