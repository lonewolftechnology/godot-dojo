//
// Created by hazel on 26/05/25.
//

#ifndef CONTROLLER_ACCOUNT_H
#define CONTROLLER_ACCOUNT_H

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/dictionary.hpp"

#include "dojo_types.h"
#include "variant/field_element.h"
using namespace godot;

/**
 * @class ControllerAccount
 * @brief Wrapper para DOJO::ControllerAccount que gestiona cuentas de controlador en Dojo
 *
 * Esta clase proporciona una interfaz Godot para interactuar con cuentas de controlador
 * del ecosistema Dojo, incluyendo funcionalidades de autenticación, ejecución de transacciones
 * y gestión de sesiones.
 */
class ControllerAccount : public RefCounted
{
    GDCLASS(ControllerAccount, RefCounted)
    DOJO::ControllerAccount* session_account;
    DOJO::Provider* provider;
    bool is_connected;
    friend class FieldElementDictionary;

protected:
    static void _bind_methods();
    static ControllerAccount* singleton;

public:
    ControllerAccount();
    ~ControllerAccount();

    // Singleton pattern
    static ControllerAccount* get_singleton();
    Dictionary contracts = {};

    // Métodos principales de configuración
    void set_session_account(DOJO::ControllerAccount* account);
    DOJO::ControllerAccount* get_session_account() const;
    void add_dojo_contract(const String& name, const String& contract_address);
    void remove(const String& name);

    void setup_policies(const Dictionary& policies_data);
    // Métodos de conexión y autenticación
    void create(const Dictionary& policies_data);
    void disconnect_controller();
    bool is_controller_connected() const;

    // Información de la cuenta
    String get_username() const;
    String get_address() const;
    String get_chain_id() const;

    // Ejecución de transacciones
    void execute_raw(const Dictionary& action);
    void execute_from_outside(const Dictionary& action);

    // Métodos de utilidad
    Dictionary get_account_info() const;

    // Señales de estado
    void emit_connection_status(bool connected);
};

#endif // CONTROLLER_ACCOUNT_H
