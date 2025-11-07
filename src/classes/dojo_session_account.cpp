#include "classes/dojo_session_account.h"

#include <godot_cpp/classes/json.hpp>
#include <utility>

namespace {
    controller::SessionPolicies to_c_policies(const Dictionary &policies) {
        controller::SessionPolicies c_policies;
        c_policies.max_fee = "0x0";

        if (policies.is_empty()) {
            Logger::warning("Policies dictionary is empty.");
            return c_policies;
        }

        if (policies.has("max_fee") && policies["max_fee"].get_type() == Variant::STRING) {
            c_policies.max_fee = policies["max_fee"].operator String().utf8().get_data();
            Logger::debug_extra("DojoSessionAccount", "max_fee", c_policies.max_fee.c_str());
        } else {
            Logger::warning("`max_fee` not found or not a String in policies. Using default.");
        }

        if (!policies.has("policies") || policies["policies"].get_type() != Variant::ARRAY) {
            Logger::error("`policies` key not found or is not an Array.");
            return c_policies;
        }

        Array policy_array = policies["policies"];

        for (int i = 0; i < policy_array.size(); ++i) {
            Dictionary policy_group = policy_array[i];
            if (!policy_group.has("contract_address") || !policy_group.has("entrypoints")) continue;

            String contract_address = policy_group["contract_address"];
            Array entrypoints = policy_group["entrypoints"];
            Logger::debug_extra("Policy", "Contract", contract_address);
            for (int j = 0; j < entrypoints.size(); ++j) {
                auto p = std::make_shared<controller::SessionPolicy>();
                p->contract_address = contract_address.utf8().get_data();
                p->entrypoint = String(entrypoints[j]).utf8().get_data();
                Logger::debug_extra("Policy", "Entrypoint", p->entrypoint.c_str());
                c_policies.policies.push_back(p);
            }
        }
        return c_policies;
    }
}

void DojoSessionAccount::set_internal(std::shared_ptr<controller::SessionAccount> p_internal) {
    internal = std::move(p_internal);
}

void DojoSessionAccount::create(const String &rpc_url, const String &private_key, const String &address,
                                const String &chain_id, const Dictionary &policies,
                                uint64_t session_expiration) {
    try {
        controller::SessionPolicies c_policies = to_c_policies(policies);
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

void DojoSessionAccount::create_from_subscribe(const String &_private_key, const Dictionary &_policies,
                                               const String &_rpc_url, const String &_cartridge_api_url) {
    try {
        String session_key_guid = ControllerHelper::signer_to_guid(_private_key);
        Logger::info("Using session_key_guid: " + session_key_guid);
        Logger::info("Attempting to subscribe with cartridge_api_url: " + _cartridge_api_url);
        controller::SessionPolicies c_policies = to_c_policies(_policies);
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
        return "";
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
        String tx_hash = String(internal->execute(c_calls).c_str());
        Logger::success_extra("DojoSessionAccount", "Execute successful. Tx hash:", tx_hash);
        return tx_hash;
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.execute failed:", e.what());
        return "";
    }
}

String DojoSessionAccount::execute_from_outside(const TypedArray<Dictionary> &calls) const {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }

    const std::vector<std::shared_ptr<controller::Call>> c_calls = ControllerHelper::prepare_calls(calls);
    try {
        String tx_hash = String(internal->execute_from_outside(c_calls).c_str());
        Logger::success_extra("DojoSessionAccount", "Execute from outside successful. Tx hash:", tx_hash);
        return tx_hash;
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.execute_from_outside failed:", e.what());
        return "";
    }
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

String DojoSessionAccount::generate_session_request_url(const String &base_url, const String &public_key,
                                                        const Dictionary &policies, const String &rpc_url,
                                                        const String &redirect_uri, const String &redirect_query_name) {
    if (base_url.is_empty() || public_key.is_empty() || policies.is_empty() || rpc_url.is_empty()) {
        Logger::error("generate_session_request_url: base_url, public_key, policies, and rpc_url are required.");
        return "";
    }

    const String policies_json_string = JSON::stringify(policies);
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
