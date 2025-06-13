//
// Created by hazel on 27/05/25.
//

#include <classes/torii_client.h>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <debug_macros.h>
#include <variant/primitive.h>

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
    dojo_bindings::ResultToriiClient resClient = dojo_bindings::client_new(
        torii_url.utf8().get_data(),
        *world.get_felt()
    );

    if (resClient.tag == dojo_bindings::ErrToriiClient)
    {
        LOG_ERROR("Error al crear cliente Torii: ", resClient.err.message);
        emit_signal("client_connected", false);
        return false;
    }

    client = resClient.ok;
    is_connected = true;

    // Configurar logger
    dojo_bindings::client_set_logger(client, [](const char* msg)
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
        return Dictionary();
    }

    dojo_bindings::ResultWorldMetadata resMetadata = dojo_bindings::client_metadata(client);

    if (resMetadata.tag == dojo_bindings::ErrWorldMetadata)
    {
        LOG_ERROR("Error al obtener metadatos: ", resMetadata.err.message);
        return Dictionary();
    }

    dojo_bindings::WorldMetadata metadata = resMetadata.ok;
    Dictionary result = {};

    // Convertir modelos a Dictionary
    TypedArray<Dictionary> models_array = {};
    std::vector<dojo_bindings::CHashItemFieldElementModelMetadata> models_vec(
        metadata.models.data, metadata.models.data + metadata.models.data_len
    );

    for (const auto& model_item : models_vec)
    {
        Dictionary model_dict = {};
        dojo_bindings::ModelMetadata model_metadata = model_item.value;

        model_dict["namespace"] = model_metadata.namespace_;
        model_dict["name"] = model_metadata.name;
        model_dict["schema_type"] = String::num_int64(model_metadata.schema.tag);

        // Procesar esquema según el tipo
        if (model_metadata.schema.tag == dojo_bindings::Ty_Tag::Struct_)
        {
            Dictionary struct_info = {};
            dojo_bindings::Struct meta_struct = model_metadata.schema.struct_;
            struct_info["name"] = meta_struct.name;

            TypedArray<Dictionary> members_array = {};
            std::vector<dojo_bindings::Member> members(
                meta_struct.children.data, meta_struct.children.data + meta_struct.children.data_len
            );

            for (const auto& member : members)
            {
                Dictionary member_dict = {};
                member_dict["name"] = member.name;
                member_dict["type"] = String::num_int64(member.ty->tag);
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
    dojo_bindings::Query query = create_query_from_dict(query_params);
    LOG_DEBUG("Query COnverted");
    dojo_bindings::ResultPageEntity resPageEntities = dojo_bindings::client_entities(client, query);

    if (resPageEntities.tag == dojo_bindings::ErrPageEntity)
    {
        LOG_ERROR("Error al obtener entidades: ", resPageEntities.err.message);
        return TypedArray<Dictionary>();
    }

    dojo_bindings::PageEntity pageEntities = resPageEntities.ok;
    TypedArray<Dictionary> result = {};

    std::vector<dojo_bindings::Entity> entities(
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

    dojo_bindings::COptionFieldElement player_filter = {};

    if (!player_address.is_empty())
    {
        FieldElement player_felt(player_address, 32);
        player_filter.tag = dojo_bindings::SomeFieldElement;
        player_filter.some = *player_felt.get_felt();
    }
    else
    {
        player_filter.tag = dojo_bindings::NoneFieldElement;
    }

    dojo_bindings::ResultCArrayController resControllers = dojo_bindings::client_controllers(
        client, &player_filter.some, 0
    );

    if (resControllers.tag == dojo_bindings::ErrCArrayController)
    {
        LOG_ERROR("Error al obtener controladores: ", resControllers.err.message);
        return TypedArray<Dictionary>();
    }

    dojo_bindings::CArrayController controllers = resControllers.ok;
    TypedArray<Dictionary> result = {};

    std::vector<dojo_bindings::Controller> controller_list(
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
//     dojo_bindings::Query query = create_query_from_dict(query_params);
//
//     dojo_bindings::ResultPageToken resPageTokens = dojo_bindings::client_tokens(client, query);
//
//     if (resPageTokens.tag == dojo_bindings::ErrPageToken) {
//         LOG_ERROR("Error al obtener tokens: ", resPageTokens.err.message);
//         return TypedArray<Dictionary>();
//     }
//
//     dojo_bindings::PageToken pageTokens = resPageTokens.ok;
//     TypedArray<Dictionary> result = {};
//
//     std::vector<dojo_bindings::Token> tokens(
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
//     dojo_bindings::Query query = {};
//     // Configurar query para balances específicos
//
//     dojo_bindings::ResultPageTokenBalance resPageBalances = dojo_bindings::client_token_balances(client, query);
//
//     if (resPageBalances.tag == dojo_bindings::ErrPageTokenBalance) {
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
//     dojo_bindings::Query query = {};
//     // Configurar query para colecciones
//
//     dojo_bindings::ResultPageTokenCollection resPageCollections = dojo_bindings::client_token_collections(client, query);
//
//     if (resPageCollections.tag == dojo_bindings::ErrPageTokenCollection) {
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
    dojo_bindings::COptionClause event_clause = {};
    event_clause.tag = dojo_bindings::NoneClause;

    // Callback wrapper que llamará al callback de Godot
    auto wrapper = [this, callback](dojo_bindings::FieldElement entity_id, dojo_bindings::CArrayStruct models)
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

    dojo_bindings::ResultSubscription resSubscription = dojo_bindings::client_on_entity_state_update(
        client, event_clause, [](dojo_bindings::FieldElement entity_id, dojo_bindings::CArrayStruct models)
        {
            // Implementar callback apropiado
        }
    );

    if (resSubscription.tag == dojo_bindings::ErrSubscription)
    {
        LOG_ERROR("Error al crear suscripción de entidades: ", resSubscription.err.message);
        emit_signal("subscription_error", String(resSubscription.err.message));
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

    dojo_bindings::COptionClause event_clause = {};
    event_clause.tag = dojo_bindings::NoneClause;

    dojo_bindings::ResultSubscription resSubscription = dojo_bindings::client_on_event_message_update(
        client, event_clause, [](dojo_bindings::FieldElement entity_id, dojo_bindings::CArrayStruct models)
        {
            // Implementar callback apropiado
        }
    );

    if (resSubscription.tag == dojo_bindings::ErrSubscription)
    {
        LOG_ERROR("Error al crear suscripción de eventos: ", resSubscription.err.message);
        emit_signal("subscription_error", String(resSubscription.err.message));
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
    std::vector<dojo_bindings::FieldElement> felts;
    for (int i = 0; i < signature_felts.size(); i++)
    {
        Ref<FieldElement> felt_ref = signature_felts[i];
        if (felt_ref.is_valid())
        {
            felts.push_back(*felt_ref->get_felt());
        }
    }

    dojo_bindings::ResultFieldElement result = dojo_bindings::client_publish_message(
        client,
        message_data.utf8().get_data(),
        felts.data(),
        felts.size()
    );

    if (result.tag == dojo_bindings::ErrFieldElement)
    {
        LOG_ERROR("Error al publicar mensaje: ", result.err.message);
        return false;
    }

    FieldElement msg_hash(&result.ok);
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
    dojo_bindings::client_set_logger(client, client_logger);
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
dojo_bindings::Query ToriiClient::create_query_from_dict(const Dictionary& query_params) const
{
    dojo_bindings::Query query = {};
    LOG_INFO("Query Creation");

    // Configurar paginación
    dojo_bindings::Pagination pagination = {};
    LOG_INFO("Pagination Creation");
    Dictionary pagination_dict = query_params.get("pagination", {});
    pagination.limit = query_params.get("limit", 10);
    String cursor = pagination_dict.get("cursor", "");
    if (cursor.is_empty())
    {
        pagination.cursor.tag = dojo_bindings::COptionc_char_Tag::Nonec_char;
    }
    else
    {
        pagination.cursor.tag = dojo_bindings::COptionc_char_Tag::Somec_char;
        pagination.cursor.some = cursor.utf8().get_data();
    }
    // TODO: Ver una forma "mas elegante"
    LOG_DEBUG(pagination_dict.get("direction", "NO HABIA"));
    if (pagination_dict.get("direction", "forward") == "backward")
    {
        pagination.direction = dojo_bindings::PaginationDirection::Backward;
    }
    else
    {
        pagination.direction = dojo_bindings::PaginationDirection::Forward;
    }

    dojo_bindings::CArrayOrderBy orderBy = {};
    LOG_INFO("OrderBy Creation");
    Array orderBy_array = pagination_dict.get("order_by", {});
    if (!orderBy_array.is_empty())
    {
        orderBy.data_len = orderBy_array.size();
        orderBy.data = new dojo_bindings::OrderBy[orderBy.data_len];
        for (int i = 0; i < orderBy_array.size(); i++)
        {
            Dictionary data = orderBy_array[i];
            String model_name = data.get("model", "");
            String member_name = data.get("member", "");
            uint32_t direction = data.get("order_direction", 0);
            dojo_bindings::OrderDirection order_direction = static_cast<dojo_bindings::OrderDirection>(direction);
            orderBy.data[i] = {
                model_name.utf8().get_data(), member_name.utf8().get_data(), order_direction
            };
        }
    }
    pagination.order_by = orderBy;

    query.pagination = pagination;

    query.clause.tag = dojo_bindings::NoneClause;
    query.no_hashed_keys = query_params.get("no_hashed_keys", false);
    query.historical = query_params.get("historical", false);

    // Configurar modelos si se especifican
    dojo_bindings::CArrayc_char models = {};
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

dojo_bindings::Pagination ToriiClient::create_pagination_from_dict(const Dictionary& pagination_params) const
{
    dojo_bindings::Pagination pagination = {};

    pagination.limit = pagination_params.get("limit", 10);
    pagination.cursor.tag = dojo_bindings::COptionc_char_Tag::Nonec_char;

    String direction = pagination_params.get("direction", "forward");
    pagination.direction = (direction == "backward")
                               ? dojo_bindings::PaginationDirection::Backward
                               : dojo_bindings::PaginationDirection::Forward;

    dojo_bindings::CArrayOrderBy orderBy = {};
    pagination.order_by = orderBy;

    return pagination;
}

Dictionary ToriiClient::entity_to_dictionary(const dojo_bindings::Entity& entity) const
{
    Dictionary entity_dict = {};

    // Procesar modelos
    TypedArray<Dictionary> models_array = {};
    std::vector<dojo_bindings::Struct> c_structs(
        entity.models.data, entity.models.data + entity.models.data_len
    );

    for (const auto& c_struct : c_structs)
    {
        Dictionary model_dict = {};
        model_dict["name"] = c_struct.name;

        // Procesar miembros del struct
        Dictionary members_dict = {};
        std::vector<dojo_bindings::Member> members(
            c_struct.children.data, c_struct.children.data + c_struct.children.data_len
        );

        for (const auto& member : members)
        {
            String member_name = member.name;

            if (member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
            {
                DojoPrimitive primitive(member.ty->primitive);
                members_dict[member_name] = primitive.get_value();
            }
            else if (member.ty->tag == dojo_bindings::Ty_Tag::Struct_)
            {
                // Procesar structs anidados (como Vec2, etc.)
                Dictionary nested_struct = {};
                dojo_bindings::Struct nested = member.ty->struct_;
                nested_struct["type"] = nested.name;

                if (String(nested.name) == "Vec2")
                {
                    Vector2 vec2 = Vector2(0, 0);
                    std::vector<dojo_bindings::Member> nested_members(
                        nested.children.data, nested.children.data + nested.children.data_len
                    );

                    for (const auto& nested_member : nested_members)
                    {
                        if (nested_member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
                        {
                            DojoPrimitive nested_primitive(nested_member.ty->primitive);
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

Dictionary ToriiClient::controller_to_dictionary(const dojo_bindings::Controller& controller) const
{
    Dictionary controller_dict = {};
    dojo_bindings::FieldElement _felt = controller.address;
    FieldElement controller_addr{&_felt};
    controller_dict["address"] = controller_addr.to_string();
    controller_dict["username"] = controller.username;
    controller_dict["deployed_at_timestamp"] = controller.deployed_at_timestamp;

    return controller_dict;
}

// Dictionary ToriiClient::token_to_dictionary(const dojo_bindings::Token& token) const {
//     Dictionary token_dict = {};
//
//     // Implementar conversión de Token a Dictionary según la estructura disponible
//     // token_dict["address"] = ...;
//     // token_dict["name"] = ...;
//     // token_dict["symbol"] = ...;
//
//     return token_dict;
// }

// void ToriiClient::on_entity_update_internal(dojo_bindings::FieldElement entity_id, dojo_bindings::CArrayStruct models) {
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
// void ToriiClient::on_event_update_internal(dojo_bindings::Event event) {
//     // Procesar evento y emitir señal
//     Dictionary event_data = {};
//     // Convertir event a Dictionary
//     // ...
//
//     emit_signal("event_received", event_data);
// }
