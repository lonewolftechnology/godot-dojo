//
// Created by hazel on 27/05/25.
//

#include "classes/torii_client.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "debug_macros.h"
#include "variant/primitive.h"

#include "variant/ty/ty.h"

ToriiClient* ToriiClient::singleton = nullptr;

void ToriiClient::_bind_methods()
{
    // Métodos de conexión
    ClassDB::bind_method(D_METHOD("create_client", "world_addr", "torii_url"),
                         &ToriiClient::create_client, DEFVAL("http://localhost:8080"));
    ClassDB::bind_method(D_METHOD("disconnect_client"), &ToriiClient::disconnect_client);
    ClassDB::bind_method(D_METHOD("is_client_connected"), &ToriiClient::is_client_connected);
    ClassDB::bind_method(D_METHOD("get_torii_url"), &ToriiClient::get_torii_url);
    ClassDB::bind_method(D_METHOD("get_world_address"), &ToriiClient::get_world_address);

    // Metadatos
    ClassDB::bind_method(D_METHOD("get_world_metadata"), &ToriiClient::get_world_metadata);
    ClassDB::bind_method(D_METHOD("refresh_metadata"), &ToriiClient::refresh_metadata);

    // Consulta de entidades
    ClassDB::bind_method(D_METHOD("get_entities", "query_params"),
                         &ToriiClient::get_entities);

    // Controladores
    ClassDB::bind_method(D_METHOD("get_controllers", "player_address"),
                         &ToriiClient::get_controllers, DEFVAL(""));
    ClassDB::bind_method(D_METHOD("get_controller_info", "controller_address"),
                         &ToriiClient::get_controller_info);

    // Tokens
    // ClassDB::bind_method(D_METHOD("get_tokens", "query_params"),
    // &ToriiClient::get_tokens, DEFVAL(Dictionary()));
    // ClassDB::bind_method(D_METHOD("get_token_balances", "account_address"),
    // &ToriiClient::get_token_balances);
    // ClassDB::bind_method(D_METHOD("get_token_collections"), &ToriiClient::get_token_collections);
    // ClassDB::bind_method(D_METHOD("get_token_info", "token_address"), &ToriiClient::get_token_info);

    // Suscripciones
    ClassDB::bind_method(D_METHOD("create_entity_subscription", "callback", "filter_params"),
                         &ToriiClient::create_entity_subscription, DEFVAL(Dictionary()));
    ClassDB::bind_method(D_METHOD("create_event_subscription", "callback", "filter_params"),
                         &ToriiClient::create_event_subscription, DEFVAL(Dictionary()));
    ClassDB::bind_method(D_METHOD("create_token_subscription", "callback", "account_address"),
                         &ToriiClient::create_token_subscription);
    ClassDB::bind_method(D_METHOD("cancel_all_subscriptions"), &ToriiClient::cancel_all_subscriptions);

    // Publicación de mensajes
    ClassDB::bind_method(D_METHOD("publish_message", "message_data", "signature_felts"),
                         &ToriiClient::publish_message);
    ClassDB::bind_method(D_METHOD("publish_typed_message", "typed_data", "signature_felts"),
                         &ToriiClient::publish_typed_message);


    // Configuración
    ClassDB::bind_method(D_METHOD("set_logger_callback", "logger_callback"),
                         &ToriiClient::set_logger_callback);
    ClassDB::bind_method(D_METHOD("get_client_info"), &ToriiClient::get_client_info);

    // Utilidades
    ClassDB::bind_method(D_METHOD("get_connection_status"), &ToriiClient::get_connection_status);

    // Señales
    ADD_SIGNAL(MethodInfo("client_connected", PropertyInfo(Variant::BOOL, "success")));
    ADD_SIGNAL(MethodInfo("client_disconnected"));
    ADD_SIGNAL(MethodInfo("entity_updated", PropertyInfo(Variant::DICTIONARY, "entity_data")));
    ADD_SIGNAL(MethodInfo("event_received", PropertyInfo(Variant::DICTIONARY, "event_data")));
    ADD_SIGNAL(MethodInfo("subscription_error", PropertyInfo(Variant::STRING, "error_message")));
    ADD_SIGNAL(MethodInfo("metadata_updated", PropertyInfo(Variant::DICTIONARY, "metadata")));
    ADD_SIGNAL(MethodInfo("message_published", PropertyInfo(Variant::STRING, "message_hash")));
    ADD_SIGNAL(MethodInfo("transaction_confirmed", PropertyInfo(Variant::STRING, "transaction_hash")));

    // Enums
    BIND_ENUM_CONSTANT(ASC);
    BIND_ENUM_CONSTANT(DESC);
    BIND_ENUM_CONSTANT(FORWARD);
    BIND_ENUM_CONSTANT(BACKWARD);
}

