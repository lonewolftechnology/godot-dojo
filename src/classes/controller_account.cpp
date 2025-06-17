//
// Created by hazel on 16/06/25.
//

#include "classes/controller_account.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "debug_macros.h"

#include "classes/torii_client.h"

ControllerAccount* ControllerAccount::singleton = nullptr;

void ControllerAccount::_bind_methods() {
    // Métodos de conexión
    ClassDB::bind_method(D_METHOD("create", "rpc_url", "private_key", "account_address"),
                        &ControllerAccount::create);
    ClassDB::bind_method(D_METHOD("disconnect_controller"), &ControllerAccount::disconnect_controller);
    ClassDB::bind_method(D_METHOD("is_controller_connected"), &ControllerAccount::is_controller_connected);

    // Información de la cuenta
    ClassDB::bind_method(D_METHOD("get_username"), &ControllerAccount::get_username);
    ClassDB::bind_method(D_METHOD("get_address"), &ControllerAccount::get_address);
    ClassDB::bind_method(D_METHOD("get_chain_id"), &ControllerAccount::get_chain_id);

    // Ejecución de transacciones
    ClassDB::bind_method(D_METHOD("execute_raw", "selector", "calldata"),
                        &ControllerAccount::execute_raw, DEFVAL(Array()));
    ClassDB::bind_method(D_METHOD("execute_from_outside", "selector", "calldata"), 
                        &ControllerAccount::execute_from_outside, DEFVAL(Array()));

    // Métodos de utilidad
    ClassDB::bind_method(D_METHOD("get_account_info"), &ControllerAccount::get_account_info);
    // ClassDB::bind_method(D_METHOD("sign_message", "message"), &ControllerAccount::sign_message);

    // Señales
    ADD_SIGNAL(MethodInfo("controller_connected", PropertyInfo(Variant::BOOL, "success")));
    ADD_SIGNAL(MethodInfo("controller_disconnected"));
    ADD_SIGNAL(MethodInfo("transaction_executed", PropertyInfo(Variant::STRING, "transaction_hash")));
    ADD_SIGNAL(MethodInfo("transaction_failed", PropertyInfo(Variant::STRING, "error_message")));
}

ControllerAccount::ControllerAccount() {
    if (Engine::get_singleton()->is_editor_hint()) {
        UtilityFunctions::push_warning("ControllerAccount está ejecutándose en modo editor");
        return;
    }
    
    singleton = this;
    session_account = nullptr;
    is_connected = false;

    LOG_INFO("ControllerAccount inicializado");
}

ControllerAccount::~ControllerAccount() {
    disconnect_controller();
    singleton = nullptr;
    LOG_INFO("ControllerAccount destruido");
}

ControllerAccount* ControllerAccount::get_singleton() {
    return singleton;
}

void ControllerAccount::set_session_account(DOJO::ControllerAccount* account) {
    session_account = account;
    is_connected = (account != nullptr);
    emit_connection_status(is_connected);
}

DOJO::ControllerAccount* ControllerAccount::get_session_account() const {
    return session_account;
}

void on_account(DOJO::ControllerAccount* account)
{
    LOG_SUCCESS("Account Data received");
    ControllerAccount::get_singleton()->set_session_account(account);
    DOJO::FieldElement player_address = DOJO::controller_address(account);
    DOJO::FieldElement player_chain_id = DOJO::controller_chain_id(account);
    LOG_CUSTOM("PLAYER CHAIN IN", FieldElement::get_as_string(&player_chain_id));
    LOG_CUSTOM("PLAYER ID", FieldElement::get_as_string(&player_address));
    LOG_INFO("--\n\n");
}

void ControllerAccount::create(const String& controller_addr,
                           const String& rpc_url = "http://localhost:5050"
)
{
    FieldElement target = {controller_addr, 32};

    DOJO::Result<DOJO::Provider*> resControllerProvider = DOJO::provider_new(rpc_url.utf8().get_data());
    if (resControllerProvider.tag == DOJO::Result<DOJO::Provider*>::Tag::Err)
    {
        UtilityFunctions::printerr("Error: ", resControllerProvider.err._0.message);
        emit_signal("provider_status_updated", false);

        return;
    }
    else
    {
        LOG_SUCCESS("Controller Provider created");
        provider = resControllerProvider.ok._0;
        emit_signal("provider_status_updated", true);
    }

    // string_to_bytes(controller_addr, target.data, 32);
    // Por ahora hardcodeado
    DOJO::Policy policies[] = {
        {*target.get_felt(), "reset_spawn", "Spawns at 0,0"},
        {*target.get_felt(), "spawn", "Spawns"},
        {*target.get_felt(), "move", "Move to a direction"},
    };
    uintptr_t policies_len = 3;
    DOJO::FieldElement katana = ToriiClient::get_singleton()->get_world().get_felt_no_ptr();
    DOJO::Result<DOJO::ControllerAccount*> resControllerAccount =
        DOJO::controller_account(policies, policies_len, katana);
    if (resControllerAccount.tag == DOJO::Result<DOJO::ControllerAccount*>::Tag::Ok)
    {
        LOG_INFO("Session account already connected");
        session_account = resControllerAccount.ok._0;
        emit_signal("on_account");
    }
    else
    {
        LOG_INFO("Session account not connected, connecting...");
        DOJO::controller_connect(rpc_url.utf8().get_data(), policies, policies_len, on_account);
    }
}


