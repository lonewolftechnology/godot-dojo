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
    void start_provider();

    void create(const String& _rpc_url, const String& address, const String& private_key);
    void deploy_burner(Account* master_account, const String& signing_key);

    String get_address() const;
    String get_chain_id(const bool& parse = false) const;
    void set_block_id(const String& block_id = String());
    String get_nonce() const;

    bool is_account_valid() const;

    void execute_raw(const String& to, const String& selector, const Array& args = Array());


protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("create", "rpc_url", "address", "private_key"), &Account::create);
        ClassDB::bind_method(D_METHOD("start_provider"), &Account::start_provider);
        ClassDB::bind_method(D_METHOD("deploy_burner", "master_account", "signing_key"), &Account::deploy_burner);
        ClassDB::bind_method(D_METHOD("get_address"), &Account::get_address);
        ClassDB::bind_method(D_METHOD("get_chain_id", "parse"), &Account::get_chain_id, DEFVAL(false));
        ClassDB::bind_method(D_METHOD("set_block_id", "block_id"), &Account::set_block_id, DEFVAL(String()));
        ClassDB::bind_method(D_METHOD("get_nonce"), &Account::get_nonce);
        ClassDB::bind_method(D_METHOD("execute_raw", "to", "selector", "calldata"), &Account::execute_raw, DEFVAL(Array()));
        ClassDB::bind_method(D_METHOD("is_account_valid"), &Account::is_account_valid);

        ADD_SIGNAL(MethodInfo("transaction_executed", PropertyInfo(Variant::DICTIONARY, "success_message")));
        ADD_SIGNAL(MethodInfo("transaction_failed", PropertyInfo(Variant::DICTIONARY, "error_message")));

    }
};

#endif // ACCOUNT_H
