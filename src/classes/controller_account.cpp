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
#include <classes/account.h>

#include "tools/dojo_helper.h"

ControllerAccount* ControllerAccount::singleton = nullptr;


ControllerAccount::ControllerAccount()
{
    if (Engine::get_singleton()->is_editor_hint() == false)
    {
        return;
    }
    singleton = this;
    session_account = nullptr;
    is_connected = false;
    provider = nullptr;
    chain_id = "";
    rpc_url = "";
    Logger::debug_extra("ControllerAccount", "CONSTRUCTOR CALLED");
}

ControllerAccount::~ControllerAccount()
{
    if (Engine::get_singleton()->is_editor_hint() == false)
    {
        return;
    }
    disconnect_controller();
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

void ControllerAccount::on_account_callback(DOJO::ControllerAccount* account)
{
    Logger::success("Account Data received");
    ControllerAccount* instance = get_singleton();
    if (!instance)
    {
        Logger::warning(
            "ControllerAccount singleton is null in on_account_callback. The application may be shutting down.");
        return;
    }

    instance->set_session_account(account);

    DOJO::FieldElement player_address = DOJO::controller_address(account);
    DOJO::FieldElement player_chain_id = DOJO::controller_chain_id(account);

    Logger::custom("PLAYER CHAIN ID", FieldElement::get_as_string(&player_chain_id));
    Logger::custom("PLAYER ID", FieldElement::get_as_string(&player_address));
}

void ControllerAccount::setup()
{
    check_rpc_url();
    init_provider();
    create(policies);
}

void ControllerAccount::init_provider()
{
    // Provider
    if (rpc_url.is_empty())
    {
        Logger::error("RPC URL not set");
        return;
    }

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
#ifdef ANDROID_ENABLED
    Logger::error(
        "ControllerAccount: Session management is not supported on Android due to external library limitations.");
    emit_signal("controller_disconnected");
    // Early return to prevent calling incompatible FFI functions.
    return;
#else
    Logger::info("RPC URL: ", rpc_url);
    if (provider == nullptr)
    {
        Logger::error("Provider not initialized");
        return;
    }
    if (policies_data->is_empty())
    {
        Logger::error("Invalid policies data");
        return;
    }

    this->policies = policies_data;

    std::vector<DOJO::Policy> policies = policies_data->build();
    uintptr_t policies_len = policies.size();
    String _chain = get_chain_id();
    Logger::debug_extra("ControllerAccount", "Chain ID: ", _chain);
    FieldElement katana = {_chain};
    Logger::custom_color("azure", "katana", katana.to_string());

    DOJO::ResultControllerAccount resControllerAccount =
        DOJO::controller_account(policies.data(), policies_len, katana.get_felt_no_ptr());

    if (resControllerAccount.tag == DOJO::OkControllerAccount)
    {
        Logger::info("Session account already connected");
        session_account = GET_DOJO_OK(resControllerAccount);
        on_account_callback(session_account);
    }
    else
    {
        Logger::info("Session account not connected, connecting...");
        DOJO::controller_connect(
            rpc_url.utf8().get_data(),
            policies.data(),
            policies_len,
            on_account_callback);
    }
#endif
}

void ControllerAccount::disconnect_controller()
{
    if (session_account != nullptr)
    {
        std::vector<DOJO::Policy> policy = policies->build();
        uintptr_t policies_len = policy.size();
        DOJO::FieldElement chain_id = DOJO::controller_chain_id(session_account);

#ifndef ANDROID_ENABLED
        DOJO::Resultbool resClear = DOJO::controller_clear(policy.data(), policies_len, chain_id);
        if (resClear.tag == DOJO::Errbool)
        {
            Logger::error("Failed to clear Controller", resClear.err.message);
        }
        else
        {
            Logger::success("Controller cleared");
        }
#else
        Logger::warning(
            "ControllerAccount: Remote session clearing is not supported on Android. Disconnecting locally.");
#endif
        // WARNING: This will leak memory on all platforms. The 'dojo.c' library allocates the session_account
        // but does not provide a function to free it.
        session_account = nullptr;
        is_connected = false;
        emit_signal("controller_disconnected");
        Logger::info("ControllerAccount disconnected");
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
    if (chain_id.is_empty() || !is_controller_connected())
    {
        Logger::debug_extra("ControllerAccount", "Getting chain id from provider by creating an Account");
        Account burner_account = {};
        String _rpc_url = ProjectSettings::get_singleton()->get("dojo/config/katana_url");
        String _address = ProjectSettings::get_singleton()->get("dojo/config/account/address");
        String _private_key = ProjectSettings::get_singleton()->get("dojo/config/account/private_key");
        // if (provider != nullptr)
        // {
        //     burner_account.set_provider(provider);
        // }
        burner_account.create(_rpc_url, _address, _private_key);
        // if (provider != nullptr)
        // {
        //     burner_account.set_provider(nullptr); // To avoid freeing the provider from ControllerAccoount
        // }

        chain_id = burner_account.get_chain_id();
        burner_account.queue_free();
    }
    return chain_id;
}

void ControllerAccount::check_rpc_url()
{
    Logger::debug_extra("ControllerAccount", "Checking RPC URL", rpc_url, rpc_url.is_empty());
    if (rpc_url.is_empty())
    {
        Logger::debug_extra("ControllerAccount", "RPC URL not set, checking project settings");
        String setting = ProjectSettings::get_singleton()->get("dojo/config/katana_url");
        if (setting.is_empty())
        {
            Logger::error("RPC URL not set");
            return;
        }
        rpc_url = setting;
        Logger::debug_extra("ControllerAccount", "RPC URL set to: ", rpc_url);
    }
}

String ControllerAccount::get_rpc_url()
{
    return rpc_url;
}

// TODO: Ver Threads
void ControllerAccount::execute_from_outside(const String& to, const String& selector,
                                             const Variant& calldata = Array())
{
    if (!is_controller_connected())
    {
        Logger::error("ControllerAccount not found");
        emit_signal("transaction_failed", "Controller not found");
        return;
    }
    if (DojoHelpers::is_valid_calldata(calldata) == false)
    {
        return;
    }
    const Array& args = calldata;

    DojoCallData call_data = DojoHelpers::prepare_dojo_call_data(to, selector, args);

    Logger::debug_extra("ControllerAccount", "Populating Call");

    std::string selector_str = selector.utf8().get_data();

    DOJO::Call call = {
        call_data.to,
        selector_str.c_str(),
        {call_data.calldata_felts.data(), call_data.calldata_felts.size()}
    };

    DOJO::ResultFieldElement result = DOJO::controller_execute_from_outside(
        session_account, &call, 1
    );
    if (result.tag == DOJO::ErrFieldElement)
    {
        Logger::error("Transaction failed");
        Logger::error("To:", to);
        Logger::error("Selector:", selector);
        Logger::error("Args:", args);
        Logger::error("Error:", GET_DOJO_ERROR(result));
        emit_signal("transaction_failed", GET_DOJO_ERROR(result));
    }
    else
    {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(result));
        Logger::success_extra("EXECUTED", selector);
    }
}

void ControllerAccount::execute_raw(const String& to, const String& selector, const Variant& calldata = Array())
{
    if (!is_controller_connected())
    {
        Logger::error("ControllerAccount not found");
        emit_signal("transaction_failed", "Controller not found");
        return;
    }
    if (DojoHelpers::is_valid_calldata(calldata) == false)
    {
        return;
    }

    const Array& args = calldata;
    DojoCallData call_data = DojoHelpers::prepare_dojo_call_data(to, selector, args);

    Logger::debug_extra("ControllerAccount", "Populating Call");

    std::string selector_str = selector.utf8().get_data();

    DOJO::Call call = {
        call_data.to,
        selector_str.c_str(),
        {call_data.calldata_felts.data(), call_data.calldata_felts.size()}
    };

    DOJO::ResultFieldElement result = DOJO::controller_execute_raw(
        session_account, &call, 1
    );
    if (result.tag == DOJO::ErrFieldElement)
    {
        Logger::error("Transaction failed");
        Logger::error("To:", to);
        Logger::error("Selector:", selector);
        Logger::error("Args:", args);
        Logger::error("Error:", GET_DOJO_ERROR(result));
        emit_signal("transaction_failed", GET_DOJO_ERROR(result));
    }
    else
    {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(result));
        Logger::success_extra("EXECUTED", selector);
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
