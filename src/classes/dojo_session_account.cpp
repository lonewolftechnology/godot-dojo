#include "classes/dojo_session_account.h"

#include <random>

namespace {
    controller::SessionPolicies to_c_policies(const Dictionary &policies) {
        if (policies.is_empty()) {
            Logger::warning("Policies dictionary is empty.");
            return {};
        }

        controller::SessionPolicies c_policies;

        if (policies.has("max_fee") && policies["max_fee"].get_type() == Variant::STRING) {
            c_policies.max_fee = policies["max_fee"].operator String().utf8().get_data();
        } else {
            Logger::warning("`max_fee` not found or not a String in policies. Using default.");
            c_policies.max_fee = "0x0"; // O un valor por defecto razonable
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
            
            for (int j = 0; j < entrypoints.size(); ++j) {
                auto p = std::make_shared<controller::SessionPolicy>();
                p->contract_address = contract_address.utf8().get_data();
                p->entrypoint = String(entrypoints[j]).utf8().get_data();
                c_policies.policies.push_back(p);
            }
        }
        return c_policies;
    }
}

void DojoSessionAccount::set_internal(std::shared_ptr<controller::SessionAccount> p_internal) {
    internal = p_internal;
}

void DojoSessionAccount::create(const String &rpc_url, const String &private_key, const String &address,
                                const String &chain_id, const Dictionary &policies,
                                uint64_t session_expiration) {
    try {
        controller::SessionPolicies c_policies = to_c_policies(policies);
        controller::FieldElement owner_guid = controller::signer_to_guid(private_key.utf8().get_data());
        auto internal_account = controller::SessionAccount::init(
            rpc_url.utf8().get_data(),
            private_key.utf8().get_data(),
            address.utf8().get_data(),
            owner_guid,
            chain_id.utf8().get_data(),
            c_policies,
            session_expiration
        );
        set_internal(internal_account);
        // TODO: hacer controller_types.hpp para macro de esto
        auto username_opt = internal_account->username();
        Logger::debug_extra("DojoSessionAccount", "username", username_opt.value_or("N/A").c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.create failed:", e.what());
    }
}

void DojoSessionAccount::create_from_subscribe(const String &_private_key, const Dictionary &_policies,
                                               const String &_rpc_url, const String &_cartridge_api_url) {
    try {
        Logger::info("Attempting to subscribe with cartridge_api_url: " + _cartridge_api_url);
        controller::SessionPolicies c_policies = to_c_policies(_policies);
        auto internal_account = controller::SessionAccount::create_from_subscribe(
            _private_key.utf8().get_data(),
            c_policies,
            _rpc_url.utf8().get_data(),
            _cartridge_api_url.utf8().get_data()
        );
        set_internal(internal_account);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.create_from_subscribe failed:", e.what());
        throw;
    }
}

void DojoSessionAccount::set_private_key(const String &p_private_key) {
    private_key = p_private_key;
}

String DojoSessionAccount::get_private_key() {
    return private_key;
}

void DojoSessionAccount::set_policies(const Dictionary &p_policies) {
    policies = p_policies;
}

Dictionary DojoSessionAccount::get_policies() const {
    return policies;
}

void DojoSessionAccount::set_rpc_url(const String &p_rpc_url) {
    rpc_url = p_rpc_url;
}

String DojoSessionAccount::get_rpc_url() const {
    return rpc_url;
}

void DojoSessionAccount::set_cartridge_api_url(const String &p_cartridge_api_url) {
    cartridge_api_url = p_cartridge_api_url;
}

String DojoSessionAccount::get_cartridge_api_url() const {
    return cartridge_api_url;
}

String DojoSessionAccount::execute(const TypedArray<Dictionary> &calls) {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }

    std::vector<std::shared_ptr<controller::Call>> c_calls;
    for (int i = 0; i < calls.size(); ++i) {
        Dictionary call_dict = calls[i];
        auto c = std::make_shared<controller::Call>();
        c->contract_address = call_dict["contract_address"].operator String().utf8().get_data();
        c->entrypoint = call_dict["entrypoint"].operator String().utf8().get_data();

        Array calldata_array = call_dict["calldata"];
        for (int j = 0; j < calldata_array.size(); ++j) {
            c->calldata.push_back(calldata_array[j].operator String().utf8().get_data());
        }
        c_calls.push_back(c);
    }

    try {
        return String(internal->execute(c_calls).c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.execute failed:", e.what());
        return "";
    }
}

String DojoSessionAccount::execute_from_outside(const TypedArray<Dictionary> &calls) {
    if (!internal) {
        Logger::error("DojoSessionAccount is not initialized.");
        return "";
    }

    std::vector<std::shared_ptr<controller::Call>> c_calls;
    for (int i = 0; i < calls.size(); ++i) {
        Dictionary call_dict = calls[i];
        auto c = std::make_shared<controller::Call>();
        c->contract_address = call_dict["contract_address"].operator String().utf8().get_data();
        c->entrypoint = call_dict["entrypoint"].operator String().utf8().get_data();

        Array calldata_array = call_dict["calldata"];
        for (int j = 0; j < calldata_array.size(); ++j) {
            c->calldata.push_back(calldata_array[j].operator String().utf8().get_data());
        }
        c_calls.push_back(c);
    }

    try {
        return String(internal->execute_from_outside(c_calls).c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoSessionAccount.execute_from_outside failed:", e.what());
        return "";
    }
}

String DojoSessionAccount::generate_private_key() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);

    std::stringstream ss;
    ss << "0x";
    for (int i = 0; i < 32; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)dis(gen);
    }
    return ss.str().data();
}

bool DojoSessionAccount::is_expired() const {
    if (!internal) {
        return true;
    }
    return internal->is_expired();
}