ToriiClient::ToriiClient()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        UtilityFunctions::push_warning("ToriiClient está ejecutándose en modo editor");
        return;
    }

    singleton = this;
    client = nullptr;
    is_connected = false;
    torii_url = "";
    world_address = "";

    LOG_INFO("ToriiClient inicializado");
}

ToriiClient::~ToriiClient()
{
    disconnect_client();
    singleton = nullptr;
    LOG_INFO("ToriiClient destruido");
}

ToriiClient* ToriiClient::get_singleton()
{
    return singleton;
}

bool ToriiClient::create_client(const String& world_addr, const String& torii_url_param)
{
    // Limpiar cliente anterior si existe
    disconnect_client();

    // Convertir world address a FieldElement
    FieldElement world(world_addr, 32);
    set_world(world);
    world.bytearray_deserialize();
    torii_url = torii_url_param;
    world_address = world_addr;

    LOG_INFO("Creando cliente Torii...");
    LOG_INFO("URL: ", torii_url);
    LOG_INFO("World Address: ", world_address);

    // Crear cliente
    DOJO::Result<DOJO::ToriiClient*> resClient = DOJO::client_new(
        torii_url.utf8().get_data(),
        *world.get_felt()
    );

    if (resClient.IsErr())
    {
        LOG_ERROR("Error al crear cliente Torii: ", GET_DOJO_VALUE(resClient.err).message);
        emit_signal("client_connected", false);
        return false;
    }

    client = GET_DOJO_VALUE(resClient.ok);
    is_connected = true;

    // Configurar logger
    DOJO::client_set_logger(client, [](const char* msg)
    {
        LOG_INFO("ToriiClient: ", msg);
    });

    LOG_SUCCESS("Cliente Torii creado exitosamente");
    emit_signal("client_connected", true);
    return true;
}

void ToriiClient::disconnect_client()
{
    if (client != nullptr)
    {
        // Cancelar suscripciones
        cancel_all_subscriptions();

        // Aquí podrías agregar limpieza específica del cliente si es necesaria
        client = nullptr;
        is_connected = false;

        emit_signal("client_disconnected");
        LOG_INFO("Cliente Torii desconectado");
    }
}

bool ToriiClient::is_client_connected() const
{
    return is_connected && client != nullptr;
}

String ToriiClient::get_torii_url() const
{
    return torii_url;
}

String ToriiClient::get_world_address() const
{
    return world_address;
}


bool ToriiClient::is_calable_valid()
{
    return logger_callable.is_valid();
}

