#include "classes/dojo_session_account.h"

#include <godot_cpp/classes/json.hpp>
#include <utility>

void DojoSessionAccount::set_internal(std::shared_ptr<controller::SessionAccount> p_internal) {
    internal = std::move(p_internal);
}

void DojoSessionAccount::create(const String &rpc_url, const String &private_key, const String &address,
                                const String &chain_id, uint64_t session_expiration) {
    try {
        controller::SessionPolicies c_policies = ControllerHelper::to_c_policies(get_session_policy());
        if (!ControllerHelper::validate_felt(address)) {
            Logger::error("Invalid address format.");
            return;
        }
        String owner_guid = ControllerHelper::signer_to_guid(private_key);
        auto internal_account = controller::SessionAccount::init(
            rpc_url.utf8().get_data(),
            private_key.utf8().get_data(),
            address.utf8().get_data(),
            owner_guid.utf8().get_data(),
            chain_id.utf8().get_data(),
            c_policies,
            session_expiration
        );
        set_internal(internal_account);
        auto username_opt = internal_account->username();
        if (username_opt.has_value()) {
            Logger::debug_extra("DojoSessionAccount", "username", username_opt.value().c_str());
        } else {
            Logger::debug_extra("DojoSessionAccount", "username", "N/A");
        }
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.create failed:", e.what());
    }
}

void DojoSessionAccount::create_from_subscribe(const String &_private_key,
                                               const String &_rpc_url, const String &_cartridge_api_url, const Dictionary &_policies) {
    try {
        String session_key_guid = ControllerHelper::signer_to_guid(_private_key);
        Logger::info("Using session_key_guid: " + session_key_guid);
        Logger::info("Attempting to subscribe with cartridge_api_url: " + _cartridge_api_url);
        
        Dictionary policies_to_use = _policies.is_empty() ? get_session_policy() : _policies;
        controller::SessionPolicies c_policies = ControllerHelper::to_c_policies(policies_to_use);

        auto internal_account = controller::SessionAccount::create_from_subscribe(
            _private_key.utf8().get_data(),
            c_policies,
            _rpc_url.utf8().get_data(),
            _cartridge_api_url.utf8().get_data()
        );
        Logger::success_extra("DojoSessionAccount", "Session account created successfully");
        set_internal(internal_account);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.create_from_subscribe failed:", e.what());
        internal.reset(); // Asegurarse de que el puntero interno es nulo en caso de fallo
    }
}

String DojoSessionAccount::get_address() const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "0x0";
    }
    return {internal->address().c_str()};
}

String DojoSessionAccount::execute(const TypedArray<Dictionary> &calls) const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }

    std::vector<std::shared_ptr<controller::Call> > c_calls = ControllerHelper::prepare_calls(calls);

    try {
        String tx_hash = internal->execute(c_calls).c_str();
        Logger::success_extra("DojoSessionAccount", "Execute successful. Tx hash:", tx_hash);
        return tx_hash;
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.execute failed:", e.what());
        return e.what();
    }
}

String DojoSessionAccount::execute_raw(const String &contract_address, const String &entrypoint, const Array &calldata) const {
    Dictionary _call_dict;
    _call_dict["contract_address"] = contract_address;
    _call_dict["entrypoint"] = entrypoint;
    _call_dict["calldata"] = calldata;

    return execute(Array::make(_call_dict));
}


String DojoSessionAccount::execute_from_outside(const TypedArray<Dictionary> &calls) const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }

    std::vector<std::shared_ptr<controller::Call> > c_calls = ControllerHelper::prepare_calls(calls);

    try {
        String tx_hash = internal->execute_from_outside(c_calls).c_str();
        Logger::success_extra("DojoSessionAccount", "Execute from outside successful. Tx hash:", tx_hash);
        return tx_hash;
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.execute_from_outside failed:", e.what());
        return e.what();
    }
}

