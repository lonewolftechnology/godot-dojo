//
// Created by hazel on 27/05/25.
//

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"

#include "classes/torii_client.h"
#include "debug_macros.h"
#include "classes/event_subscription.h"
#include "variant/primitive.h"

#include "variant/ty/ty.h"

ToriiClient* ToriiClient::singleton = nullptr;

ToriiClient::ToriiClient()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        LOG_DEBUG_EXTRA("Torii", "Editor mode, ignoring constructor");
        return;
    }

    singleton = this;
    client = nullptr;
    is_connected = false;
    DOJO::ResultFieldElement test_felt = DOJO::cairo_short_string_to_felt("WP_GODOT_DEMO_ROOKIE");
    if (test_felt.tag == DOJO::ErrFieldElement)
    {
        LOG_ERROR("Error al crear FieldElement: ", GET_DOJO_ERROR(test_felt));
        return;
    }
    else
    {
        LOG_SUCCESS("FieldElement creado exitosamente");
        LOG_DEBUG(FieldElement::get_as_string(&GET_DOJO_OK(test_felt)));
    }
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

bool ToriiClient::create_client()
{
    disconnect_client();

    if (world_address.is_empty())
    {
        LOG_ERROR("Missing world address");
        emit_signal("client_connected", false);
        return false;
    }

    if (torii_url.is_empty())
    {
        LOG_ERROR("Missing torii url");
        emit_signal("client_connected", false);
        return false;
    }
    FieldElement world(world_address, 32);
    set_world(world);
    world.bytearray_deserialize();

    LOG_INFO("Creando cliente Torii...");
    LOG_INFO("URL: ", torii_url);
    LOG_INFO("World Address: ", world_address);

    DOJO::ResultToriiClient resClient = DOJO::client_new(
        torii_url.utf8().get_data(),
        world.get_felt_no_ptr()
    );

    if (resClient.tag == DOJO::ErrToriiClient)
    {
        LOG_ERROR("Error al crear cliente Torii: ", GET_DOJO_ERROR(resClient));
        emit_signal("client_connected", false);
        return false;
    }

    client = resClient.ok;
    is_connected = true;

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
        cancel_all_subscriptions();

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
        LOG_ERROR("Cliente no conectado");
        return {};
    }

    DOJO::ResultWorldMetadata resMetadata = DOJO::client_metadata(client);

    if (resMetadata.tag == DOJO::ErrWorldMetadata)
    {
        LOG_ERROR("Error al obtener metadatos: ", GET_DOJO_ERROR(resMetadata));
        return {};
    }

    DOJO::WorldMetadata metadata = GET_DOJO_OK(resMetadata);
    Dictionary result = {};

    TypedArray<Dictionary> models_array = {};
    std::vector<DOJO::CHashItemFieldElementModelMetadata> models_vec(
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

        Ty testing_ty = {model_metadata.schema};

        if (model_metadata.schema.tag == DOJO::Ty_Tag::Struct_)
        {
            Dictionary struct_info = {};
            DOJO::Struct meta_struct = model_metadata.schema.struct_;
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
    DOJO::ResultPageEntity resPageEntities = DOJO::client_entities(client, query);

    if (resPageEntities.tag == DOJO::ErrPageEntity)
    {
        LOG_ERROR("Error al obtener entidades: ", GET_DOJO_ERROR(resPageEntities));
        return {};
    }

    DOJO::PageEntity pageEntities = resPageEntities.ok;
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

    DOJO::ResultCArrayController resControllers = DOJO::client_controllers(
        client, &player_filter, 0
    );

    if (resControllers.tag == DOJO::ErrCArrayController)
    {
        LOG_ERROR("Error al obtener controladores: ", GET_DOJO_ERROR(resControllers));
        return {};
    }

    DOJO::CArrayController controllers = resControllers.ok;
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


bool ToriiClient::create_entity_subscription(const Callable& callback, const Dictionary& filter_params)
{
    if (!is_client_connected())
    {
        LOG_ERROR("Cliente no conectado");
        return false;
    }
    
    LOG_INFO("Creating entity subscription...");
    
    Ref<EventSubscription> entity_subscription;
    entity_subscription.instantiate();

    LOG_INFO("EventSubscription instance created successfully at: ", entity_subscription);
    
    DOJO::COptionClause event_clause = {};
    event_clause.tag = DOJO::COptionClause_Tag::NoneClause;
    
    bool result = entity_subscription->setup(this, event_clause, callback);
    
    if (result) {
        LOG_SUCCESS("Entity subscription created successfully");
    } else {
        LOG_ERROR("Failed to setup entity subscription");
    }
    
    return result;
}

bool ToriiClient::create_event_subscription(const Callable& callback, const Dictionary& filter_params)
{
    if (!is_client_connected())
    {
        LOG_ERROR("Cliente no conectado");
        return false;
    }

    LOG_INFO("Creating entity subscription...");

    Ref<EventSubscription> event_subscription;
    event_subscription.instantiate();

    LOG_INFO("EventSubscription instance created successfully at: ", event_subscription);

    DOJO::COptionClause event_clause = {};
    event_clause.tag = DOJO::COptionClause_Tag::NoneClause;

    bool result = event_subscription->setup(this, event_clause, callback);

    if (result) {
        LOG_SUCCESS("Entity subscription created successfully");
    } else {
        LOG_ERROR("Failed to setup entity subscription");
    }
    return result;
}

bool ToriiClient::create_token_subscription(const Callable& callback, const String& account_address)
{
    LOG_INFO("Creando suscripción de tokens para: ", account_address);
    return true;
}

void ToriiClient::cancel_all_subscriptions()
{
    LOG_INFO("Cancelando todas las suscripciones");
}

bool ToriiClient::publish_message(const String& message_data, const Array& signature_felts)
{
    if (!is_client_connected())
    {
        LOG_ERROR("Cliente no conectado");
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

void logger_callback_wrapper(const char* msg)
{
    LOG_DEBUG_EXTRA("Torii",msg);
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
    LOG_INFO("Query Creation");

    DOJO::Pagination pagination = {};
    LOG_INFO("Pagination Creation");
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
    LOG_DEBUG(pagination_dict.get("direction", "NO HABIA"));
    if (pagination_dict.get("direction", "forward") == "backward")
    {
        pagination.direction = DOJO::PaginationDirection::Backward;
    }
    else
    {
        pagination.direction = DOJO::PaginationDirection::Forward;
    }

    DOJO::CArrayOrderBy orderBy = {};
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

Dictionary ToriiClient::entity_to_dictionary(const DOJO::Entity& entity)
{
    Dictionary entity_dict = {};

    TypedArray<Dictionary> models_array = {};
    std::vector<DOJO::Struct> c_structs(
        entity.models.data, entity.models.data + entity.models.data_len
    );

    for (const auto& c_struct : c_structs)
    {
        Dictionary model_dict = {};
        model_dict["name"] = c_struct.name;

        Dictionary members_dict = {};
        std::vector<DOJO::Member> members(
            c_struct.children.data, c_struct.children.data + c_struct.children.data_len
        );

        for (const auto& member : members)
        {
            String member_name = member.name;
            Ty member_data = {member};
            if (member.ty->tag == DOJO::Ty_Tag::Primitive_)
            {
                DojoPrimitive primitive(member.ty->primitive);
                members_dict[member_name] = member_data.get_value();
            }
            else if (member.ty->tag == DOJO::Ty_Tag::Struct_)
            {
                Dictionary nested_struct = {};
                DOJO::Struct nested = member.ty->struct_;
                nested_struct["type"] = nested.name;

                if (String(nested.name) == "Vec2")
                {
                    Vector2 vec2 = Vector2(0, 0);
                    std::vector<DOJO::Member> nested_members(
                        nested.children.data, nested.children.data + nested.children.data_len
                    );

                    for (const auto& nested_member : nested_members)
                    {
                        if (nested_member.ty->tag == DOJO::Ty_Tag::Primitive_)
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

Dictionary ToriiClient::controller_to_dictionary(const DOJO::Controller& controller)
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