void ToriiClient::callable_call(const char* msg)
{
    if (is_calable_valid())
    {
        logger_callable.call(String(msg));
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
        LOG_ERROR("Cliente no conectado");
        return {};
    }

    DOJO::Result<DOJO::WorldMetadata> resMetadata = DOJO::client_metadata(client);

    if (resMetadata.IsErr())
    {
        LOG_ERROR("Error al obtener metadatos: ", GET_DOJO_VALUE(resMetadata.err).message);
        return {};
    }

    DOJO::WorldMetadata metadata = GET_DOJO_VALUE(resMetadata.ok);
    Dictionary result = {};

    // Convertir modelos a Dictionary
    TypedArray<Dictionary> models_array = {};
    std::vector<DOJO::CHashItem<DOJO::FieldElement, DOJO::ModelMetadata>> models_vec(
        metadata.models.data, metadata.models.data + metadata.models.data_len
    );

    for (const auto& model_item : models_vec)
    {

        Dictionary model_dict = {};
        DOJO::ModelMetadata model_metadata = model_item.value;

        LOG_DEBUG_EXTRA("METADATA",model_metadata.namespace_);
        LOG_DEBUG_EXTRA("METADATA",model_metadata.name);
        FieldElement key_felt = {model_item.key};
        LOG_DEBUG_EXTRA("HEX",key_felt.to_string());
        FieldElement testing_felt = {model_metadata.class_hash};
        LOG_DEBUG_EXTRA("HEX",testing_felt.to_string());
        FieldElement testing_felta = {model_metadata.contract_address};
        LOG_DEBUG_EXTRA("HEX",testing_felt.to_string());

        model_dict["namespace"] = model_metadata.namespace_;
        model_dict["name"] = model_metadata.name;
        model_dict["schema_type"] = String::num_int64(static_cast<int64_t>(model_metadata.schema.tag));
        LOG_DEBUG_EXTRA("SCHEMA",String::num_int64(static_cast<int64_t>(model_metadata.schema.tag)));

        // Procesar esquema según el tipo
        Ty testing_ty = {model_metadata.schema};

        if (model_metadata.schema.IsStruct_())
        {
            Dictionary struct_info = {};
            DOJO::Struct meta_struct = GET_DOJO_VALUE(model_metadata.schema.struct_);
            struct_info["name"] = meta_struct.name;
            LOG_DEBUG_EXTRA("STRUCT",meta_struct.name);

            TypedArray<Dictionary> members_array = {};
            std::vector<DOJO::Member> members(
                meta_struct.children.data, meta_struct.children.data + meta_struct.children.data_len
            );

            for (const auto& member : members)
            {
                Dictionary member_dict = {};
                member_dict["name"] = member.name;
                Ty member_ty = {*member.ty};

                LOG_DEBUG_EXTRA("MEMBER",member.name);
                LOG_DEBUG_EXTRA("TYPE",String::num_int64(static_cast<int64_t>(member.ty->tag)));
                member_dict["type"] = String::num_int64(static_cast<int64_t>(member.ty->tag));
                members_array.append(member_dict);
            }

            struct_info["members"] = members_array;
            model_dict["schema"] = struct_info;
        }

        models_array.append(model_dict);
    }

    result["models"] = models_array;
    result["world_address"] = world_address;

    LOG_SUCCESS("Metadatos obtenidos");
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
    LOG_INFO("Obteniendo entidades...");
    if (!is_client_connected())
    {
        LOG_ERROR("Cliente no conectado");
        return TypedArray<Dictionary>();
    }
    LOG_DEBUG("Converting Query");
    DOJO::Query query = create_query_from_dict(query_params);
    LOG_DEBUG("Query COnverted");
    DOJO::Result<DOJO::Page<DOJO::Entity>> resPageEntities = DOJO::client_entities(client, query);

    if (resPageEntities.IsErr())
    {
        LOG_ERROR("Error al obtener entidades: ", GET_DOJO_VALUE(resPageEntities.err).message);
        return {};
    }

    DOJO::Page<DOJO::Entity> pageEntities = GET_DOJO_VALUE(resPageEntities.ok);
    TypedArray<Dictionary> result = {};

    std::vector<DOJO::Entity> entities(
        pageEntities.items.data, pageEntities.items.data + pageEntities.items.data_len
    );

    for (const auto& entity : entities)
    {
        Dictionary entity_dict = entity_to_dictionary(entity);
        result.append(entity_dict);
    }

    LOG_SUCCESS("Entidades obtenidas: ", String::num_int64(result.size()));
    return result;
}


