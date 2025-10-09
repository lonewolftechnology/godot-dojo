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

#ifdef ANDROID_ENABLED
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/java_class_wrapper.hpp"
#endif

ControllerAccount *ControllerAccount::singleton = nullptr;


ControllerAccount::ControllerAccount() {
    if (Engine::get_singleton()->is_editor_hint()) {
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

ControllerAccount::~ControllerAccount() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    if (DojoHelpers::get_setting("dojo/config/disconnect_controller_on_exit")) {
        disconnect_controller();
    } else {
        session_account = nullptr;
    }
    if (singleton != nullptr && singleton == this) {
        singleton = nullptr;
    }
    Logger::debug_extra("ControllerAccount", "DESTRUCTOR CALLED");
}

ControllerAccount *ControllerAccount::get_singleton() {
    return singleton;
}

void ControllerAccount::set_session_account(DOJO::ControllerAccount *account) {
    session_account = account;
    is_connected = (account != nullptr);
    emit_connection_status(is_connected);
}

DOJO::ControllerAccount *ControllerAccount::get_session_account() const {
    return session_account;
}

void ControllerAccount::on_account_callback(DOJO::ControllerAccount *account) {
    Logger::success("Account Data received");
    ControllerAccount *instance = get_singleton();
    if (!instance) {
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

void ControllerAccount::setup() {
    init_provider();
    create(policies);
}

void ControllerAccount::init_provider() {
    check_rpc_url();

    Logger::debug_extra("Provider", rpc_url);
    DOJO::ResultProvider resControllerProvider = DOJO::provider_new(rpc_url.utf8().get_data());
    if (resControllerProvider.tag == DOJO::ErrProvider) {
        Logger::error(GET_DOJO_ERROR(resControllerProvider));
        call_deferred("emit_signal", "provider_status_updated", false);
        return;
    }
    Logger::success("Controller Provider created");
    provider = GET_DOJO_OK(resControllerProvider);
    call_deferred("emit_signal", "provider_status_updated", true);
}

void ControllerAccount::create(const Dictionary &policies_data) {
#ifdef ANDROID_ENABLED
    Logger::error("Not implemented yet");
    return;
#else
    check_rpc_url();

    std::vector<DOJO::Policy> policies_vector = build_policies(policies_data);

    uintptr_t policies_len = policies_vector.size();
    String _chain = get_chain_id(false);

    Logger::debug_extra("ControllerAccount", "Chain ID: ", _chain);
    Ref<FieldElement> katana = memnew(FieldElement(_chain));
    Logger::custom_color("azure", "katana", katana->to_string());

    DOJO::ResultControllerAccount resControllerAccount =
            DOJO::controller_account(policies_vector.data(), policies_len, katana->get_felt_no_ptr());

    if (resControllerAccount.tag == DOJO::OkControllerAccount) {
        Logger::info("Session account already connected");
        session_account = GET_DOJO_OK(resControllerAccount);
        on_account_callback(session_account);
    } else {
        Logger::info("Session account not connected, connecting...");
        DOJO::controller_connect(
            rpc_url.utf8().get_data(),
            policies_vector.data(),
            policies_len,
            on_account_callback);
    }
#endif
}

void ControllerAccount::disconnect_controller() {
    if (session_account != nullptr) {
        std::vector<DOJO::Policy> policies_vector = build_policies();
        uintptr_t policies_len = policies_vector.size();
        DOJO::FieldElement controller_chain_id = DOJO::controller_chain_id(session_account);

        DOJO::Resultbool resClear = DOJO::controller_clear(policies_vector.data(), policies_len, controller_chain_id);
        if (resClear.tag == DOJO::Errbool) {
            Logger::error("Failed to clear Controller", resClear.err.message);
        } else {
            Logger::success("Controller cleared");
        }

        // WARNING: This will leak memory on all platforms. The 'dojo.c' library allocates the session_account
        // but does not provide a function to free it.
        session_account = nullptr;
        is_connected = false;
        call_deferred("emit_signal", "controller_disconnected");
        Logger::info("ControllerAccount disconnected");
    }
}

bool ControllerAccount::is_controller_connected() const {
    return is_connected && session_account != nullptr;
}

String ControllerAccount::get_chain_id(const bool &parse) const {
    if (Engine::get_singleton()->is_editor_hint()) return chain_id;

    if (chain_id.is_empty() && !is_controller_connected()) {
        Logger::debug_extra("ControllerAccount", "Getting chain id from provider by creating an Account");
        String _rpc_url = rpc_url;
        if (_rpc_url.is_empty()) {
            _rpc_url = DojoHelpers::get_dojo_setting("katana_url");
        }
        String _address = DojoHelpers::get_dojo_setting("account/address");
        String _private_key = DojoHelpers::get_dojo_setting("account/private_key");
        if (_rpc_url.is_empty() || _address.is_empty() || _private_key.is_empty()
            || _rpc_url == "0x0" || _address == "0x0" || _private_key == "0x0"
            || _rpc_url == "<null>" || _address == "<null>" || _private_key == "<null>"
            ) {
            Logger::error(
                "Invalid account settings",
                vformat("\n[Katana Url] %s", _rpc_url),
                vformat("\n[Account Address] %s", _address),
                vformat("\n[Private Key] %s", _private_key)
                );
            return "0x0";
        }
        Account *burner_account = memnew(Account);
        burner_account->create(_rpc_url, _address, _private_key);
        chain_id = burner_account->get_chain_id(true);
        chain_id_hex = burner_account->get_chain_id(false);
        Logger::debug_extra("CHAIN ID", chain_id, burner_account->get_chain_id());
        burner_account->queue_free();
    }
    if (parse) {
        return chain_id;
    }
    if (chain_id_hex.is_empty()) {
        DOJO::FieldElement chain_id_felt = FieldElement::cairo_short_string_to_felt(chain_id);
        chain_id_hex = FieldElement::get_as_string(&chain_id_felt);
    }
    return chain_id_hex;
}

void ControllerAccount::check_rpc_url() {
    Logger::debug_extra("ControllerAccount", "Checking RPC URL", rpc_url, "is_empty: ",rpc_url.is_empty());
    if (rpc_url.is_empty()) {
        Logger::debug_extra("ControllerAccount", "RPC URL not set, checking project settings");
        String setting = ProjectSettings::get_singleton()->get("dojo/config/katana_url");
        if (setting.is_empty()) {
            Logger::error("RPC URL not found");
            return;
        }
        rpc_url = setting;
        Logger::debug_extra("ControllerAccount", "RPC URL set to: ", rpc_url);
    }
}

String ControllerAccount::get_rpc_url() {
    return rpc_url;
}

// TODO: Ver Threads
void ControllerAccount::execute_from_outside(const String &to, const String &selector,
                                             const Variant &call_args = Array()) {
    if (!is_controller_connected()) {
        Logger::error("ControllerAccount not found");
        call_deferred("emit_signal", "transaction_failed", "Controller not found");
        return;
    }
    if (DojoHelpers::is_valid_calldata(call_args) == false) {
        return;
    }
    const Array &args = call_args;

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
    if (result.tag == DOJO::ErrFieldElement) {
        Logger::error("Transaction failed");
        Logger::error("To:", to);
        Logger::error("Selector:", selector);
        Logger::error("Args:", args);
        Logger::error("Error:", GET_DOJO_ERROR(result));
        call_deferred("emit_signal", "transaction_failed", GET_DOJO_ERROR(result));
    } else {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(result));
        Logger::success_extra("EXECUTED", selector);
    }
}

void ControllerAccount::execute_raw(const String &to, const String &selector, const Variant &call_args = Array()) {
    if (!is_controller_connected()) {
        Logger::error("ControllerAccount not found");
        call_deferred("emit_signal", "transaction_failed", "Controller not found");
        return;
    }
    if (DojoHelpers::is_valid_calldata(call_args) == false) {
        return;
    }

    const Array &args = call_args;
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
    if (result.tag == DOJO::ErrFieldElement) {
        Logger::error("Transaction failed");
        Logger::error("To:", to);
        Logger::error("Selector:", selector);
        Logger::error("Args:", args);
        Logger::error("Error:", GET_DOJO_ERROR(result));
        call_deferred("emit_signal", "transaction_failed", GET_DOJO_ERROR(result));
    } else {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(result));
        Logger::success_extra("EXECUTED", selector);
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
    info["chain_id"] = get_chain_id(true);

    return info;
}


void ControllerAccount::emit_connection_status(bool connected) {
    if (connected) {
        call_deferred("emit_signal", "controller_connected", connected);
        call_deferred("emit_signal", "current_user_info", get_account_info());
    } else {
        call_deferred("emit_signal", "controller_disconnected");
    }
}

std::vector<DOJO::Policy> ControllerAccount::build_policies(const Dictionary& policies_data) {
    if (contract_address.is_empty()) {
        Logger::debug_extra("ControllerAccount", "Contract Address not set, searching ProjectSettings");
        this->contract_address = DojoHelpers::get_dojo_setting("contract_address");
        if (contract_address.is_empty()) {
            Logger::error("Contract Address not found.");
            return {};
        }
    }
    if (policies_data.is_empty() && policies.is_empty()) {
        Logger::debug_extra("Policies", "Invalid policies data, trying from ProjectSettings");
        Dictionary settings_data = DojoHelpers::get_policies();
        if (settings_data.is_empty()) {
            Logger::error("Invalid Policies data");
            return {};
        }
        this->policies = settings_data.duplicate(true);
    } else {
        Logger::debug_extra("Policies", "Invalid policies data, trying from saving");
        this->policies = policies_data.duplicate(true);
    }

    policy_string_storage.clear();
    policy_string_storage.reserve(policies.size() * 2); // method + description for each policy

    std::vector<DOJO::Policy> policies_vector;
    policies_vector.reserve(policies.size());

    DOJO::FieldElement contract = FieldElement::from_string(contract_address);


    for (int i = 0; i < policies.size(); i++) {
        String key = policies.keys()[i];
        String value = policies[key];

        const auto &method_str = policy_string_storage.emplace_back(key.utf8().get_data());
        const auto &desc_str = policy_string_storage.emplace_back(value.utf8().get_data());

        policies_vector.push_back({contract, method_str.c_str(), desc_str.c_str()});
    }

    return policies_vector;
}
