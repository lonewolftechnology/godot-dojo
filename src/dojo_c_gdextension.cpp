//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include <iomanip>
#include <unistd.h>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "types/primitive.h"

using namespace godot;
DojoC* DojoC::singleton = nullptr;
dojo_bindings::ControllerAccount* session_account = nullptr;
void DojoC::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoC::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &DojoC::get_enabled);
    ClassDB::bind_method(D_METHOD("testing"), &DojoC::testing);
    // ClassDB::bind_method(D_METHOD("client_new"), &DojoC::client_new);
    ClassDB::bind_method(D_METHOD("create_client", "p_world"), &DojoC::create_client);
    ClassDB::bind_method(D_METHOD("controller_connect"), &DojoC::controller_connect);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");

    ClassDB::bind_method(D_METHOD("get_message"), &DojoC::get_output_message);
    ClassDB::bind_method(D_METHOD("set_message", "p_message"), &DojoC::set_output_message);
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "output_message", PROPERTY_HINT_ARRAY_TYPE), "set_message", "get_message");

}

DojoC::DojoC()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        UtilityFunctions::print_verbose("DojoC is running in editor mode");
        return;
    }
    // Initialize any variables here.
    singleton = this;
    enabled = true;

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

void hex_to_bytes(const String _addr, dojo_bindings::FieldElement* felt)
{
    const char* hex = _addr.utf8().get_data();

    if (hex[0] == '0' && hex[1] == 'x')
    {
        hex += 2;
    }

    // handle hex less than 64 characters - pad with 0
    size_t len = strlen(hex);
    if (len < 64)
    {
        char* padded = static_cast<char*>(malloc(65));
        memset(padded, '0', 64 - len);
        padded[64 - len] = '\0';
        strcat(padded, hex);
        hex = padded;
    }

    UtilityFunctions::prints("hex: ", hex);

    for (size_t i = 0; i < 32; i++)
    {
        sscanf(hex + 2 * i, "%2hhx", &(*felt).data[i]);
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
    hex_to_bytes(world_addr, &world);
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
        return "ERROR";
    }
    else
    {
        UtilityFunctions::print_verbose("Client creado");
        client = resClient.ok;
        _msg = "Client creado";
    }


    return _msg;
}


// callback al iniciar sesion en el controller de Cartridge
void on_account(dojo_bindings::ControllerAccount* account)
{
    // Implementar
    UtilityFunctions::print_verbose("on_account callback Triggered");
    session_account = account;
}

void DojoC::controller_connect(const String& controller_addr)
{
    const char* rpc_url = "http://localhost:5050";

    dojo_bindings::FieldElement target;
    hex_to_bytes(controller_addr, &target);

    dojo_bindings::Policy policies[] = {
        {target, "spawn", "spawn"},
        {target, "move", "move"},
    };

    uintptr_t policies_len = 2;

    dojo_bindings::controller_connect(rpc_url, policies, policies_len, on_account);
}