TypedArray<Dictionary> ToriiClient::get_controllers(const String& player_address)
{
    if (!is_client_connected())
    {
        LOG_ERROR("Cliente no conectado");
        return TypedArray<Dictionary>();
    }

    DOJO::FieldElement player_filter = {};

    if (!player_address.is_empty())
    {
        FieldElement player_felt(player_address, 32);
        player_filter = *player_felt.get_felt();
    }

    DOJO::Result<DOJO::CArray<DOJO::Controller>> resControllers = DOJO::client_controllers(
        client, &player_filter, 0
    );

    if (resControllers.IsErr())
    {
        LOG_ERROR("Error al obtener controladores: ", GET_DOJO_ERROR(resControllers));
        return {};
    }

    DOJO::CArray<DOJO::Controller> controllers = GET_DOJO_VALUE(resControllers.ok);
    TypedArray<Dictionary> result = {};

    std::vector<DOJO::Controller> controller_list(
        controllers.data, controllers.data + controllers.data_len
    );

    for (const auto& controller : controller_list)
    {
        Dictionary controller_dict = controller_to_dictionary(controller);
        result.append(controller_dict);
    }

    LOG_SUCCESS("Controladores obtenidos: ", String::num_int64(result.size()));
    return result;
}

Dictionary ToriiClient::get_controller_info(const String& controller_address)
{
    TypedArray<Dictionary> controllers = get_controllers();

    for (int i = 0; i < controllers.size(); i++)
    {
        Dictionary controller = controllers[i];
        if (controller.get("address", "") == controller_address)
        {
            return controller;
        }
    }

    return Dictionary();
}

// TypedArray<Dictionary> ToriiClient::get_tokens(const Dictionary& query_params) {
//     if (!is_client_connected()) {
//         LOG_ERROR("Cliente no conectado");
//         return TypedArray<Dictionary>();
//     }
//
//     // Crear query básico para tokens
//     DOJO::Query query = create_query_from_dict(query_params);
//
//     DOJO::ResultPageToken resPageTokens = DOJO::client_tokens(client, query);
//
//     if (resPageTokens.tag == DOJO::ErrPageToken) {
//         LOG_ERROR("Error al obtener tokens: ", resPageTokens.err.message);
//         return TypedArray<Dictionary>();
//     }
//
//     DOJO::PageToken pageTokens = resPageTokens.ok;
//     TypedArray<Dictionary> result = {};
//
//     std::vector<DOJO::Token> tokens(
//         pageTokens.items.data, pageTokens.items.data + pageTokens.items.data_len
//     );
//
//     for (const auto& token : tokens) {
//         Dictionary token_dict = token_to_dictionary(token);
//         result.append(token_dict);
//     }
//
//     LOG_SUCCESS("Tokens obtenidos: ", String::num_int64(result.size()));
//     return result;
// }
//
// TypedArray<Dictionary> ToriiClient::get_token_balances(const String& account_address) {
//     if (!is_client_connected()) {
//         LOG_ERROR("Cliente no conectado");
//         return TypedArray<Dictionary>();
//     }
//
//     // Convertir dirección a FieldElement
//     FieldElement account_felt(account_address, 32);
//
//     DOJO::Query query = {};
//     // Configurar query para balances específicos
//
//     DOJO::ResultPageTokenBalance resPageBalances = DOJO::client_token_balances(client, query);
//
//     if (resPageBalances.tag == DOJO::ErrPageTokenBalance) {
//         LOG_ERROR("Error al obtener balances: ", resPageBalances.err.message);
//         return TypedArray<Dictionary>();
//     }
//
//     // Procesar resultados
//     TypedArray<Dictionary> result = {};
//     // Implementar conversión de balances
//
//     return result;
// }
//
// TypedArray<Dictionary> ToriiClient::get_token_collections() {
//     if (!is_client_connected()) {
//         LOG_ERROR("Cliente no conectado");
//         return TypedArray<Dictionary>();
//     }
//
//     DOJO::Query query = {};
//     // Configurar query para colecciones
//
//     DOJO::ResultPageTokenCollection resPageCollections = DOJO::client_token_collections(client, query);
//
//     if (resPageCollections.tag == DOJO::ErrPageTokenCollection) {
//         LOG_ERROR("Error al obtener colecciones: ", resPageCollections.err.message);
//         return TypedArray<Dictionary>();
//     }
//
//     // Procesar resultados
//     TypedArray<Dictionary> result = {};
//     // Implementar conversión de colecciones
//
//     return result;
// }
//
// Dictionary ToriiClient::get_token_info(const String& token_address) {
//     TypedArray<Dictionary> tokens = get_tokens();
//
//     for (int i = 0; i < tokens.size(); i++) {
//         Dictionary token = tokens[i];
//         if (token.get("address", "") == token_address) {
//             return token;
//         }
//     }
//
//     return Dictionary();
// }

