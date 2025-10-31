#ifndef DOJO_SESSION_ACCOUNT_H
#define DOJO_SESSION_ACCOUNT_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "controller/controller.hpp"
#include "tools/dojo_helper.h"
#include "tools/logger.h"

using namespace godot;

class DojoSessionAccount : public Node {
    GDCLASS(DojoSessionAccount, Node)
    std::shared_ptr<controller::SessionAccount> internal;
    String private_key;
    Dictionary policies;
    String rpc_url;
    String cartridge_api_url;

public:
    DojoSessionAccount() = default;

    ~DojoSessionAccount() override = default;

    void create_from_subscribe(const String &_private_key, const Dictionary &_policies, const String &_rpc_url,
                               const String &_cartridge_api_url);

    void create(const String &rpc_url, const String &private_key, const String &address,
        const String &chain_id, const Dictionary &policies, uint64_t session_expiration);

    void set_private_key(const String &p_private_key);

    String get_private_key();

    void set_policies(const Dictionary &p_policies);

    Dictionary get_policies() const;

    void set_rpc_url(const String &p_rpc_url);

    String get_rpc_url() const;

    void set_cartridge_api_url(const String &p_cartridge_api_url);

    String get_cartridge_api_url() const;

    String execute(const TypedArray<Dictionary> &calls);

    String execute_from_outside(const TypedArray<Dictionary> &calls);

    String generate_private_key();

    bool is_expired() const;

protected:
    void set_internal(std::shared_ptr<controller::SessionAccount> p_internal);

    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("create_from_subscribe", "private_key", "policies", "rpc_url",
                                      "cartridge_api_url"),
                             &DojoSessionAccount::create_from_subscribe);
        ClassDB::bind_method(D_METHOD("create", "rpc_url", "private_key", "address", "chain_id",
                                      "policies", "session_expiration"), &DojoSessionAccount::create);

        ClassDB::bind_method(D_METHOD("generate_private_key"), &DojoSessionAccount::generate_private_key);

        ClassDB::bind_method(D_METHOD("set_private_key", "p_private_key"), &DojoSessionAccount::set_private_key);
        ClassDB::bind_method(D_METHOD("get_private_key"), &DojoSessionAccount::get_private_key);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "private_key"), "set_private_key", "get_private_key");

        ClassDB::bind_method(D_METHOD("set_policies", "p_policies"), &DojoSessionAccount::set_policies);
        ClassDB::bind_method(D_METHOD("get_policies"), &DojoSessionAccount::get_policies);
        ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "policies"),
                     "set_policies", "get_policies");


        ClassDB::bind_method(D_METHOD("set_rpc_url", "p_rpc_url"), &DojoSessionAccount::set_rpc_url);
        ClassDB::bind_method(D_METHOD("get_rpc_url"), &DojoSessionAccount::get_rpc_url);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "rpc_url"), "set_rpc_url", "get_rpc_url");

        ClassDB::bind_method(D_METHOD("set_cartridge_api_url", "p_cartridge_api_url"),
                             &DojoSessionAccount::set_cartridge_api_url);
        ClassDB::bind_method(D_METHOD("get_cartridge_api_url"), &DojoSessionAccount::get_cartridge_api_url);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "cartridge_api_url"), "set_cartridge_api_url",
                     "get_cartridge_api_url");

        ClassDB::bind_method(D_METHOD("execute", "calls"), &DojoSessionAccount::execute);
        ClassDB::bind_method(D_METHOD("execute_from_outside", "calls"), &DojoSessionAccount::execute_from_outside);
        ClassDB::bind_method(D_METHOD("is_expired"), &DojoSessionAccount::is_expired);

    }
};

#endif //DOJO_SESSION_ACCOUNT_H
