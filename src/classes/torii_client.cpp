//
// Created by hazel on 27/05/25.
//

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"

#include "classes/torii_client.h"
#include "tools/logger.h"
#include "classes/event_subscription.h"
#include "variant/ty/dojo_array.h"
#include "variant/ty/primitive.h"
#include "variant/ty/struct.h"

#include "variant/ty/ty.h"

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
    world.bytearray_deserialize();

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

    DOJO::ResultWorldMetadata resMetadata = DOJO::client_metadata(client);

    if (resMetadata.tag == DOJO::ErrWorldMetadata)
    {
        return Logger::error_dict("Error al obtener metadatos: ", GET_DOJO_ERROR(resMetadata));
    }

    DOJO::WorldMetadata metadata = GET_DOJO_OK(resMetadata);
    Dictionary result = {};

    TypedArray<Dictionary> models_array = ArrayDojo(metadata.models).get_value();

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
        return TypedArray<Dictionary>();
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
    TypedArray<Dictionary> result = ArrayDojo(pageEntities.items).get_value();

    Logger::success("Entidades obtenidas: ", String::num_int64(result.size()));
    return result;
}


TypedArray<Dictionary> ToriiClient::get_controllers(const TypedArray<String>& addresses = Array())
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
    std::vector<dojo_bindings::FieldElement> contract_addresses = FieldElement::create_array(addresses);
    Logger::debug_extra("TORII CLIENT", "Address size: ", contract_addresses.size());
    DOJO::ResultCArrayController resControllers = DOJO::client_controllers(
        client, contract_addresses.data(), contract_addresses.size()
    );

    if (resControllers.tag == DOJO::ErrCArrayController)
    {
        Logger::error("Error obtaining controllers: ", GET_DOJO_ERROR(resControllers));
        return {Logger::error_dict("Error obtaining controllers: ", GET_DOJO_ERROR(resControllers))};
    }

    DOJO::CArrayController controllers = resControllers.ok;
    TypedArray<Dictionary> result = ArrayDojo(controllers).get_value();

    Logger::success("Controllers obtained: ", String::num_int64(result.size()));
    return result;
}

Dictionary ToriiClient::get_controller_info(const String& controller_address)
{
    TypedArray<Dictionary> controllers = get_controllers(Array());

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


bool ToriiClient::create_entity_subscription(const Callable& callback, const Dictionary& filter_params)
{
    if (!is_client_connected())
    {
        Logger::error("Cliente no conectado");
        return false;
    }

    Logger::custom("Entity Sub","Creating entity subscription...");

    Ref entity_subscription = memnew(EventSubscription());
    entity_subscription.instantiate();
    entity_subscription->set_name("entity");
    Logger::info("EventSubscription instance created successfully at: ", entity_subscription, entity_subscription->get_name());

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

    Logger::custom("Event Sub","Creating Event subscription...");

    Ref event_subscription = memnew(EventSubscription());;
    event_subscription.instantiate();
    event_subscription->set_name("event");

    Logger::info("EventSubscription instance created successfully at: ", event_subscription, event_subscription->get_name());

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
    Logger::info("[TODO] Creating token subscription for: ", account_address);
    return true;
}

void ToriiClient::cancel_all_subscriptions()
{
    Logger::debug_extra("ToriiClient", "[TODO] Cancelling all subscription");
}

bool ToriiClient::publish_message(const String& message_data, const Array& signature_felts)
{
    if (!is_client_connected())
    {
        Logger::error("Cliente no conectado");
        return false;
    }

    std::vector<DOJO::FieldElement> felts;
    for (int i = 0; i < signature_felts.size(); i++)
    {
        Ref<FieldElement> felt_ref = signature_felts[i];
        if (felt_ref.is_valid())
        {
            felts.push_back(*felt_ref->get_felt());
        }
    }

    DOJO::ResultFieldElement result = DOJO::client_publish_message(
        client,
        message_data.utf8().get_data(),
        felts.data(),
        felts.size()
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
    Logger::debug_extra("Torii", msg);
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
    pagination.limit = query_params.get("limit", 10);
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
            String model_name = data.get("model", "");
            String member_name = data.get("member", "");
            uint32_t direction = data.get("order_direction", 0);
            DOJO::OrderDirection order_direction = static_cast<DOJO::OrderDirection>(direction);
            orderBy.data[i] = {
                model_name.utf8().get_data(), member_name.utf8().get_data(), order_direction
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

    pagination.limit = pagination_params.get("limit", 10);
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
