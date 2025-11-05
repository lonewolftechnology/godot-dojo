#ifndef DOJO_SESSION_ACCOUNT_H
#define DOJO_SESSION_ACCOUNT_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "controller/controller.hpp"
#include "tools/controller_helper.h"
#include "tools/logger.h"

using namespace godot;

class DojoSessionAccount : public Node {
    GDCLASS(DojoSessionAccount, Node)
    std::shared_ptr<controller::SessionAccount> internal;

public:
    DojoSessionAccount() = default;

    ~DojoSessionAccount() override = default;

    void create_from_subscribe(const String &_private_key, const Dictionary &_policies, const String &_rpc_url,
                               const String &_cartridge_api_url);

    void create(const String &rpc_url, const String &private_key, const String &address,
        const String &chain_id, const Dictionary &policies, uint64_t session_expiration);

    String get_address() const;
    String get_chain_id() const;
    String get_app_id() const;
    String execute(const TypedArray<Dictionary> &calls) const;
    String execute_from_outside(const TypedArray<Dictionary> &calls) const;
    uint64_t get_expires_at() const;
    bool is_expired() const;
    bool is_revoked() const;
    String get_owner_guid() const;
    String get_session_id() const;
    String get_username() const;

    static String generate_session_request_url(const String &base_url, const String &public_key,
                                               const Dictionary &policies,
                                               const String &rpc_url, const String &redirect_uri = String(),
                                               const String &redirect_query_name = String());



protected:
    void set_internal(std::shared_ptr<controller::SessionAccount> p_internal);

    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("create_from_subscribe", "private_key", "policies", "rpc_url",
                                      "cartridge_api_url"),
                             &DojoSessionAccount::create_from_subscribe);
        ClassDB::bind_method(D_METHOD("create", "rpc_url", "private_key", "address", "chain_id",
                                      "policies", "session_expiration"), &DojoSessionAccount::create);

        ClassDB::bind_method(D_METHOD("get_address"), &DojoSessionAccount::get_address);
        ClassDB::bind_method(D_METHOD("execute", "calls"), &DojoSessionAccount::execute);
        ClassDB::bind_method(D_METHOD("execute_from_outside", "calls"), &DojoSessionAccount::execute_from_outside);
        ClassDB::bind_method(D_METHOD("get_expires_at"), &DojoSessionAccount::get_expires_at);
        ClassDB::bind_method(D_METHOD("is_expired"), &DojoSessionAccount::is_expired);
        ClassDB::bind_method(D_METHOD("is_revoked"), &DojoSessionAccount::is_revoked);
        ClassDB::bind_method(D_METHOD("get_chain_id"), &DojoSessionAccount::get_chain_id);

        ClassDB::bind_static_method("DojoSessionAccount",
                                    D_METHOD("generate_session_request_url", "base_url", "public_key", "policies",
                                             "rpc_url", "redirect_uri", "redirect_query_name"),
                                    &DojoSessionAccount::generate_session_request_url, DEFVAL(String()), DEFVAL(String()));
    }
};
VARIANT_ENUM_CAST(controller::SignerType);

#endif //DOJO_SESSION_ACCOUNT_H