// Esto viene de la integración con Unreal, pero parece que funciona igual/mejor que hex_to_bytes
void string_to_bytes(const std::string& hex_str, uint8_t* out_bytes, size_t max_bytes)
{
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
        std::string nibble = hex_str.substr(start_idx, 1);
        out_bytes[out_idx++] = static_cast<uint8_t>(std::stoul(nibble, nullptr, 16));
    }

    // Process two hex digits at a time
    for (size_t i = is_odd ? 1 : 0; i < hex_length; i += 2)
    {
        std::string byte_str = hex_str.substr(start_idx + i, 2);
        out_bytes[out_idx++] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
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


void on_event_update(dojo_bindings::FieldElement entity_id, dojo_bindings::CArrayStruct models)
{
    // Implementar
    Array result;
    dojo_bindings::FieldElement nullFelt;
    hex_to_bytes("0x00000000000000000000000000000000", &nullFelt);
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

    UtilityFunctions::print_rich("[color=RED]START");
    // Accede a models.data->children.data->ty de forma segura
    UtilityFunctions::prints("model name", models.data->name);
    auto children = models.data->children;
    std::cout << "children.data_len: " << children.data_len << std::endl;
    std::cout << "children.data: " << children.data << std::endl;

    // Convertir `CArrayMember` a `std::vector`
    std::vector<dojo_bindings::Member> members(children.data, children.data + children.data_len);

    // Iterar y procesar los elementos
    for (const auto& member : members)
    {
        // Accede al miembro que necesites dependiendo de la estructura de `Member`.
        std::cout << "----------------" << std::endl;
        UtilityFunctions::print_rich("[color=PERU]Procesando member...");
        UtilityFunctions::print_rich("member type: ", typeid(member).name());
        UtilityFunctions::print_rich("member.name: ", member.name);

        if (member.ty->tag == dojo_bindings::Ty_Tag::Primitive_)
        {
            UtilityFunctions::print_rich("member_type is [color=YELLOW]Primitive");
            dojo_bindings::Primitive primitive = member.ty->primitive;
            DojoPrimitive _primitive = DojoPrimitive(primitive);
            result.append(_primitive.get_value());

        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Struct_)
        {
            UtilityFunctions::print_rich("member_type is [color=YELLOW]Struct");
            dojo_bindings::Struct struct_ = member.ty->struct_;
            UtilityFunctions::print_rich("[color=Peru]struct_name [color=YELLOW]", struct_.name);
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Array_)
        {
            UtilityFunctions::print_rich("member_type is [color=YELLOW]CArrayTy");
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::ByteArray)
        {
            UtilityFunctions::print_rich("member_type is [color=YELLOW]ByteArray");
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Enum_)
        {
            UtilityFunctions::print_rich("member_type is [color=YELLOW]Enum");
            dojo_bindings::Enum enum_ = member.ty->enum_;
            UtilityFunctions::prints("enum_name", enum_.name);
            UtilityFunctions::prints("enum_option", enum_.option);
        }
        else if (member.ty->tag == dojo_bindings::Ty_Tag::Tuple_)
        {
            UtilityFunctions::print("member_type is [color=YELLOW]Tuple");
        }

        DojoC::get_singleton()->set_output_message(result);
        //     printf("\n");
        //     printf("on_entity_state_update\n");
        //     printf("Key: 0x");
        //     for (size_t i = 0; i < 32; i++)
        //     {
        //         printf("%02x", entity_id.data[i]);
        //     }
        //     printf("\n");
        //
        //     for (size_t i = 0; i < models.data_len; i++)
        //     {
        //         printf("Model: %s\n", models.data[i].name);
        //         for (size_t j = 0; j < models.data[i].children.data_len; j++)
        //         {
        //             printf("Field: %s\n", models.data[i].children.data[j].name);
        //             printf("Value: %p\n", models.data[i].children.data[j].ty);
        //         }
        //     }
    }

    // UtilityFunctions::prints( "children value:", children);
    // auto ty = models.data->children.data->ty;
    // UtilityFunctions::prints( "data value:", data);
    // UtilityFunctions::prints( "ty value:", ty);
    // UtilityFunctions::prints("type of ty:", typeid(ty->tag).name());
    // UtilityFunctions::print("END");
}


void DojoC::testing()
{
    const char* rpc_url = "http://0.0.0.0:5050";
    const char* torii_url = "http://0.0.0.0:8080";
    const char* relay_url = "/ip4/127.0.0.1/tcp/9090";

    dojo_bindings::FieldElement world;
    hex_to_bytes("0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5", &world);
    // string_to_bytes("0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5", world.data, 32);

    dojo_bindings::FieldElement actions;
    hex_to_bytes("0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf", &actions);
    // string_to_bytes("0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf", actions.data, 32);

    dojo_bindings::FieldElement player;
    hex_to_bytes("0x127fd5f1fe78a71f8bcd1fec63e3fe2f0486b6ecd5c86a0466c3a21fa5cfcec", &player);

    dojo_bindings::FieldElement katana;
    hex_to_bytes("0x4b4154414e41", &katana);

    // string_to_bytes("0x4b4154414e41", katana.data, 32);
    // hex_to_bytes("0x07dc7899aa655b0aae51eadff6d801a58e97dd99cf4666ee59e704249e51adf2", &katana);

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
        UtilityFunctions::print_verbose("Client created.");
        client = resClient.ok;
    }

    dojo_bindings::client_set_logger(client, [](const char* msg)
    {
    });

    dojo_bindings::Policy policies[] = {
        {actions, "move", "Create a board"},
        {actions, "spawn", "Create a game"},
    };

    dojo_bindings::ResultProvider resControllerProvider = dojo_bindings::provider_new(rpc_url);
    if (resControllerProvider.tag == dojo_bindings::ErrProvider)
    {
        UtilityFunctions::printerr("Error: ", resControllerProvider.err.message);
        // UtilityFunctions::push_error("Error:", resControllerProvider.err.message);
        return;
    }
    else
    {
        UtilityFunctions::print_verbose("Provider created.");
    }
    dojo_bindings::Provider* controller_provider = resControllerProvider.ok;


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
    // dojo_bindings::ResultControllerAccount resControllerAccount = dojo_bindings::controller_account(policies,2, katana);
    // if (resControllerAccount.tag == dojo_bindings::OkControllerAccount)
    // {
    // 	UtilityFunctions::print_verbose("Session account already connected");
    // 	session_account = resControllerAccount.ok;
    // }else
    // {
    // 	UtilityFunctions::print_verbose("Session account not connected, connecting...");
    // dojo_bindings::controller_connect(rpc_url, policies, 2, on_account);
    // }

    // while (session_account == nullptr)
    // {
    //     usleep(100000); // Sleep for 100 ms to avoid busy waiting
    // }
    dojo_bindings::FieldElement spawn_key;
    hex_to_bytes("0x02a29373f1af8348bd366a990eb3a342ef2cbe5e85160539eaca3441a673f468", &spawn_key);

    dojo_bindings::PatternMatching pattern_matching = dojo_bindings::PatternMatching::FixedLen;

    dojo_bindings::COptionFieldElement coption;
    coption.some = spawn_key;
    const char* model = "spawn";
    dojo_bindings::CArrayCOptionFieldElement keys = {&coption, 5};
    dojo_bindings::CArrayc_char Cmodel = {&model, 5};

    dojo_bindings::KeysClause* subClause = new dojo_bindings::KeysClause{
        keys,
        pattern_matching,
        Cmodel
    };

    dojo_bindings::ResultSubscription result_subscription = dojo_bindings::client_on_starknet_event(
        client, subClause, 1, &subscription_callback);

    if (result_subscription.tag == dojo_bindings::ErrSubscription)
    {
        UtilityFunctions::printerr("Error: ", result_subscription.err.message);
        // UtilityFunctions::push_error("Error:", result_subscription.err.message);
        return;
    }
    else
    {
        UtilityFunctions::print_verbose("Subscription created.");
    }
    dojo_bindings::Subscription* subscription = result_subscription.ok;

    dojo_bindings::COptionClause event_clause = {};
    event_clause.tag = dojo_bindings::NoneClause;

    dojo_bindings::ResultSubscription resEvent = dojo_bindings::client_on_entity_state_update(
        client, event_clause, on_event_update);
    if (resEvent.tag == dojo_bindings::ErrSubscription)
    {
        UtilityFunctions::printerr("Error: ", resEvent.err.message);
        // UtilityFunctions::push_error("Error: ", resEvent.err.message);
        return;
    }
    else
    {
        UtilityFunctions::print_verbose("Event subscription created.");
    }
    dojo_bindings::Subscription* event_subscription = resEvent.ok;

    UtilityFunctions::print_verbose("Session account connected");
}
