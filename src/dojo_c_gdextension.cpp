//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include <iomanip>
#include <unistd.h>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <debug_macros.h>
#include <variant/primitive.h>

#include "variant/field_element.h"

using namespace godot;
DojoC* DojoC::singleton = nullptr;
dojo_bindings::ControllerAccount* session_account;
dojo_bindings::Provider* controller_provider;
dojo_bindings::Account* account;
dojo_bindings::FieldElement actions;

void DojoC::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoC::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &DojoC::get_enabled);
    ClassDB::bind_method(D_METHOD("testing"), &DojoC::testing);
    ClassDB::bind_method(D_METHOD("spawn"), &DojoC::spawn);
    ClassDB::bind_method(D_METHOD("move"), &DojoC::move);

    // ClassDB::bind_method(D_METHOD("client_new"), &DojoC::client_new);
    ClassDB::bind_method(D_METHOD("create_client", "p_world"), &DojoC::create_client);
    ClassDB::bind_method(D_METHOD("controller_connect"), &DojoC::controller_connect);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");

    ClassDB::bind_method(D_METHOD("get_message"), &DojoC::get_output_message);
    ClassDB::bind_method(D_METHOD("set_message", "p_message"), &DojoC::set_output_message);


    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "output_message", PROPERTY_HINT_ARRAY_TYPE), "set_message",
                 "get_message");

    ADD_SIGNAL(MethodInfo("event_update", PropertyInfo(Variant::STRING, "msg")));
    ADD_SIGNAL(MethodInfo("controller_account_status_updated", PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(MethodInfo("account_status_updated", PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(MethodInfo("provider_status_updated", PropertyInfo(Variant::BOOL, "status")));
    ADD_SIGNAL(
        MethodInfo(
            "subscription_status_updated",
            PropertyInfo(Variant::BOOL, "status"),
            PropertyInfo(Variant::STRING, "subscription_name") )
    );
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
    actions = {};
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

// Esto viene de la integración con Unreal
void string_to_bytes(const String& hex_str, uint8_t* out_bytes, size_t max_bytes = 32)
{
    // const std::string& hex_str{hex_str_godot.utf8()};
    // Skip "0x" prefix if present
    size_t start_idx = (hex_str.substr(0, 2) == "0x") ? 2 : 0;

    // Calculate actual string length without prefix
    size_t hex_length = hex_str.length() - start_idx;

    // Handle odd number of characters by assuming leading zero
    bool is_odd = hex_length % 2 != 0;
    size_t num_bytes = (hex_length + is_odd) / 2;

    // Ensure we don't overflow the output buffer
    if (num_bytes > max_bytes)
    {
        return;
        //        throw std::runtime_error("Input hex string too long for output buffer");
    }

    size_t out_idx = 0;

    // Handle first nibble separately if we have odd number of characters
    if (is_odd)
    {
        String nibble = hex_str.substr(start_idx, 1);
        out_bytes[out_idx++] = static_cast<uint8_t>(nibble.hex_to_int());
    }

    // Process two hex digits at a time
    for (size_t i = is_odd ? 1 : 0; i < hex_length; i += 2)
    {
        String byte_str = hex_str.substr(start_idx + i, 2);
        out_bytes[out_idx++] = static_cast<uint8_t>(byte_str.hex_to_int());
    }
}

// que devuelva un msg es testing, no deberia devolver nada
String DojoC::create_client(const String& world_addr)
{
    String _msg = "";
    const char* torii_url = "http://localhost:8080";
    const char* relay_url = "/ip4/127.0.0.1/tcp/9090";

    dojo_bindings::ToriiClient* client;

    UtilityFunctions::print("Attempting to create client...");

    dojo_bindings::FieldElement world;
    // string_to_bytes(world_addr, world.data, 32);
    // Log las direcciones que estamos usando
    UtilityFunctions::prints("RPC Endpoint: ", torii_url);
    UtilityFunctions::prints("World Endpoint: ", relay_url);

    dojo_bindings::ResultToriiClient resClient = dojo_bindings::client_new(
        torii_url,
        relay_url,
        world);

    if (resClient.tag == dojo_bindings::ErrToriiClient)
    {
        UtilityFunctions::printerr("Error: ", resClient.err.message);
        // UtilityFunctions::push_error("Error: ", resClient.err.message);
        _msg = String("[Error] create_client: ") + resClient.err.message;
    }
    else
    {
        UtilityFunctions::print_rich("[color=Green]Client creado[/color]");
        client = resClient.ok;
        _msg = "Client creado";
    }

    dojo_bindings::ResultWorldMetadata resMetadata = dojo_bindings::client_metadata(client);

    if (resMetadata.tag == dojo_bindings::ErrWorldMetadata)
    {
        UtilityFunctions::printerr("Error: ", resMetadata.err.message);
        // UtilityFunctions::push_error("Error: ", resMetadata.err.message);
        _msg = String("[Error] create_client: ") + resMetadata.err.message;
    }
    else
    {
        UtilityFunctions::print_rich("[color=Green]Metadata creado[/color]");
        _msg = "Metadata creado";
    }
    dojo_bindings::WorldMetadata metadata = resMetadata.ok;

    std::vector<dojo_bindings::CHashItemFieldElementModelMetadata> feltmetadata(
        metadata.models.data, metadata.models.data + metadata.models.data_len);

    for (const auto& feltMeta : feltmetadata)
    {
        dojo_bindings::ModelMetadata model_metadata = feltMeta.value;
        UtilityFunctions::print_rich(vformat("[color=Cyan]NameSpace: %s | Model: %s | TY: %s[/color]",
                                             model_metadata.namespace_, model_metadata.name,
                                             DojoPrimitive::PrimitiveTagToString(model_metadata.schema.tag)));
        if (model_metadata.schema.tag == dojo_bindings::Ty_Tag::Primitive_)
        {
            UtilityFunctions::print_rich("[color=Peru]Primitive[/color]");
            dojo_bindings::Primitive primitive = model_metadata.schema.primitive;
            DojoPrimitive _primitive = DojoPrimitive(primitive);
            // _msg += _primitive.get_value();
        }
        else if (model_metadata.schema.tag == dojo_bindings::Ty_Tag::Struct_)
        {
            dojo_bindings::Struct metaStruct = model_metadata.schema.struct_;
            dojo_bindings::CArrayMember children = metaStruct.children;
            UtilityFunctions::print_rich(vformat("[color=Peru]Struct is: %s [/color]", metaStruct.name));
            std::vector<dojo_bindings::Member> members(children.data, children.data + children.data_len);

            // Iterar y procesar los elementos
            for (const auto& member : members)
            {
                UtilityFunctions::print_rich(vformat("[color=YELLOW]member: %s[/color]", member.name));

                if (member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
                {
                    UtilityFunctions::print_rich("member_type is [color=YELLOW]Primitive[/color]");
                    dojo_bindings::Primitive primitive = member.ty->primitive;
                    DojoPrimitive _primitive = DojoPrimitive(primitive);
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::Struct_)
                {
                    UtilityFunctions::print_rich("member_type is [color=YELLOW]Struct[/color]");
                    dojo_bindings::Struct struct_ = member.ty->struct_;
                    UtilityFunctions::print_rich(vformat("[color=Peru]struct_name[/color] [color=YELLOW] %s [/color]",
                                                         struct_.name));
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::Array_)
                {
                    UtilityFunctions::print_rich("member_type is [color=YELLOW]CArrayTy[/color]");
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::ByteArray)
                {
                    UtilityFunctions::print_rich("member_type is [color=YELLOW]ByteArray[/color]");
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::Enum_)
                {
                    UtilityFunctions::print_rich("member_type is [color=YELLOW]Enum[/color]");
                    dojo_bindings::Enum enum_ = member.ty->enum_;
                    UtilityFunctions::print_rich(vformat("enum_name [color=YELLOW]%s[/color]", enum_.name));
                    UtilityFunctions::print_rich(vformat("enum_option [color=YELLOW]%s[/color]", enum_.option));
                }
                else if (member.ty->tag == dojo_bindings::Ty_Tag::Tuple_)
                {
                    UtilityFunctions::print_rich("member_type is [color=YELLOW]Tuple[/color]");
                }
            }
        }
    }
    return _msg;
}


// callback al iniciar sesion en el controller de Cartridge
void on_account(dojo_bindings::ControllerAccount* account)
{
    // Implementar
    UtilityFunctions::print_rich("[color=Green]on_account callback Triggered[/color]");
    session_account = account;
    DojoC::get_singleton()->call_deferred("emit_signal", "controller_account_status_updated", true);
}


void DojoC::controller_connect(const String& controller_addr)
{
    const char* rpc_url = "http://localhost:5050";

    dojo_bindings::FieldElement target;
    // string_to_bytes(controller_addr, target.data, 32);
    dojo_bindings::Policy policies[] = {
        {target, "spawn", "spawn"},
        {target, "move", "move"},
    };

    uintptr_t policies_len = 2;

    dojo_bindings::controller_connect(rpc_url, policies, policies_len, on_account);
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

void on_event_update_msg(const String& _msg)
{
    DojoC::get_singleton()->call_deferred("emit_signal", "event_update", _msg);
    UtilityFunctions::print_rich(_msg);
}

void on_event_update(dojo_bindings::FieldElement entity_id, dojo_bindings::CArrayStruct models)
{
    // Implementar
    Array result;
    dojo_bindings::FieldElement nullFelt;
    string_to_bytes("0x0", nullFelt.data, 0);
    UtilityFunctions::print_verbose("on_event_update callback Triggered.");
    UtilityFunctions::prints("Entity ID", field_element_to_hex(entity_id));

    if (field_element_to_hex(entity_id) == "0x00000000000000000000000000000000")
    // if (&entity_id == &nullFelt)
    {
        UtilityFunctions::print("Entity ID is 0, WAITING");
        // sleep(2);
        return;
    }

    if (models.data == nullptr)
    {
        UtilityFunctions::print("models.data is null");
        return;
    }

    if (models.data->children.data == nullptr)
    {
        UtilityFunctions::print("models.data->children.data is null");
        return;
    }

    on_event_update_msg("[color=RED]START[/color]");
    // Accede a models.data->children.data->ty de forma segura
    on_event_update_msg(vformat("model name %s", models.data->name));
    auto children = models.data->children;
    //std::cout << "children.data_len: " << children.data_len << std::endl;
    // std::cout << "children.data: " << children.data << std::endl;

    // Convertir `CArrayMember` a `std::vector`
    std::vector<dojo_bindings::Member> members(children.data, children.data + children.data_len);

    // Iterar y procesar los elementos
    for (const auto& member : members)
    {
        // Accede al miembro que necesites dependiendo de la estructura de `Member`.
        std::cout << "----------------" << std::endl;
        on_event_update_msg("[color=PERU]Procesando member... [/color]");
        on_event_update_msg(vformat("member type: %s", typeid(member).name()));
        on_event_update_msg(vformat("member.name: %s", member.name));

        if (member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
        {
            on_event_update_msg("member_type is [color=YELLOW]Primitive[/color]");
            dojo_bindings::Primitive primitive = member.ty->primitive;
            DojoPrimitive _primitive = DojoPrimitive(primitive);
            result.append(_primitive.get_value());
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Struct_)
        {
            on_event_update_msg("member_type is [color=YELLOW]Struct[/color]");
            dojo_bindings::Struct struct_ = member.ty->struct_;
            on_event_update_msg(vformat("[color=Peru]struct_name[/color] [color=YELLOW] %s [/color]", struct_.name));
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Array_)
        {
            on_event_update_msg("member_type is [color=YELLOW]CArrayTy[/color]");
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::ByteArray)
        {
            on_event_update_msg("member_type is [color=YELLOW]ByteArray[/color]");
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Enum_)
        {
            on_event_update_msg("member_type is [color=YELLOW]Enum[/color]");
            dojo_bindings::Enum enum_ = member.ty->enum_;
            on_event_update_msg(vformat("enum_name [color=YELLOW]%s[/color]", enum_.name));
            on_event_update_msg(vformat("enum_option [color=YELLOW]%s[/color]", enum_.option));
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Tuple_)
        {
            on_event_update_msg("member_type is [color=YELLOW]Tuple[/color]");
        }

        DojoC::get_singleton()->set_output_message(result);
    }
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

void DojoC::testing()
{
    const char* rpc_url = "http://localhost:5050";
    const char* torii_url = "http://localhost:8080";
    const char* relay_url = "/ip4/127.0.0.1/tcp/9090";

    dojo_bindings::FieldElement world;
    string_to_bytes("0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5", world.data, 32);
    print_felt("world", world);
    // dojo_bindings::FieldElement actions;
    string_to_bytes("0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf", actions.data, 32);
    print_felt("actions", actions);

    dojo_bindings::FieldElement player;
    string_to_bytes("0x13d9ee239f33fea4f8785b9e3870ade909e20a9599ae7cd62c1c292b73af1b7", player.data, 32);
    print_felt("player", player);

    dojo_bindings::FieldElement katana;
    string_to_bytes("0x4b4154414e41", katana.data, 32);
    print_felt("katana", katana);

    dojo_bindings::FieldElement private_key;
    string_to_bytes("0x1c9053c053edf324aec366a34c6901b1095b07af69495bffec7d7fe21effb1b", private_key.data, 32);
    print_felt("private_key", private_key);

    // string_to_bytes("0x4b4154414e41", katana.data, 32);

    dojo_bindings::ToriiClient* client;
    dojo_bindings::ResultToriiClient resClient = dojo_bindings::client_new(torii_url, relay_url, world);
    if (resClient.tag == dojo_bindings::ErrToriiClient)
    {
        UtilityFunctions::printerr("Error: ", resClient.err.message);
        // UtilityFunctions::push_error("Error: ", resClient.err.message);
        return;
    }
    else
    {
        UtilityFunctions::print_rich("[color=Green]Client created.[/color]");
        client = resClient.ok;
    }

    dojo_bindings::client_set_logger(client, [](const char* msg)
    {
        UtilityFunctions::print_rich("[color=Green]ToriiClient log: ", msg);
    });

    dojo_bindings::ResultProvider resControllerProvider = dojo_bindings::provider_new(rpc_url);
    if (resControllerProvider.tag == dojo_bindings::ErrProvider)
    {
        UtilityFunctions::printerr("Error: ", resControllerProvider.err.message);
        emit_signal("provider_status_updated", false);

        return;
    }
    else
    {
        UtilityFunctions::print_rich("[color=green]Controller Provider created.[/color]");
        controller_provider = resControllerProvider.ok;
        emit_signal("provider_status_updated", true);
    }

    UtilityFunctions::print_rich("[color=RED]----------ACCOUNT---------");

    // // dojo_bindings::ResultFieldElement resExecute = dojo_bindings::controller_execute_raw();
    // std::vector<dojo_bindings::Controller> controllerList(data, data + data_len);

    dojo_bindings::ResultAccount resultAccount = dojo_bindings::account_new(
        controller_provider, private_key, "0x13d9ee239f33fea4f8785b9e3870ade909e20a9599ae7cd62c1c292b73af1b7");
    if (resultAccount.tag == dojo_bindings::ErrAccount)
    {
        UtilityFunctions::printerr("Error: ", resultAccount.err.message);
        // UtilityFunctions::push_error("Error: ", resultAccount.err.message);
        emit_signal("account_status_updated", false);

        return;
    }
    else
    {
        UtilityFunctions::print_rich("[color=Green]Account created.");
        account = resultAccount.ok;
        emit_signal("account_status_updated", true);
    }

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


    UtilityFunctions::print_verbose("Fetching entities...");
    dojo_bindings::ResultPageEntity resPageEntities = dojo_bindings::client_entities(client, query);
    if (resPageEntities.tag == dojo_bindings::ErrPageEntity)
    {
        UtilityFunctions::printerr("Error: ", resPageEntities.err.message);
        // UtilityFunctions::push_error("Error:", resPageEntities.err.message);
        return;
    }
    else
    {
        UtilityFunctions::print_verbose("Entities fetched.");
    }
    dojo_bindings::CArrayEntity fetchedEntities = resPageEntities.ok.items;


    printf("Fetched %zu entities\n", fetchedEntities.data_len);

    // retrieve session
    dojo_bindings::Policy policies[] = {
        {actions, "move", "move"},
        {actions, "spawn", "spawn"},
    };
    uintptr_t policies_len = 2;

    dojo_bindings::ResultControllerAccount resControllerAccount =
        dojo_bindings::controller_account(policies, policies_len, katana);
    if (resControllerAccount.tag == dojo_bindings::OkControllerAccount)
    {
        UtilityFunctions::print_rich("[color=Green]Session account already connected");
        session_account = resControllerAccount.ok;
    }
    else
    {
        UtilityFunctions::print_rich("[color=Red]Session account not connected, connecting...");
        dojo_bindings::controller_connect(rpc_url, policies, policies_len, on_account);
        // while (session_account == nullptr)
        // {
        //     usleep(100000); // Sleep for 100 ms to avoid busy waiting
        // }
    }


    dojo_bindings::COptionClause event_clause = {};
    event_clause.tag = dojo_bindings::NoneClause;

    dojo_bindings::ResultSubscription resEvent = dojo_bindings::client_on_entity_state_update(
        client, event_clause, on_event_update);
    if (resEvent.tag == dojo_bindings::ErrSubscription)
    {
        UtilityFunctions::printerr("Error: ", resEvent.err.message);
        emit_signal("subscription_status_updated", false, "entity_subscription");

        // UtilityFunctions::push_error("Error: ", resEvent.err.message);
        return;
    }
    else
    {
        UtilityFunctions::print_verbose("Event subscription created.");
        emit_signal("subscription_status_updated", true, "entity_subscription");
    }
    dojo_bindings::Subscription* event_subscription = resEvent.ok;


    // UtilityFunctions::print_rich("[color=RED]--EXECUTING SPAWN--");
    // system(
    //     "sozo execute dojo_starter-actions spawn --manifest-path /home/hazel/Proyectos/Godot/GDE/starting/dojo-starter/Scarb.toml");
    // while (get_singleton()->get_output_message().size() == 0)
    // {
    //     usleep(100000);
    // }


    // for (const auto& control : controllerList)
    // {
    //     // PackedByteArray addr = DojoPrimitive(control.address).get_value();
    //     // UtilityFunctions::print_rich("[color=Crimson]Controller: ", addr.hex_encode());
    //     // UtilityFunctions::print_rich("[color=Crimson]Controller: ", control.username);
    //     UtilityFunctions::print_rich("[color=Crimson]----er: ");
    //     // std::cout << "Controller: " << control.username << std::endl;
    // }
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

void DojoC::spawn()
{
    UtilityFunctions::print_rich("[color=RED]----------Block ID---------");

    dojo_bindings::BlockId block_id = {
        .tag = dojo_bindings::BlockTag_,
        .block_tag = dojo_bindings::Pending
    };
    dojo_bindings::account_set_block_id(account, block_id);
    UtilityFunctions::print_rich("[color=RED]----------Spawn---------");

    dojo_bindings::Call spawn = {
        .to = actions,
        .selector = "spawn",
    };
    account_execute_raw(&spawn, 1);
}

void DojoC::move()
{
    LOG_INFO("----------CallData---------");
    dojo_bindings::FieldElement direction_felt = {};
    String direction_hex = "0x0" + String::num_int64(0, 16);
    LOG_INFO(direction_hex);
    // string_to_bytes(direction_hex, direction_felt.data, 32);
    direction_felt.data[31] = 0x01;
    LOG_INFO("----------Move---------");
    // dojo_bindings::ResultFieldElement result = dojo_bindings::cairo_short_string_to_felt("0");
    // if (result.tag == dojo_bindings::ErrFieldElement)
    // {
    //     LOG_ERROR(result.err.message);
    //     return;
    // }
    // else
    // {
    //     direction_felt = result.ok;
    // }
    LOG_INFO(field_element_to_hex(direction_felt));
    dojo_bindings::Call move = {
        .to = actions,
        .selector = "move",
        .calldata = {
            .data = &direction_felt,
            .data_len = 1
        }
    };


    account_execute_raw(&move, 1);
}
