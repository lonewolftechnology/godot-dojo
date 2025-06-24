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

#include "resources/dojo_policy.h"

//TODO: refactor a instancia global en vez de singleton
ControllerAccount* ControllerAccount::singleton = nullptr;


ControllerAccount::ControllerAccount()
{
    singleton = this;
    if (Engine::get_singleton()->is_editor_hint())
    {
        LOG_DEBUG_EXTRA("ControllerAccount", "Desactivando en modo editor");
        return;
    }
    session_account = nullptr;
    is_connected = false;
}

ControllerAccount::~ControllerAccount()
{
    // disconnect_controller();
    // singleton = nullptr;
    // LOG_INFO("ControllerAccount destruido");
    LOG_DEBUG_EXTRA("ControllerAccount", "DESTRUCTOR CALLED");
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
    LOG_SUCCESS("Account Data received");
    ControllerAccount::get_singleton()->set_session_account(account);

    DOJO::FieldElement player_address = DOJO::controller_address(account);
    DOJO::FieldElement player_chain_id = DOJO::controller_chain_id(account);

    LOG_CUSTOM("PLAYER CHAIN ID", FieldElement::get_as_string(&player_chain_id));
    LOG_CUSTOM("PLAYER ID", FieldElement::get_as_string(&player_address));
}

void ControllerAccount::setup()
{
    create(policies);
}

// TODO: refactor a estatico
void ControllerAccount::create(const Ref<DojoPolicies>& policies_data)
{
    String rpc_url = ProjectSettings::get_singleton()->get("dojo/rpc_url");

    // Provider
    DOJO::ResultProvider resControllerProvider = DOJO::provider_new(rpc_url.utf8().get_data());
    if (resControllerProvider.tag == DOJO::ErrProvider)
    {
        UtilityFunctions::printerr("Error: ", GET_DOJO_ERROR(resControllerProvider));
        emit_signal("provider_status_updated", false);
        return;
    }
    LOG_SUCCESS("Controller Provider created");
    provider = GET_DOJO_OK(resControllerProvider);
    emit_signal("provider_status_updated", true);

    if (policies_data->is_empty())
    {
        LOG_ERROR("Invalid policies data");
        return;
    }

    std::vector<DOJO::Policy> policies = policies_data->build();
    uintptr_t policies_len = policies.size();
    String torii_chain_id = ToriiClient::get_singleton()->get_chain_id();
    DOJO::ResultFieldElement resKatana = DOJO::cairo_short_string_to_felt(torii_chain_id.utf8().get_data());
    if (resKatana.tag == DOJO::ErrFieldElement)
    {
        LOG_ERROR("Error al convertir Chain ID: ", GET_DOJO_ERROR(resKatana));
    }
    DOJO::FieldElement katana = resKatana.ok;

    DOJO::ResultControllerAccount resControllerAccount =
        DOJO::controller_account(policies.data(), policies_len, katana);

    if (resControllerAccount.tag == DOJO::OkControllerAccount)
    {
        LOG_INFO("Session account already connected");
        session_account = GET_DOJO_OK(resControllerAccount);
        on_account(session_account);
    }
    else
    {
        LOG_INFO("Session account not connected, connecting...");
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
            LOG_ERROR("Failed to clear Controller", resClear.err.message);
            return;
        }
        else
        {
            LOG_SUCCESS("Controller cleared");
        }

        session_account = nullptr;
        is_connected = false;
        emit_signal("controller_disconnected");
        LOG_INFO("ControllerAccount desconectado");
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
    return String(DOJO::controller_username(session_account));
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
        return "";
    }
    DOJO::FieldElement felt = DOJO::controller_chain_id(session_account);
    return FieldElement::get_as_string(&felt);
}

DOJO::CArrayFieldElement array_to_felt_array(const Array& data)
{
    DOJO::CArrayFieldElement felt_array = {};

    if (data.size() == 0)
    {
        felt_array.data = nullptr;
        felt_array.data_len = 0;
        return felt_array;
    }

    felt_array.data_len = data.size();

    return felt_array;
}

DOJO::Call create_call_from_data(const String& contract_address, const String& selector, const Array& calldata)
{
    DOJO::Call call = {};
    call.to = FieldElement::from_string(contract_address);
    call.selector = selector.utf8().get_data();

    if (!calldata.is_empty())
    {
        LOG_DEBUG_EXTRA("CALLDATA", Variant(calldata.size()));
        call.calldata = array_to_felt_array(calldata);
    }

    return call;
}

void ControllerAccount::execute_raw(const Ref<DojoCall>& action)
{
    if (!is_controller_connected())
    {
        LOG_ERROR("ControllerAccount no está conectado");
        emit_signal("transaction_failed", "Cuenta no conectada");
        return;
    }

    DOJO::Call call = action->build();
    uintptr_t calldata_len = action->get_size();

    DOJO::ResultFieldElement result = DOJO::controller_execute_raw(
        session_account, &call, calldata_len
    );

    if (result.tag == DOJO::ErrFieldElement)
    {
        LOG_ERROR("Error al ejecutar transacción: ", GET_DOJO_ERROR(result));
        emit_signal("transaction_failed", GET_DOJO_ERROR(result));
    }
    else
    {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(result));
        LOG_SUCCESS_EXTRA("EXECUTED", call.selector);
    }
}


void ControllerAccount::execute_from_outside(const Ref<DojoCall>& action)
{
    if (!is_controller_connected())
    {
        LOG_ERROR("ControllerAccount no está conectado");
        emit_signal("transaction_failed", "Cuenta no conectada");
    }
    DOJO::Call call = action->build();
    uintptr_t calldata_len = action->get_size();

    DOJO::ResultFieldElement result = DOJO::controller_execute_from_outside(
        session_account, &call, calldata_len
    );
    if (result.tag == DOJO::ErrFieldElement)
    {
        LOG_ERROR(GET_DOJO_ERROR(result));
        emit_signal("transaction_failed", GET_DOJO_ERROR(result));
    }
    else
    {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(result));
        LOG_SUCCESS_EXTRA("EXECUTED", call.selector);
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