bool ToriiClient::create_entity_subscription(const Callable& callback, const Dictionary& filter_params)
{
    if (!is_client_connected())
    {
        LOG_ERROR("Cliente no conectado");
        return false;
    }

    // Crear filtro de cláusulas
    DOJO::COption<DOJO::Clause> event_clause = {};
    event_clause = DOJO::COption<DOJO::Clause>::None();

    // Callback wrapper que llamará al callback de Godot
    auto wrapper = [this, callback](DOJO::FieldElement entity_id, DOJO::CArray<DOJO::Struct> models)
    {
        Dictionary entity_data = {};
        FieldElement felt_id(&entity_id);
        entity_data["id"] = felt_id.to_string();

        // Procesar modelos
        // Implementar conversión de models a Dictionary

        if (callback.is_valid())
        {
            callback.call(entity_data);
        }

        emit_signal("entity_updated", entity_data);
    };

    DOJO::Result<DOJO::Subscription*> resSubscription = DOJO::client_on_entity_state_update(
        client, event_clause, [](DOJO::FieldElement entity_id, DOJO::CArray<DOJO::Struct> models)
        {
            // Implementar callback apropiado
        }
    );

    if (resSubscription.IsErr())
    {
        LOG_ERROR("Error al crear suscripción de entidades: ", GET_DOJO_ERROR(resSubscription));
        emit_signal("subscription_error", String(GET_DOJO_ERROR(resSubscription)));
        return false;
    }

    LOG_SUCCESS("Suscripción de entidades creada");
    return true;
}

bool ToriiClient::create_event_subscription(const Callable& callback, const Dictionary& filter_params)
{
    if (!is_client_connected())
    {
        LOG_ERROR("Cliente no conectado");
        return false;
    }

    DOJO::COption<DOJO::Clause> event_clause = {};
    event_clause = DOJO::COption<DOJO::Clause>::None();

    DOJO::Result<DOJO::Subscription*> resSubscription = DOJO::client_on_event_message_update(
        client, event_clause, [](DOJO::FieldElement entity_id, DOJO::CArray<DOJO::Struct> models)
        {
            // Implementar callback apropiado
        }
    );

    if (resSubscription.IsErr())
    {
        LOG_ERROR("Error al crear suscripción de eventos: ", GET_DOJO_ERROR(resSubscription));
        emit_signal("subscription_error", GET_DOJO_ERROR(resSubscription));
        return false;
    }

    LOG_SUCCESS("Suscripción de eventos creada");
    return true;
}

bool ToriiClient::create_token_subscription(const Callable& callback, const String& account_address)
{
    // Implementar suscripción específica para tokens
    LOG_INFO("Creando suscripción de tokens para: ", account_address);
    return true;
}

