//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/time.hpp>

#include <debug_macros.h>
#include <variant/primitive.h>


using namespace godot;
dojo_bindings::ToriiClient* client;
DojoC* DojoC::singleton = nullptr;
dojo_bindings::ControllerAccount* session_account;
dojo_bindings::Provider* controller_provider;
dojo_bindings::Account* account;
dojo_bindings::FieldElement* actions;
dojo_bindings::FieldElement priv;

void DojoC::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoC::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &DojoC::get_enabled);
    ClassDB::bind_method(D_METHOD("get_controllers"), &DojoC::get_controllers);
    ClassDB::bind_method(D_METHOD("get_entities"), &DojoC::get_entities);
    ClassDB::bind_method(D_METHOD("client_metadata"), &DojoC::client_metadata);
    ClassDB::bind_method(D_METHOD("spawn"), &DojoC::spawn);
    ClassDB::bind_method(D_METHOD("move"), &DojoC::move);
    ClassDB::bind_method(D_METHOD("get_username"), &DojoC::get_username);
    ClassDB::bind_method(D_METHOD("get_session_packed_byte"), &DojoC::get_session_packed_byte);
    ClassDB::bind_method(D_METHOD("get_session_address"), &DojoC::get_session_address);
    ClassDB::bind_method(D_METHOD("create_entity_subscription", "p_callable"), &DojoC::create_entity_subscription);
    ClassDB::bind_method(D_METHOD("entity_subscription", "p_callable"), &DojoC::entity_subscription);
    ClassDB::bind_method(
        D_METHOD(
            "controller_new",
            "p_controller_addr",
            "p_relay_rul"),
        &DojoC::controller_new,
        DEFVAL("http://localhost:5050")
    );

    // ClassDB::bind_method(D_METHOD("client_new"), &DojoC::client_new);
    ClassDB::bind_method(
        D_METHOD("create_client", "p_world", "p_torii_url"),
        &DojoC::create_client,
        DEFVAL("http://localhost:8080")
    );
    // ClassDB::bind_method(D_METHOD("controller_connect"), &DojoC::controller_connect);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");

    ClassDB::bind_method(D_METHOD("get_message"), &DojoC::get_output_message);
    ClassDB::bind_method(D_METHOD("set_message", "p_message"), &DojoC::set_output_message);

    ClassDB::bind_method(D_METHOD("send_message", "p_message"), &DojoC::send_message);

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "output_message", PROPERTY_HINT_ARRAY_TYPE), "set_message",
                 "get_message");

    ADD_SIGNAL(MethodInfo("event_update", PropertyInfo(Variant::STRING, "msg")));
    ADD_SIGNAL(MethodInfo("controller_account_status_updated", PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(MethodInfo("account_status_updated", PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(MethodInfo("provider_status_updated", PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(MethodInfo("client_status_updated", PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(MethodInfo("entities_status_updated", PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(MethodInfo("subscription_status_updated",PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(MethodInfo("on_account"));
}

DojoC::DojoC()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        UtilityFunctions::push_warning("DojoC is running in editor mode");
        return;
    }
    // Initialize any variables here.
    singleton = this;
    enabled = true;
    controller_provider = nullptr;
    account = nullptr;
    FieldElement action_felt{"0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf", 32};
    actions = action_felt.get_felt();
}

DojoC* DojoC::get_singleton()
{
    return singleton;
}

DojoC::~DojoC()
{
    // Add your cleanup here.
    singleton = nullptr;
}

void print_felt(String fname, const dojo_bindings::FieldElement felt)
{
    dojo_bindings::Resultc_char testing = dojo_bindings::parse_cairo_short_string(felt);
    if (testing.tag == dojo_bindings::Resultc_char_Tag::Errc_char)
    {
        LOG_ERROR("NO STRING");
    }
    else
    {
        LOG_SUCCESS("Felt:", fname, testing.ok);
    }
}

// que devuelva un msg es testing, no deberia devolver nada
void DojoC::create_client(const String& world_addr,
                          String torii_url = "http://localhost:8080")
{
    UtilityFunctions::print("Attempting to create client...");

    FieldElement world = {world_addr, 32};
    world.bytearray_deserialize();

    LOG_INFO("RPC Endpoint: ", torii_url);

    dojo_bindings::ResultToriiClient resClient = dojo_bindings::client_new(
        torii_url.utf8().get_data(),
        *world.get_felt());

    if (resClient.tag == dojo_bindings::ErrToriiClient)
    {
        LOG_ERROR(resClient.err.message);
        emit_signal("client_status_updated", false);
    }
    else
    {
        client = resClient.ok;
        LOG_SUCCESS("Client created");
        dojo_bindings::client_set_logger(client, [](const char* msg)
        {
            LOG_INFO("ToriiClient log: ", msg);
        });
        LOG_SUCCESS("Client Logger created");
        emit_signal("client_status_updated", true);
    }
}

void DojoC::client_metadata()
{
    dojo_bindings::ResultWorldMetadata resMetadata = dojo_bindings::client_metadata(client);

    if (resMetadata.tag == dojo_bindings::ErrWorldMetadata)
    {
        LOG_ERROR(resMetadata.err.message);
        // UtilityFunctions::push_error("Error: ", resMetadata.err.message);
    }
    else
    {
        LOG_SUCCESS("Metadata creado");
    }
    dojo_bindings::WorldMetadata metadata = resMetadata.ok;
    std::vector<dojo_bindings::CHashItemFieldElementModelMetadata> feltmetadata(
        metadata.models.data, metadata.models.data + metadata.models.data_len);

    for (const auto& feltMeta : feltmetadata)
    {
        dojo_bindings::ModelMetadata model_metadata = feltMeta.value;
        LOG_DEBUG(vformat("[color=Cyan]NameSpace: %s | Model: %s | TY: %s[/color]",
            model_metadata.namespace_, model_metadata.name,
            DojoPrimitive::PrimitiveTagToString(model_metadata.schema.tag)));
        if (model_metadata.schema.tag == dojo_bindings::Ty_Tag::Primitive_)
        {
            LOG_DEBUG("[color=Peru]Primitive[/color]");
            dojo_bindings::Primitive primitive = model_metadata.schema.primitive;
            DojoPrimitive _primitive = DojoPrimitive(primitive);
            // _msg += _primitive.get_value();
        }
        else if (model_metadata.schema.tag == dojo_bindings::Ty_Tag::Struct_)
        {
            dojo_bindings::Struct metaStruct = model_metadata.schema.struct_;
            dojo_bindings::CArrayMember children = metaStruct.children;
            LOG_DEBUG(vformat("[color=Peru]Struct is: %s [/color]", metaStruct.name));
            std::vector<dojo_bindings::Member> members(children.data, children.data + children.data_len);

            // Iterar y procesar los elementos
            for (const auto& member : members)
            {
                LOG_DEBUG(vformat("[color=YELLOW]member: %s[/color]", member.name));

                if (member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
                {
                    LOG_DEBUG("member_type is [color=YELLOW]Primitive[/color]");
                    dojo_bindings::Primitive primitive = member.ty->primitive;
                    DojoPrimitive _primitive = DojoPrimitive(primitive);
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::Struct_)
                {
                    LOG_DEBUG("member_type is [color=YELLOW]Struct[/color]");
                    dojo_bindings::Struct struct_ = member.ty->struct_;
                    LOG_DEBUG(vformat("[color=Peru]struct_name[/color] [color=YELLOW] %s [/color]",
                        struct_.name));
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::Array_)
                {
                    LOG_DEBUG("member_type is [color=YELLOW]CArrayTy[/color]");
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::ByteArray)
                {
                    LOG_DEBUG("member_type is [color=YELLOW]ByteArray[/color]");
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::Enum_)
                {
                    LOG_DEBUG("member_type is [color=YELLOW]Enum[/color]");
                    dojo_bindings::Enum enum_ = member.ty->enum_;
                    LOG_DEBUG(vformat("enum_name [color=YELLOW]%s[/color]", enum_.name));
                    LOG_DEBUG(vformat("enum_option [color=YELLOW]%s[/color]", enum_.option));
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::Tuple_)
                {
                    LOG_DEBUG("member_type is [color=YELLOW]Tuple[/color]");
                }
            }
        }
    }
}

// callback al iniciar sesion en el controller de Cartridge
void on_account(dojo_bindings::ControllerAccount* account)
{
    // Implementar
    LOG_SUCCESS("Account Data received");
    session_account = account;
    DojoC::get_singleton()->call_deferred("emit_signal", "controller_account_status_updated", true);
    DojoC::get_singleton()->call_deferred("emit_signal", "on_account");

    print_felt("chain ID ", dojo_bindings::controller_chain_id(session_account));
    FieldElement private_key = {"0x14d6672dcb4b77ca36a887e9a11cd9d637d5012468175829e9c6e770c61642"};
    priv = private_key.get_felt_no_ptr();
    dojo_bindings::FieldElement res_addr = dojo_bindings::controller_address(session_account);
    FieldElement account_addr = {&res_addr};
    LOG_CUSTOM("PLAYER ID", account_addr.to_string());
    LOG_INFO("--\n\n");
}

TypedArray<Dictionary> DojoC::get_entities()
{
    TypedArray<Dictionary> result = {};
    dojo_bindings::Query query = {};
    dojo_bindings::Pagination pagination = {};
    pagination.limit = 10;
    pagination.cursor.tag = dojo_bindings::COptionc_char_Tag::Nonec_char;
    dojo_bindings::CArrayOrderBy orderBy = {};
    pagination.order_by = orderBy;
    pagination.direction = dojo_bindings::PaginationDirection::Forward;
    query.pagination = pagination;
    query.clause.tag = dojo_bindings::NoneClause;
    query.no_hashed_keys = true;
    dojo_bindings::CArrayc_char models = {};
    query.models = models;
    query.historical = false;

    dojo_bindings::ResultPageEntity resPageEntities = dojo_bindings::client_entities(client, query);
    if (resPageEntities.tag == dojo_bindings::ErrPageEntity)
    {
        LOG_ERROR(resPageEntities.err.message);
        return result;
    }
    else
    {
        dojo_bindings::PageEntity pageEntities = resPageEntities.ok;
        LOG_DEBUG("ENTITIES PAGE");
        // dojo_bindings::COptionc_char o_char = pageEntities.next_cursor;
        // String cursor = o_char.some;
        // LOG_SUCCESS("Entities received", cursor);
        std::vector<dojo_bindings::Entity> entities(pageEntities.items.data,
                                                    pageEntities.items.data + pageEntities.items.data_len);
        for (const auto& entity : entities)
        {
            LOG_DEBUG("ENTITY");
            // dojo_bindings::FieldElement felt_keys = entity.hashed_keys;
            // FieldElement hashed_keys = {&felt_keys};
            Dictionary data = {};
            std::vector<dojo_bindings::Struct> c_structs(entity.models.data,
                                                         entity.models.data + entity.models.data_len);
            for (const auto& c_struct : c_structs)
            {
                LOG_INFO("Entity Struct: ", c_struct.name);
                std::vector<dojo_bindings::Member> mebmers(c_struct.children.data,
                                                           c_struct.children.data + c_struct.children.data_len);
                for (const auto& member : mebmers)
                {
                    String member_name = member.name;
                    LOG_SUCCESS_EXTRA("Entity", "MEMBER " , member_name);
                    LOG_SUCCESS_EXTRA("TAG", member_name + " " , String::num_int64(member.ty->tag) );
                    if (member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
                    {
                        LOG_INFO(member.name, " is [color=YELLOW]Primitive[/color]");
                        dojo_bindings::Primitive primitive = member.ty->primitive;
                        LOG_SUCCESS_EXTRA("PRIMITIVE TAG", String::num_int64(primitive.tag) );
                        LOG_INFO("primitive.tag is [color=YELLOW]Felt252[/color]");
                        if (String(member.name) == "player")
                        {
                            LOG_DEBUG("Player");
                            FieldElement felt = {&primitive.felt252};
                            data["id"] = felt.to_string();
                        }
                    }
                    else if (member.ty->tag == dojo_bindings::Ty_Tag::Struct_)
                    {
                        LOG_INFO(member.name, " is [color=YELLOW]Struct[/color]");
                        dojo_bindings::Struct struct_ = member.ty->struct_;
                        LOG_INFO("[color=Peru]struct_name[/color] [color=YELLOW]", struct_.name, "[/color]");
                        std::vector<dojo_bindings::Member> struct_child(struct_.children.data,
                                                                        struct_.children.data + struct_.children.data_len);
                        String member_name = member.name;
                        if (member_name == "vec")
                        {
                            Vector2 vec2 = {0, 0};
                            for (const auto& struct_child_member : struct_child)
                            {
                                LOG_INFO("struct_child_member.name: ", struct_child_member.name);

                                if (struct_child_member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
                                {
                                    DojoPrimitive s_value = {struct_child_member.ty->primitive};
                                    LOG_DEBUG(struct_child_member.name, " | ", s_value.get_value());
                                    real_t s_value_converted = s_value.get_value();
                                    if (String(struct_child_member.name) == "x")
                                    {
                                        LOG_WARNING("UPDATING X");
                                        vec2.x = s_value_converted;
                                    }
                                    else if (String(struct_child_member.name) == "y")
                                    {
                                        LOG_WARNING("UPDATING Y");
                                        vec2.y = s_value_converted;
                                    }
                                    // LOG_DEBUG(struct_child_member.name, s_value.get_value());
                                }
                            }
                            LOG_SUCCESS("[color=MAGENTA]NEW VECTOR2 [/color]", vec2);
                            data["pos"] = vec2;

                        }
                    }
                }
            }
            result.append(data);
        }
    }
    return result;
}

TypedArray<Dictionary> DojoC::get_controllers()
{
    TypedArray<Dictionary> result = {};
    dojo_bindings::COptionFieldElement control = {};
    control.tag = dojo_bindings::NoneFieldElement;
    // dojo_bindings::ResultCArrayController resControllers = dojo_bindings::client_controllers(client, nullptr, 0);
    dojo_bindings::ResultCArrayController resControllers = dojo_bindings::client_controllers(client, &control.some, 0);
    if (resControllers.tag == dojo_bindings::ErrCArrayController)
    {
        LOG_ERROR(resControllers.err.message);
    }
    else
    {
        dojo_bindings::CArrayController controllers = resControllers.ok;
        LOG_SUCCESS("Controllers received");
        LOG_CUSTOM("CURRENT PLAYER", controller_username(session_account));
        if (controllers.data_len <= 0)
        {
            LOG_DEBUG("YOU ARE ALONE IN THIS WORLD...");
            return {};
        }

        std::vector<dojo_bindings::Controller> controller_list(controllers.data,
                                                               controllers.data + controllers.data_len);
        for (const auto& controller : controller_list)
        {
            Dictionary controller_dict = {};
            dojo_bindings::FieldElement controller_addr = controller.address;
            FieldElement user_felt = {&controller_addr};
            LOG_DEBUG("CONTROLLER");

            if (strcmp(controller.username, controller_username(session_account)) == 0)
            {
                LOG_DEBUG("Player Found");
                controller_dict["player"] = true;
            }
            else
            {
                controller_dict["player"] = false;
            }
            controller_dict["username"] = controller.username;
            controller_dict["id"] = user_felt.to_string();
            controller_dict["timestamp"] = controller.deployed_at_timestamp;
            LOG_CUSTOM("CONTROLLER USERNAME", controller_dict["username"]);
            LOG_CUSTOM("CONTROLLER ID", controller_dict["id"]);
            LOG_CUSTOM("CONTROLLER TIMESTAMP", controller_dict["timestamp"]);
            result.append(controller_dict);
        }
    }
    return result;
}

void DojoC::controller_new(const String& controller_addr,
                           const String& rpc_url = "http://localhost:5050"
)
{
    FieldElement target = {controller_addr, 32};
    FieldElement katana = {"0x57505f474f444f545f44454d4f5f524f4f4b4945", 32};
    LOG_INFO("CHAIN ID: ", katana.bytearray_deserialize());

    dojo_bindings::ResultProvider resControllerProvider = dojo_bindings::provider_new(rpc_url.utf8().get_data());
    if (resControllerProvider.tag == dojo_bindings::ErrProvider)
    {
        UtilityFunctions::printerr("Error: ", resControllerProvider.err.message);
        emit_signal("provider_status_updated", false);

        return;
    }
    else
    {
        LOG_SUCCESS("Controller Provider created");
        controller_provider = resControllerProvider.ok;
        emit_signal("provider_status_updated", true);
    }

    // string_to_bytes(controller_addr, target.data, 32);
    // Por ahora hardcodeado
    dojo_bindings::Policy policies[] = {
        {*target.get_felt(), "reset_spawn", "Spawns at 0,0"},
        {*target.get_felt(), "spawn", "Spawns"},
        {*target.get_felt(), "move", "Move to a direction"},
    };
    uintptr_t policies_len = 3;

    dojo_bindings::ResultControllerAccount resControllerAccount =
        dojo_bindings::controller_account(policies, policies_len, *katana.get_felt());
    if (resControllerAccount.tag == dojo_bindings::OkControllerAccount)
    {
        LOG_INFO("Session account already connected");
        session_account = resControllerAccount.ok;
        emit_signal("on_account");
    }
    else
    {
        LOG_INFO("Session account not connected, connecting...");
        dojo_bindings::controller_connect(rpc_url.utf8().get_data(), policies, policies_len, on_account);
    }
}

void subscription_callback(dojo_bindings::Event event)
{
    UtilityFunctions::print_verbose("subscription_callback Triggered");
    UtilityFunctions::print_verbose(&event.data);
}

void on_indexer_update(dojo_bindings::ResultSubscription result)
{
    // Implementar
    UtilityFunctions::print_verbose("on_indexer_update callback Triggered");
    dojo_bindings::Subscription* subscription = result.ok;
}

String field_element_to_hex(const dojo_bindings::FieldElement& fe)
{
    String ret = "0x";
    for (size_t i = 0; i < 32; i++)
    {
        ret += String::num_int64(fe.data[i], 16, false);
    };

    return ret;
}

EventSubscription* event_subscription;
EventSubscription* entity_subs;

void event_wrapper(dojo_bindings::FieldElement entity_id, dojo_bindings::CArrayStruct models)
{
    event_subscription->on_entity_update(&entity_id, models);
}

void message_wrapper(dojo_bindings::FieldElement entity_id, dojo_bindings::CArrayStruct models)
{
    entity_subs->on_entity_update(&entity_id, models);
}

void DojoC::create_entity_subscription(Callable callback)
{
    event_subscription = new EventSubscription();

    dojo_bindings::COptionClause event_clause = {};

    event_clause.tag = dojo_bindings::NoneClause;
    event_subscription->set_callback(callback);
    dojo_bindings::ResultSubscription resEvent = dojo_bindings::client_on_event_message_update(
        client, event_clause, event_wrapper);
    if (resEvent.tag == dojo_bindings::ErrSubscription)
    {
        LOG_ERROR(resEvent.err.message);
        emit_signal("subscription_status_updated", false);

        return;
    }
    else
    {
        event_subscription->set_subscription(resEvent.ok);

        LOG_SUCCESS("Event subscription created.");
        emit_signal("subscription_status_updated", true);
    }
}

void DojoC::entity_subscription(Callable callback)
{
    entity_subs = new EventSubscription();
    entity_subs->set_callback(callback);
    dojo_bindings::COptionClause event_clause = {};
    event_clause.tag = dojo_bindings::NoneClause;

    dojo_bindings::ResultSubscription resEntity = dojo_bindings::client_on_entity_state_update(
        client,
        event_clause,
        message_wrapper
    );
    if (resEntity.tag == dojo_bindings::ErrSubscription)
    {
        LOG_ERROR(resEntity.err.message);
        emit_signal("entities_status_updated", false);
    }
    else
    {
        entity_subs->set_subscription(resEntity.ok);
        emit_signal("entities_status_updated", true);
    }
}

void account_execute_raw(const dojo_bindings::Call* calldata, const uintptr_t calldata_len = 0)
{
    if (controller_provider == nullptr)
    {
        UtilityFunctions::printerr("Error: ", "Controller provider not initialized.");
        return;
    }
    if (account == nullptr)
    {
        UtilityFunctions::printerr("Error: ", "Account not initialized.");
        return;
    }

    dojo_bindings::ResultFieldElement result = dojo_bindings::account_execute_raw(account, calldata, calldata_len);
    if (result.tag == dojo_bindings::ErrFieldElement)
    {
        LOG_ERROR(calldata->selector, " Failed: ", result.err.message);
    }
    else
    {
        dojo_bindings::wait_for_transaction(controller_provider, result.ok);
        LOG_SUCCESS(calldata->selector);
    }
}

void DojoC::spawn(bool reset = false, bool _debug = false)
{
    LOG_CUSTOM("SPAWN", "[color=RED]----------Block ID---------");

    dojo_bindings::BlockId block_id = {
        dojo_bindings::BlockTag_,
        dojo_bindings::Pending
    };
    dojo_bindings::Call spawn = {
        *actions,
        reset ? "reset_spawn" : "spawn",
    };
    if (_debug)
    {
        if (account == nullptr)
        {
            LOG_ERROR("Account not initialized.");
            return;
        }
        dojo_bindings::account_set_block_id(account, block_id);
        account_execute_raw(&spawn, 1);
    }
    else
    {
        if (session_account == nullptr)
        {
            LOG_ERROR("Session Account not initialized.");
            return;
        }

        LOG_CUSTOM("USERNAME", dojo_bindings::controller_username(session_account));

        // dojo_bindings::account_set_block_id(session_account, block_id);
        dojo_bindings::ResultFieldElement result = dojo_bindings::controller_execute_from_outside(
            session_account, &spawn, 1);
        if (result.tag == dojo_bindings::ErrFieldElement)
        {
            LOG_ERROR(spawn.selector, " Failed: ", result.err.message);
        }
        else
        {
            dojo_bindings::wait_for_transaction(controller_provider, result.ok);
            LOG_SUCCESS(spawn.selector);
        }
    }
}

void DojoC::move(const Ref<FieldElement> ref_felt, bool test = false, const bool _debug = false)
{
    LOG_CUSTOM("MOVE", "CallData creation");
    // dojo_bindings::FieldElement direction_felt = {};
    // String direction_hex = "0x0" + String::num_int64(0, 16);
    // LOG_INFO(direction_hex);
    // // string_to_bytes(direction_hex, direction_felt.data, 32);
    // direction_felt.data[31] = 0x01;
    uintptr_t calldata_data_len = 1;
    dojo_bindings::FieldElement* direction_felt = ref_felt->get_felt();
    dojo_bindings::CArrayFieldElement calldata = {

        direction_felt,
        1

    };
    dojo_bindings::Call move = {
        *actions
    };
    if (!test)
    {
        move.selector = "move";
        move.calldata = calldata;
        calldata_data_len = calldata.data_len;
        LOG_CUSTOM("MOVE", "calldata");
    }
    else
    {
        move.selector = "Position";
    }
    LOG_CUSTOM("MOVE", "CallData created");


    if (_debug)
    {
        if (account == nullptr)
        {
            LOG_ERROR("Account not initialized.");
            return;
        }
        account_execute_raw(&move, 1);
    }
    else
    {
        if (session_account == nullptr)
        {
            LOG_ERROR("Session Account not initialized.");
            return;
        }
        LOG_CUSTOM("USERNAME", dojo_bindings::controller_username(session_account));
        // dojo_bindings::account_set_block_id(session_account, block_id);
        dojo_bindings::ResultFieldElement result = dojo_bindings::controller_execute_from_outside(
            session_account, &move, calldata_data_len);
        if (result.tag == dojo_bindings::ErrFieldElement)
        {
            LOG_ERROR(move.selector, " Failed: ", result.err.message);
        }
        else
        {
            dojo_bindings::wait_for_transaction(controller_provider, result.ok);
            LOG_SUCCESS_EXTRA("MOVE", move.selector);
        }
    }
}

void DojoC::send_message(const String& _msg)
{
    dojo_bindings::FieldElement acc = dojo_bindings::controller_address(session_account);
    FieldElement account_address = {&acc};
    Dictionary typed_data = {};
    typed_data["identity"] = account_address.as_packed_array();
    typed_data["message"] = _msg;
    typed_data["channel"] = FieldElement(0).as_packed_array();
    typed_data["salt"] = FieldElement(35).as_packed_array();

    String json_data = JSON::stringify(typed_data);

    dojo_bindings::ResultFieldElement hash = dojo_bindings::cairo_short_string_to_felt("lawea");
    if (hash.tag == dojo_bindings::ErrFieldElement)
    {
        LOG_ERROR("Failed to convert message to felt: ", hash.err.message);
        return;
    }
    LOG_INFO("Message hash: ", field_element_to_hex(hash.ok));
    dojo_bindings::ResultSignature signature = dojo_bindings::signing_key_sign(
        dojo_bindings::signing_key_new(),
        hash.ok);
    if (signature.tag == dojo_bindings::ErrSignature)
    {
        LOG_ERROR("Failed to sign message: ", signature.err.message);
        return;
    }
    LOG_INFO("Signature: ", field_element_to_hex(signature.ok.r));
    LOG_INFO("Signature: ", field_element_to_hex(signature.ok.s));
    dojo_bindings::FieldElement signature_felts[] = {
        {signature.ok.r},
        {signature.ok.s}
    };
    LOG_INFO("Signature felts");
    dojo_bindings::ResultFieldElement result = dojo_bindings::client_publish_message(
        client,
        json_data.utf8().get_data(),
        signature_felts,
        2
    );

    if (result.tag == dojo_bindings::ErrFieldElement)
    {
        LOG_ERROR("Failed to publish message: ", result.err.message);
        return;
    }
    else
    {
        LOG_SUCCESS("Message published.");
        FieldElement felt_result = {&result.ok};
        LOG_INFO(felt_result.to_string());
    }
    LOG_INFO("FINISHED");
}

String DojoC::get_username()
{
    return dojo_bindings::controller_username(session_account);
}

PackedByteArray DojoC::get_session_packed_byte()
{
    LOG_INFO("SESSION PACKED BYTE");
    return FieldElement::to_packed_array(session_account);
}

String DojoC::get_session_address()
{
    dojo_bindings::FieldElement _felt = dojo_bindings::controller_address(session_account);
    FieldElement account_address = {&_felt};
    return account_address.to_string();
}
