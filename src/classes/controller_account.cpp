//
// Created by hazel on 16/06/25.
//

#include "classes/controller_account.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/os.hpp"
#include "tools/logger.h"

#include "classes/torii_client.h"
#include <vector>
#include "tools/dojo_helper.h"

ControllerAccount* ControllerAccount::singleton = nullptr;


ControllerAccount::ControllerAccount()
{
    singleton = this;
    session_account = nullptr;
    is_connected = false;
    provider = nullptr;
    rpc_url = ProjectSettings::get_singleton()->get("dojo/config/katana/rpc_url");
}

ControllerAccount::~ControllerAccount()
{
    // disconnect_controller();
    session_account = nullptr;
    if (singleton != nullptr && singleton == this)
    {
        singleton = nullptr;
    }
    Logger::debug_extra("ControllerAccount", "DESTRUCTOR CALLED");
}

ControllerAccount* ControllerAccount::get_singleton()
{
    return singleton;
}

void ControllerAccount::set_session_account(DOJO::ControllerAccount* account)
{
    session_account = account;
    is_connected = (account != nullptr);
    emit_connection_status(is_connected);
}

DOJO::ControllerAccount* ControllerAccount::get_session_account() const
{
    return session_account;
}

void on_account(DOJO::ControllerAccount* account)
{
    Logger::success("Account Data received");
    ControllerAccount::get_singleton()->set_session_account(account);

    DOJO::FieldElement player_address = DOJO::controller_address(account);
    DOJO::FieldElement player_chain_id = DOJO::controller_chain_id(account);

    Logger::custom("PLAYER CHAIN ID", FieldElement::get_as_string(&player_chain_id));
    Logger::custom("PLAYER ID", FieldElement::get_as_string(&player_address));
}

void ControllerAccount::setup()
{
    init_provider();
    create(policies);
}

void ControllerAccount::init_provider()
{
    // Provider
    Logger::debug_extra("Provider", rpc_url);
    DOJO::ResultProvider resControllerProvider = DOJO::provider_new(rpc_url.utf8().get_data());
    if (resControllerProvider.tag == DOJO::ErrProvider)
    {
        UtilityFunctions::printerr("Error: ", GET_DOJO_ERROR(resControllerProvider));
        emit_signal("provider_status_updated", false);
        return;
    }
    Logger::success("Controller Provider created");
    provider = GET_DOJO_OK(resControllerProvider);
    emit_signal("provider_status_updated", true);
}

void ControllerAccount::create(const Ref<DojoPolicies>& policies_data)
{
    Logger::info("RPC URL: ", rpc_url);


    if (policies_data->is_empty())
    {
        Logger::error("Invalid policies data");
        return;
    }

    std::vector<DOJO::Policy> policies = policies_data->build();
    uintptr_t policies_len = policies.size();

    DOJO::FieldElement katana = FieldElement::cairo_short_string_to_felt(chain_id.utf8().get_data());
    Logger::custom_color("azure", "katana", FieldElement::get_as_string(&katana));

    DOJO::ResultControllerAccount resControllerAccount =
        DOJO::controller_account(policies.data(), policies_len, katana);

    if (resControllerAccount.tag == DOJO::OkControllerAccount)
    {
        Logger::info("Session account already connected");
        session_account = GET_DOJO_OK(resControllerAccount);
        on_account(session_account);
    }
    else
    {
        Logger::info("Session account not connected, connecting...");
        DOJO::controller_connect(rpc_url.utf8().get_data(), policies.data(), policies_len, on_account);
    }
}

void ControllerAccount::disconnect_controller()
{
    if (session_account != nullptr)
    {
        std::vector<DOJO::Policy> policy = policies->build();
        uintptr_t policies_len = policy.size();
        DOJO::FieldElement chain_id = DOJO::controller_chain_id(session_account);

        DOJO::Resultbool resClear = DOJO::controller_clear(policy.data(), policies_len, chain_id);
        if (resClear.tag == DOJO::Errbool)
        {
            Logger::error("Failed to clear Controller", resClear.err.message);
            return;
        }
        Logger::success("Controller cleared");
        session_account = nullptr;
        is_connected = false;
        emit_signal("controller_disconnected");
        Logger::info("ControllerAccount desconectado");
    }
}

bool ControllerAccount::is_controller_connected() const
{
    return is_connected && session_account != nullptr;
}

String ControllerAccount::get_username() const
{
    if (!is_controller_connected())
    {
        return "";
    }
    return {DOJO::controller_username(session_account)};
}

String ControllerAccount::get_address() const
{
    if (!is_controller_connected())
    {
        return "0x0";
    }
    DOJO::FieldElement felt = DOJO::controller_address(session_account);
    return FieldElement::get_as_string(&felt);
}