void ToriiClient::cancel_all_subscriptions()
{
    // Implementar cancelación de todas las suscripciones activas
    LOG_INFO("Cancelando todas las suscripciones");
}

bool ToriiClient::publish_message(const String& message_data, const Array& signature_felts)
{
    if (!is_client_connected())
    {
        LOG_ERROR("Cliente no conectado");
        return false;
    }

    // Convertir Array a array de FieldElement
    std::vector<DOJO::FieldElement> felts;
    for (int i = 0; i < signature_felts.size(); i++)
    {
        Ref<FieldElement> felt_ref = signature_felts[i];
        if (felt_ref.is_valid())
        {
            felts.push_back(*felt_ref->get_felt());
        }
    }

    DOJO::Result<DOJO::FieldElement> result = DOJO::client_publish_message(
        client,
        message_data.utf8().get_data(),
        felts.data(),
        felts.size()
    );

    if (result.IsErr())
    {
        LOG_ERROR("Error al publicar mensaje: ", GET_DOJO_ERROR(result));
        return false;
    }

    FieldElement msg_hash(GET_DOJO_OK(result));
    LOG_SUCCESS("Mensaje publicado: ", msg_hash.to_string());
    emit_signal("message_published", msg_hash.to_string());

    return true;
}

bool ToriiClient::publish_typed_message(const Dictionary& typed_data, const Array& signature_felts)
{
    String json_data = JSON::stringify(typed_data);
    return publish_message(json_data, signature_felts);
}

void client_logger(const char* msg)
{
    LOG_INFO(msg);
    ToriiClient* _singleton = ToriiClient::get_singleton();
    if (_singleton->is_calable_valid())
    {
        _singleton->callable_call(msg);
    }
}

