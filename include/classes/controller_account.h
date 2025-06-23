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
#include "resources/dojo_call.h"
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

public:
    ControllerAccount();
    ~ControllerAccount();

    // Singleton pattern
    static ControllerAccount* get_singleton();


    // Métodos principales de configuración
    void set_session_account(DOJO::ControllerAccount* account);
    DOJO::ControllerAccount* get_session_account() const;

    void setup();
    // Métodos de conexión y autenticación
    void create(const Ref<DojoPolicies>& policies_data);
    void disconnect_controller();
    bool is_controller_connected() const;

    // Información de la cuenta
    String get_username() const;
    String get_address() const;
    String get_chain_id() const;

    // Ejecución de transacciones
    void execute_raw(const Ref<DojoCall>& action);
    void execute_from_outside(const Ref<DojoCall>& action);

    // Métodos de utilidad
    Dictionary get_account_info() const;

    // Señales de estado
    void emit_connection_status(bool connected);

    void set_policies(const Ref<DojoPolicies>& p_policies){policies = p_policies;};

    Ref<DojoPolicies> get_policies() {return policies;};


protected:


    static void _bind_methods()
    {
        // Métodos de conexión
        // ClassDB::bind_method(D_METHOD("create", "rpc_url"),&ControllerAccount::create);
        ClassDB::bind_method(D_METHOD("create", "policies_data"), &ControllerAccount::create);
        ClassDB::bind_method(D_METHOD("setup"), &ControllerAccount::setup);
        ClassDB::bind_method(D_METHOD("disconnect_controller"), &ControllerAccount::disconnect_controller);
        ClassDB::bind_method(D_METHOD("is_controller_connected"), &ControllerAccount::is_controller_connected);

        // Información de la cuenta
        ClassDB::bind_method(D_METHOD("get_username"), &ControllerAccount::get_username);
        ClassDB::bind_method(D_METHOD("get_address"), &ControllerAccount::get_address);
        ClassDB::bind_method(D_METHOD("get_chain_id"), &ControllerAccount::get_chain_id);

        // Ejecución de transacciones
        ClassDB::bind_method(D_METHOD("execute_raw", "action"),
                             &ControllerAccount::execute_raw, DEFVAL(Array()));
        ClassDB::bind_method(D_METHOD("execute_from_outside", "action"),
                             &ControllerAccount::execute_from_outside, DEFVAL(Array()));

        // Métodos de utilidad
        ClassDB::bind_method(D_METHOD("get_account_info"), &ControllerAccount::get_account_info);
        // ClassDB::bind_method(D_METHOD("sign_message", "message"), &ControllerAccount::sign_message);

        // Señales
        ADD_SIGNAL(MethodInfo("controller_connected", PropertyInfo(Variant::BOOL, "success")));
        ADD_SIGNAL(MethodInfo("provider_status_updated", PropertyInfo(Variant::BOOL, "success")));
        ADD_SIGNAL(MethodInfo("controller_disconnected"));
        ADD_SIGNAL(MethodInfo("transaction_executed", PropertyInfo(Variant::STRING, "transaction_hash")));
        ADD_SIGNAL(MethodInfo("transaction_failed", PropertyInfo(Variant::STRING, "error_message")));

        ClassDB::bind_method(D_METHOD("set_policies", "policies"), &ControllerAccount::set_policies);
        ClassDB::bind_method(D_METHOD("get_policies"), &ControllerAccount::get_policies);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "policies", PROPERTY_HINT_RESOURCE_TYPE, "DojoPolicies"), "set_policies", "get_policies");

    }

};

#endif // CONTROLLER_ACCOUNT_H