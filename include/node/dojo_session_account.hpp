#pragma once
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "controller/controller.hpp"
#include "tools/controller_helper.h"
#include "tools/logger.hpp"

using namespace godot;

class DojoSessionAccount : public Node {
    GDCLASS(DojoSessionAccount, Node)
    std::shared_ptr<controller::SessionAccount> internal;

    Dictionary full_policies;
    String max_fee;

public:
    DojoSessionAccount() = default;

    ~DojoSessionAccount() override = default;

    void create_from_subscribe(const String &_private_key, const String &_rpc_url,
                               const String &_cartridge_api_url, const Dictionary &_policies = Dictionary());

    void create(const String &rpc_url, const String &private_key, const String &address,
        const String &chain_id, uint64_t session_expiration);

    String get_address() const;
    String get_chain_id() const;
    String get_app_id() const;

    String execute(const TypedArray<Dictionary> &calls) const;
    String execute_raw(const String &contract_address, const String &entrypoint, const Array &calldata) const;

    String execute_from_outside(const TypedArray<Dictionary> &calls) const;
    String execute_from_outside_raw(const String &contract_address, const String &entrypoint, const Array &calldata) const;
    Variant execute_test(const Variant **calldata, GDExtensionInt arg_count, GDExtensionCallError &error);

    uint64_t get_expires_at() const;
    bool is_expired() const;
    bool is_revoked() const;
    String get_owner_guid() const;
    String get_session_id() const;
    String get_username() const;
    Dictionary get_info() const;
    bool is_valid() const;
    void close();

    String generate_session_request_url(const String &base_url, const String &public_key,
                                        const String &rpc_url, const Dictionary &policies = Dictionary(),
                                        const String &redirect_uri = String(),
                                        const String &redirect_query_name = String()) const;

    void set_full_policies(const Dictionary &p_policies){ full_policies = p_policies; }
    Dictionary get_full_policies() const { return full_policies; }

    Dictionary get_session_policy() const;
    Dictionary get_register_session_policy() const;

    void set_max_fee(const String &p_max_fee){ max_fee = p_max_fee; }
    String get_max_fee() const { return max_fee; }


protected:
    void set_internal(std::shared_ptr<controller::SessionAccount> p_internal);

    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("create_from_subscribe", "private_key", "rpc_url",
                                      "cartridge_api_url", "policies"),
                             &DojoSessionAccount::create_from_subscribe, DEFVAL(Dictionary()));
        ClassDB::bind_method(D_METHOD("create", "rpc_url", "private_key", "address", "chain_id", "session_expiration"), &DojoSessionAccount::create);

        ClassDB::bind_method(D_METHOD("get_address"), &DojoSessionAccount::get_address);

        ClassDB::bind_method(D_METHOD("execute", "calls"), &DojoSessionAccount::execute);
        ClassDB::bind_method(D_METHOD("execute_raw", "contract_address", "entrypoint", "calldata"), &DojoSessionAccount::execute_raw);

        ClassDB::bind_method(D_METHOD("execute_from_outside", "calls"), &DojoSessionAccount::execute_from_outside);
        ClassDB::bind_method(D_METHOD("execute_from_outside_raw", "contract_address", "entrypoint", "calldata"), &DojoSessionAccount::execute_from_outside_raw);

        ClassDB::bind_method(D_METHOD("get_expires_at"), &DojoSessionAccount::get_expires_at);
        ClassDB::bind_method(D_METHOD("is_expired"), &DojoSessionAccount::is_expired);
        ClassDB::bind_method(D_METHOD("is_revoked"), &DojoSessionAccount::is_revoked);
        ClassDB::bind_method(D_METHOD("get_chain_id"), &DojoSessionAccount::get_chain_id);
        ClassDB::bind_method(D_METHOD("get_info"), &DojoSessionAccount::get_info);
        ClassDB::bind_method(D_METHOD("is_valid"), &DojoSessionAccount::is_valid);
        ClassDB::bind_method(D_METHOD("close"), &DojoSessionAccount::close);


        {
            MethodInfo mi;
            mi.arguments.push_back(PropertyInfo(Variant::STRING, "contract_address"));
            mi.arguments.push_back(PropertyInfo(Variant::STRING, "entrypoint"));
            mi.name = "execute_test";
            ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "execute_test", &DojoSessionAccount::execute_test, mi);
        }

        // Properties
        ClassDB::bind_method(D_METHOD("get_full_policies"), &DojoSessionAccount::get_full_policies);
        ClassDB::bind_method(D_METHOD("set_full_policies"), &DojoSessionAccount::set_full_policies);
        ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "full_policies"), "set_full_policies", "get_full_policies");

        ClassDB::bind_method(D_METHOD("get_session_policy"), &DojoSessionAccount::get_session_policy);
        ClassDB::bind_method(D_METHOD("get_register_session_policy"), &DojoSessionAccount::get_register_session_policy);

        ClassDB::bind_method(D_METHOD("get_max_fee"), &DojoSessionAccount::get_max_fee);
        ClassDB::bind_method(D_METHOD("set_max_fee"), &DojoSessionAccount::set_max_fee);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "max_fee"), "set_max_fee", "get_max_fee");

        ClassDB::bind_method(D_METHOD("generate_session_request_url", "base_url", "public_key",
                                      "rpc_url", "policies", "redirect_uri", "redirect_query_name"),
                             &DojoSessionAccount::generate_session_request_url, DEFVAL(Dictionary()),
                             DEFVAL(String()), DEFVAL(String()));

    }
};
//VARIANT_ENUM_CAST(controller::SignerType);