String DojoSessionAccount::execute_from_outside_raw(const String &contract_address, const String &entrypoint, const Array &calldata) const {
    Dictionary _call_dict;
    _call_dict["contract_address"] = contract_address;
    _call_dict["entrypoint"] = entrypoint;
    _call_dict["calldata"] = calldata;

    return execute_from_outside(Array::make(_call_dict));
}


uint64_t DojoSessionAccount::get_expires_at() const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return 0;
    }
    return internal->expires_at();
}

bool DojoSessionAccount::is_expired() const {
    if (!internal) {
        return true;
    }
    return internal->is_expired();
}

bool DojoSessionAccount::is_revoked() const {
    if (!internal) {
        return true;
    }
    return internal->is_revoked();
}

String DojoSessionAccount::get_owner_guid() const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }
    return {internal->owner_guid().c_str()};
}

String DojoSessionAccount::get_session_id() const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }
    auto session_id_opt = internal->session_id();
    return session_id_opt.has_value() ? String(session_id_opt.value().c_str()) : "";
}

String DojoSessionAccount::get_username() const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }
    auto username_opt = internal->username();
    return username_opt.has_value() ? String(username_opt.value().c_str()) : "";
}

String DojoSessionAccount::get_chain_id() const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }
    return {internal->chain_id().c_str()};
}

String DojoSessionAccount::get_app_id() const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }
    return internal->app_id().has_value() ? String(internal->app_id().value().c_str()) : "";
}

Dictionary DojoSessionAccount::get_info() const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return {};
    }

    Dictionary info;
    info["address"] = get_address();
    info["chain_id"] = get_chain_id();
    info["app_id"] = get_app_id();
    info["expires_at"] = get_expires_at();
    info["is_expired"] = is_expired();
    info["is_revoked"] = is_revoked();
    info["owner_guid"] = get_owner_guid();
    info["session_id"] = get_session_id();
    info["username"] = get_username();
    return info;
}

bool DojoSessionAccount::is_valid() const {
    if (!internal) {
        return false;
    }
    return true;
}

String DojoSessionAccount::generate_session_request_url(const String &base_url, const String &public_key,
                                                        const String &rpc_url, const Dictionary &policies,
                                                        const String &redirect_uri, const String &redirect_query_name) const {
    if (base_url.is_empty() || public_key.is_empty() || rpc_url.is_empty()) {
        Logger::error("generate_session_request_url: base_url, public_key, and rpc_url are required.");
        return "";
    }

    const Dictionary policies_to_use = policies.is_empty() ? get_session_policy() : policies;
    const String policies_json_string = JSON::stringify(policies_to_use);
    const String encoded_policies = policies_json_string.uri_encode();

    String url = String("{0}?public_key={1}&policies={2}&rpc_url={3}").format(
        Array::make(base_url, public_key, encoded_policies, rpc_url.uri_encode()));

    if (!redirect_uri.is_empty()) {
        url += "&redirect_uri=" + redirect_uri.uri_encode();
    }
    if (!redirect_query_name.is_empty()) {
        url += "&redirect_query_name=" + redirect_query_name;
    }

    return url;
}

Dictionary DojoSessionAccount::get_session_policy() const
{
    Dictionary session_policy;
    session_policy["max_fee"] = max_fee;

    Array policies_array;
    const Array contracts = full_policies.keys();
    for (int i = 0; i < contracts.size(); ++i) {
        const String& contract_address = contracts[i];
        const Dictionary& contract_details = full_policies[contract_address];
        const Array& methods = contract_details["methods"];

        Array entrypoints;
        for (int j = 0; j < methods.size(); ++j) {
            const Dictionary& method = methods[j];
            entrypoints.push_back(method["entrypoint"]);
        }

        Dictionary policy_group;
        policy_group["contract_address"] = contract_address;
        policy_group["entrypoints"] = entrypoints;
        policies_array.push_back(policy_group);
    }

    session_policy["policies"] = policies_array;
    return session_policy;
}

Dictionary DojoSessionAccount::get_register_session_policy() const
{
    Dictionary register_session_policy;
    register_session_policy["contracts"] = full_policies;
    return register_session_policy;
}
