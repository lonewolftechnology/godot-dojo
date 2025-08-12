//
// Created by hazel on 29/05/25.
//

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/dictionary.hpp"

#include "dojo_types.h"
#include "resources/dojo_call.h"

using namespace godot;

/**
 * @class Account
 * @brief Wrapper for DOJO::Account that manages accounts in Dojo
 *
 * This class provides a Godot interface for interacting with
 * Dojo accounts, including transaction execution.
 */
class Account : public Node
{
    GDCLASS(Account, Node)
    DOJO::Account* account;
    DOJO::Provider* provider;
    String rpc_url;

public:
    Account();
    ~Account();

    void create(const String& rpc_url, const String& private_key, const String& address);
    void deploy_burner(Account* master_account, const String& signing_key);

    String get_address() const;
    String get_chain_id() const;
    void set_block_id(const String& block_id);
    String get_nonce() const;

    String execute_raw(const Ref<DojoCall>& call);


protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("create", "rpc_url", "private_key", "address"), &Account::create);
        ClassDB::bind_method(D_METHOD("deploy_burner", "master_account", "signing_key"), &Account::deploy_burner);
        ClassDB::bind_method(D_METHOD("get_address"), &Account::get_address);
        ClassDB::bind_method(D_METHOD("get_chain_id"), &Account::get_chain_id);
        ClassDB::bind_method(D_METHOD("set_block_id", "block_id"), &Account::set_block_id);
        ClassDB::bind_method(D_METHOD("get_nonce"), &Account::get_nonce);
        ClassDB::bind_method(D_METHOD("execute_raw", "call"), &Account::execute_raw);
    }
};

#endif // ACCOUNT_H