String ControllerAccount::get_chain_id() const
{
    if (!is_controller_connected())
    {
        return chain_id;
    }
    DOJO::FieldElement felt = DOJO::controller_chain_id(session_account);
    String controller_chain_id = FieldElement::get_as_string(&felt);

    DOJO::FieldElement chain_id_felt = FieldElement::cairo_short_string_to_felt(chain_id);
    String katana_chain_id = FieldElement::get_as_string(&chain_id_felt);

    if (controller_chain_id != katana_chain_id)
    {
        Logger::warning("Chain ID mismatch ", controller_chain_id, " | ", katana_chain_id);
    }
    return chain_id;
}


void ControllerAccount::execute_from_outside(const Ref<DojoCall>& action)
{
    if (!is_controller_connected())
    {
        Logger::error("ControllerAccount no está conectado");
        emit_signal("transaction_failed", "Cuenta no conectada");
    }
    // Refactor based on UE implementation
    DOJO::FieldElement actions = FieldElement::from_string(action->get_to());
    std::string selector = action->get_selector_ctr();

    uintptr_t calldata_len = action->get_size();
    Array args = action->get_calldata();

    DOJO::FieldElement* felts = nullptr;
    Logger::debug_extra("Controller", "Building Call");

    DOJO::Call call = {
        actions,
        selector.c_str(),
    };

    if (calldata_len > 0)
    {
        Logger::debug_extra("Controller", "Building Calldata");

        Array final_args = {};

        for (int i = 0; i < calldata_len; i++)
        {
            const Variant& arg = args[i];
            Logger::custom("Calldata", Variant::get_type_name(arg.get_type()), arg.stringify());

            switch (arg.get_type())
            {
            case Variant::Type::ARRAY:
                {
                    Array v_array = static_cast<Array>(arg);
                    // final_args.push_back(v_array.size());
                    final_args.append_array(arg);
                    break;
                }
            case Variant::Type::INT:
                {
                    final_args.push_back(arg);
                    break;
                }
            case Variant::Type::VECTOR2:
                {
                    Vector2 vec = static_cast<Vector2>(arg);

                    final_args.append(vec.x);
                    final_args.append(vec.y);

                    break;
                }
            case Variant::Type::VECTOR2I:
                {
                    Vector2i vec = static_cast<Vector2i>(arg);

                    final_args.append(vec.x);
                    final_args.append(vec.y);

                    break;
                }
            case Variant::Type::VECTOR3:
                {
                    Vector3 vec = static_cast<Vector3>(arg);

                    final_args.append(vec.x);
                    final_args.append(vec.y);
                    final_args.append(vec.z);

                    break;
                }
            case Variant::Type::VECTOR3I:
                {
                    Vector3i vec = static_cast<Vector3i>(arg);

                    final_args.append(vec.x);
                    final_args.append(vec.y);
                    final_args.append(vec.z);

                    break;
                }
            default:
                {
                    final_args.push_back(arg.stringify());
                    break;
                }
            }
        }
        Logger::custom("CALLDATA", calldata_len);
        calldata_len = final_args.size();
        Logger::debug_extra("CALLDATA", calldata_len, Variant(final_args).stringify());
        felts = static_cast<DOJO::FieldElement*>(malloc(sizeof(*felts) * calldata_len));
        memset(felts, 0, sizeof(*felts) * calldata_len);
        for (int i = 0; i < calldata_len; i++)
        {
            const Variant& arg = final_args[i];
            if (arg.get_type() == Variant::Type::INT)
            {
                felts[i] = FieldElement::from_enum(arg);
            }
            else
            {
                felts[i] = FieldElement::from_string(arg);
            }
        }

        call.calldata = {
            felts,
            calldata_len
        };
        Logger::debug_extra("CALLDATA", "Calldata added, size:", call.calldata.data_len);
    }

    Logger::debug_extra("CALL", Variant(call.selector));
    DOJO::ResultFieldElement result = DOJO::controller_execute_from_outside(
        session_account, &call, 1
    );
    if (result.tag == DOJO::ErrFieldElement)
    {
        Logger::error("Transaction failed");
        Logger::error("To:", action->get_to());
        Logger::error("Selector:", action->get_selector());
        Logger::error("Error:", GET_DOJO_ERROR(result));
        for (int i = 0; i < calldata_len; i++)
        {
            Logger::debug_extra("Calldata[" + String::num_int64(i) + "]",
                                FieldElement::get_as_string(&felts[i]));
        }
        emit_signal("transaction_failed", GET_DOJO_ERROR(result));
    }
    else
    {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(result));
        Logger::success_extra("EXECUTED", call.selector);
    }
    if (felts)
    {
        free(felts, nullptr);
    }
}

Dictionary ControllerAccount::get_account_info() const
{
    Dictionary info = {};

    if (!is_controller_connected())
    {
        info["connected"] = false;
        return info;
    }

    info["connected"] = true;
    info["username"] = get_username();
    info["address"] = get_address();
    info["chain_id"] = get_chain_id();

    return info;
}


void ControllerAccount::emit_connection_status(bool connected)
{
    if (connected)
    {
        call_deferred("emit_signal", "controller_connected", connected);
        call_deferred("emit_signal", "current_user_info", get_account_info());
    }
    else
    {
        call_deferred("emit_signal", "controller_disconnected");
    }
}