void ControllerAccount::disconnect_controller() {
    if (session_account != nullptr) {
        // TODO: Controller_free
        session_account = nullptr;
        is_connected = false;
        emit_signal("controller_disconnected");
        LOG_INFO("ControllerAccount desconectado");
    }
}

bool ControllerAccount::is_controller_connected() const {
    return is_connected && session_account != nullptr;
}

String ControllerAccount::get_username() const {
    if (!is_controller_connected()) {
        return "";
    }
    return String(DOJO::controller_username(session_account));
}

String ControllerAccount::get_address() const {
    if (!is_controller_connected()) {
        return "0x0";
    }
    DOJO::FieldElement felt = DOJO::controller_address(session_account);
    return FieldElement::get_as_string(&felt);
}

String ControllerAccount::get_chain_id() const {
    if (!is_controller_connected()) {
        return "";
    }
    DOJO::FieldElement felt = DOJO::controller_chain_id(session_account);
    return FieldElement::get_as_string(&felt);
}

DOJO::CArray<DOJO::FieldElement> array_to_felt_array(const Array& data) {
    // Implementar conversión de Array de Godot a CArrayFieldElement
    DOJO::CArray<DOJO::FieldElement> felt_array = {};

    if (data.size() == 0) {
        felt_array.data = nullptr;
        felt_array.data_len = 0;
        return felt_array;
    }

    // Aquí necesitarías alocar memoria y convertir cada elemento
    // Esta es una implementación simplificada
    felt_array.data_len = data.size();

    return felt_array;
}
DOJO::Call create_call_from_data(const String& selector, const Array& calldata) {
    DOJO::Call call = {};

    call.selector = selector.utf8().get_data();

    if (calldata.size() > 0) {
        call.calldata = array_to_felt_array(calldata);
    }

    return call;
}

void ControllerAccount::execute_raw(const String& selector, const Array& calldata) {
    if (!is_controller_connected()) {
        LOG_ERROR("ControllerAccount no está conectado");
        emit_signal("transaction_failed", "Cuenta no conectada");
        return ;
    }

    DOJO::Call call = create_call_from_data(selector, calldata);

    DOJO::Result<DOJO::FieldElement> result = DOJO::controller_execute_raw(
        session_account, &call, 1
    );

    if (result.tag == DOJO::Result<DOJO::FieldElement>::Tag::Err) {
        LOG_ERROR("Error al ejecutar transacción: ", result.err._0.message);
        emit_signal("transaction_failed", String(result.err._0.message));
        return;
    } else
    {
        DOJO::wait_for_transaction(provider, result.ok._0);
        LOG_SUCCESS_EXTRA("EXECUTED", call.selector);
    }
}


void ControllerAccount::execute_from_outside(const String& selector, const Array& calldata) {
    if (!is_controller_connected()) {
        LOG_ERROR("ControllerAccount no está conectado");
        emit_signal("transaction_failed", "Cuenta no conectada");
    }

    DOJO::Call call = create_call_from_data(selector, calldata);

    DOJO::Result<DOJO::FieldElement> result = DOJO::controller_execute_from_outside(
        session_account, &call, call.calldata.data_len
    );
    
    if (result.tag == DOJO::Result<DOJO::FieldElement>::Tag::Err) {
        LOG_ERROR(result.err._0.message);
        emit_signal("transaction_failed", String(result.err._0.message));
        return;
    } else
    {
        DOJO::wait_for_transaction(provider, result.ok._0);
        LOG_SUCCESS_EXTRA("EXECUTED", call.selector);
    }
}

Dictionary ControllerAccount::get_account_info() const {
    Dictionary info = {};
    
    if (!is_controller_connected()) {
        info["connected"] = false;
        return info;
    }
    
    info["connected"] = true;
    info["username"] = get_username();
    info["address"] = get_address();
    info["chain_id"] = get_chain_id();
    
    return info;
}


void ControllerAccount::emit_connection_status(bool connected) {
    if (connected) {
        emit_signal("controller_connected");
    } else {
        emit_signal("controller_disconnected");
    }
}