void ToriiClient::set_logger_callback(const Callable& logger_callback)
{
    if (!is_client_connected()) return;
    logger_callable = logger_callback;
    DOJO::client_set_logger(client, client_logger);
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

// Métodos de conversión internos
DOJO::Query ToriiClient::create_query_from_dict(const Dictionary& query_params) const
{
    DOJO::Query query = {};
    LOG_INFO("Query Creation");

    // Configurar paginación
    DOJO::Pagination pagination = {};
    LOG_INFO("Pagination Creation");
    Dictionary pagination_dict = query_params.get("pagination", {});
    pagination.limit = query_params.get("limit", 10);
    String cursor = pagination_dict.get("cursor", "");
    if (cursor.is_empty())
    {
        pagination.cursor = DOJO::COption<const char*>::None();
    }
    else
    {
        pagination.cursor = DOJO::COption<const char*>::Some(cursor.utf8().get_data());
    }
    // TODO: Ver una forma "mas elegante"
    LOG_DEBUG(pagination_dict.get("direction", "NO HABIA"));
    if (pagination_dict.get("direction", "forward") == "backward")
    {
        pagination.direction = DOJO::PaginationDirection::Backward;
    }
    else
    {
        pagination.direction = DOJO::PaginationDirection::Forward;
    }

    DOJO::CArray<DOJO::OrderBy> orderBy = {};
    LOG_INFO("OrderBy Creation");
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

    query.clause = DOJO::COption<DOJO::Clause>::None();
    query.no_hashed_keys = query_params.get("no_hashed_keys", false);
    query.historical = query_params.get("historical", false);

    // Configurar modelos si se especifican
    DOJO::CArray<const char*> models = {};
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

DOJO::Pagination ToriiClient::create_pagination_from_dict(const Dictionary& pagination_params) const
{
    DOJO::Pagination pagination = {};

    pagination.limit = pagination_params.get("limit", 10);
    pagination.cursor = DOJO::COption<const char*>::None();

    String direction = pagination_params.get("direction", "forward");
    pagination.direction = (direction == "backward")
                               ? DOJO::PaginationDirection::Backward
                               : DOJO::PaginationDirection::Forward;

    DOJO::CArray<DOJO::OrderBy> orderBy = {};
    pagination.order_by = orderBy;

    return pagination;
}

Dictionary ToriiClient::entity_to_dictionary(const DOJO::Entity& entity) const
{
    Dictionary entity_dict = {};

    // Procesar modelos
    TypedArray<Dictionary> models_array = {};
    std::vector<DOJO::Struct> c_structs(
        entity.models.data, entity.models.data + entity.models.data_len
    );

    for (const auto& c_struct : c_structs)
    {
        Dictionary model_dict = {};
        model_dict["name"] = c_struct.name;

        // Procesar miembros del struct
        Dictionary members_dict = {};
        std::vector<DOJO::Member> members(
            c_struct.children.data, c_struct.children.data + c_struct.children.data_len
        );

        for (const auto& member : members)
        {
            String member_name = member.name;

            if (member.ty->IsPrimitive_())
            {
                DojoPrimitive primitive(GET_DOJO_VALUE(member.ty->primitive));
                members_dict[member_name] = primitive.get_value();
            }
            else if (member.ty->IsStruct_())
            {
                // Procesar structs anidados (como Vec2, etc.)
                Dictionary nested_struct = {};
                DOJO::Struct nested = GET_DOJO_VALUE(member.ty->struct_);
                nested_struct["type"] = nested.name;

                if (String(nested.name) == "Vec2")
                {
                    Vector2 vec2 = Vector2(0, 0);
                    std::vector<DOJO::Member> nested_members(
                        nested.children.data, nested.children.data + nested.children.data_len
                    );

                    for (const auto& nested_member : nested_members)
                    {
                        if (nested_member.ty->IsPrimitive_())
                        {
                            DojoPrimitive nested_primitive(GET_DOJO_VALUE(nested_member.ty->primitive));
                            real_t value = nested_primitive.get_value();

                            if (String(nested_member.name) == "x")
                            {
                                vec2.x = value;
                            }
                            else if (String(nested_member.name) == "y")
                            {
                                vec2.y = value;
                            }
                        }
                    }
                    members_dict[member_name] = vec2;
                }
                else
                {
                    members_dict[member_name] = nested_struct;
                }
            }
        }

        model_dict["data"] = members_dict;
        models_array.append(model_dict);
    }

    entity_dict["models"] = models_array;
    return entity_dict;
}

Dictionary ToriiClient::controller_to_dictionary(const DOJO::Controller& controller) const
{
    Dictionary controller_dict = {};
    DOJO::FieldElement _felt = controller.address;
    FieldElement controller_addr{&_felt};
    controller_dict["address"] = controller_addr.to_string();
    controller_dict["username"] = controller.username;
    controller_dict["deployed_at_timestamp"] = controller.deployed_at_timestamp;

    return controller_dict;
}

// Dictionary ToriiClient::token_to_dictionary(const DOJO::Token& token) const {
//     Dictionary token_dict = {};
//
//     // Implementar conversión de Token a Dictionary según la estructura disponible
//     // token_dict["address"] = ...;
//     // token_dict["name"] = ...;
//     // token_dict["symbol"] = ...;
//
//     return token_dict;
// }

// void ToriiClient::on_entity_update_internal(DOJO::FieldElement entity_id, DOJO::CArrayStruct models) {
//     // Procesar actualización de entidad y emitir señal
//     Dictionary entity_data = {};
//     FieldElement felt_id(&entity_id);
//     entity_data["id"] = felt_id.to_string();
//
//     // Procesar modelos
//     // ...
//
//     emit_signal("entity_updated", entity_data);
// }
//
// void ToriiClient::on_event_update_internal(DOJO::Event event) {
//     // Procesar evento y emitir señal
//     Dictionary event_data = {};
//     // Convertir event a Dictionary
//     // ...
//
//     emit_signal("event_received", event_data);
// }
