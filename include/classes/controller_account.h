//
// Created by hazel on 26/05/25.
//

#ifndef CONTROLLER_ACCOUNT_H
#define CONTROLLER_ACCOUNT_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/dictionary.hpp"

#include "dojo_types.h"
#include "resources/dojo_policies.h"
using namespace godot;

/**
 * @class ControllerAccount
 * @brief Wrapper para DOJO::ControllerAccount que gestiona cuentas de controlador en Dojo
 *
 * Esta clase proporciona una interfaz Godot para interactuar con cuentas de controlador
 * del ecosistema Dojo, incluyendo funcionalidades de autenticación, ejecución de transacciones
 * y gestión de sesiones.
 */
class ControllerAccount : public Node
{
    GDCLASS(ControllerAccount, Node)

    DOJO::ControllerAccount* session_account;
    DOJO::Provider* provider;
    bool is_connected;
    static ControllerAccount* singleton;
    Ref<DojoPolicies> policies;
    String rpc_url;

    static void on_account_callback(DOJO::ControllerAccount* account);
    mutable String chain_id;
    mutable String chain_id_hex;

public:
    ControllerAccount();
    ~ControllerAccount();

    static ControllerAccount* get_singleton();

    void set_session_account(DOJO::ControllerAccount* account);
    DOJO::ControllerAccount* get_session_account() const;

    void setup();
    void init_provider();
    void create(const Ref<DojoPolicies>& policies_data = nullptr);
    void disconnect_controller();
    bool is_controller_connected() const;

    String get_username() const
    {
        if (!is_controller_connected())
        {
            return "";
        }
        return {DOJO::controller_username(session_account)};
    }

    String get_address() const
    {
        if (!is_controller_connected())
        {
            return "0x0";
        }
        DOJO::FieldElement felt = DOJO::controller_address(session_account);
        return FieldElement::get_as_string(&felt);
    }

    String get_chain_id(const bool& parse = true) const;
    void set_chain_id(const String& p_chain_id) { chain_id = p_chain_id; }
    String _get_chain_id() const { return chain_id; }

    void check_rpc_url();
    String get_rpc_url();
    void set_rpc_url(const String& p_rpc_url) { rpc_url = p_rpc_url; };

    void execute_from_outside(const String& to, const String& selector, const Variant& args);
    void execute_raw(const String& to, const String& selector, const Variant& args);

    Dictionary get_account_info() const;

    void emit_connection_status(bool connected);

    void set_policies(const Ref<DojoPolicies>& p_policies) { policies = p_policies; };

    Ref<DojoPolicies> get_policies() { return policies; };

protected:

    static void _bind_methods()
    {
        // ClassDB::bind_method(D_METHOD("create", "rpc_url"),&ControllerAccount::create);
        ClassDB::bind_method(D_METHOD("init_provider"), &ControllerAccount::init_provider);
        ClassDB::bind_method(D_METHOD("create", "policies_data"), &ControllerAccount::create, DEFVAL(Variant()));
        ClassDB::bind_method(D_METHOD("setup"), &ControllerAccount::setup);
        ClassDB::bind_method(D_METHOD("disconnect_controller"), &ControllerAccount::disconnect_controller);
        ClassDB::bind_method(D_METHOD("is_controller_connected"), &ControllerAccount::is_controller_connected);

        ClassDB::bind_method(D_METHOD("get_username"), &ControllerAccount::get_username);
        ClassDB::bind_method(D_METHOD("get_address"), &ControllerAccount::get_address);

        ClassDB::bind_method(D_METHOD("execute_from_outside", "to", "selector", "args"),
                             &ControllerAccount::execute_from_outside, DEFVAL(Array()));
        ClassDB::bind_method(D_METHOD("execute_raw", "to", "selector", "args"),
                             &ControllerAccount::execute_raw, DEFVAL(Array()));

        ClassDB::bind_method(D_METHOD("get_account_info"), &ControllerAccount::get_account_info);
        // ClassDB::bind_method(D_METHOD("sign_message", "message"), &ControllerAccount::sign_message);

        ADD_SIGNAL(MethodInfo("controller_connected", PropertyInfo(Variant::BOOL, "success")));
        ADD_SIGNAL(MethodInfo("provider_status_updated", PropertyInfo(Variant::BOOL, "success")));
        ADD_SIGNAL(MethodInfo("controller_disconnected"));
        ADD_SIGNAL(MethodInfo("transaction_executed", PropertyInfo(Variant::STRING, "transaction_hash")));
        ADD_SIGNAL(MethodInfo("transaction_failed", PropertyInfo(Variant::STRING, "error_message")));
        ADD_SIGNAL(MethodInfo("current_user_info", PropertyInfo(Variant::DICTIONARY, "user_info")));

        ClassDB::bind_method(D_METHOD("set_rpc_url", "rpc_url"), &ControllerAccount::set_rpc_url);
        ClassDB::bind_method(D_METHOD("get_rpc_url"), &ControllerAccount::get_rpc_url);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "rpc_url"), "set_rpc_url", "get_rpc_url");

        ClassDB::bind_method(D_METHOD("set_chain_id", "chain_id"), &ControllerAccount::set_chain_id);
        ClassDB::bind_method(D_METHOD("get_chain_id"), &ControllerAccount::_get_chain_id);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "chain_id"), "set_chain_id", "get_chain_id");

        ClassDB::bind_method(D_METHOD("set_policies", "policies"), &ControllerAccount::set_policies);
        ClassDB::bind_method(D_METHOD("get_policies"), &ControllerAccount::get_policies);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "policies", PROPERTY_HINT_RESOURCE_TYPE, "DojoPolicies"),
                     "set_policies", "get_policies");
    }

};

#endif // CONTROLLER_ACCOUNT_H
