//
// Created by hazel on 9/06/25.
//

#ifndef CONTROLLER_ACCOUNT_H
#define CONTROLLER_ACCOUNT_H

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <dojo_types.h>
#include <variant/field_element.h>

using namespace godot;

class ControllerAccount : public Object
{
    GDCLASS(ControllerAccount, Object)

private:
    dojo_bindings::ControllerAccount* controller_account;
    dojo_bindings::Provider* controller_provider;
    bool is_initialized;

protected:
    static void _bind_methods();
    static ControllerAccount* singleton;

public:
    ControllerAccount();
    ~ControllerAccount();
    void connect(const String& controller_addr, const String& rpc_url);

    static ControllerAccount* get_singleton();

    void set_session_account(dojo_bindings::ControllerAccount* account) { controller_account = account; }
    dojo_bindings::ControllerAccount* get_session_account() { return controller_account; }
    void set_initialized(bool initialized) { is_initialized = initialized; }
    // Métodos de inicialización
    bool is_valid() const;

    // Métodos principales del wrapper
    String get_username() const;
    String get_address() const;
    PackedByteArray get_address_as_bytes() const;
    String get_chain_id() const;

    // Métodos para ejecutar transacciones
    bool execute_transaction(const String& selector);
    bool execute_transaction_with_calldata(const String& selector, const Array& calldata);
    bool execute_multiple_calls(const TypedArray<Dictionary>& calls);

    // Métodos de utilidad
    void disconnect_account();
    Dictionary get_account_info() const;
};

#endif // CONTROLLER_ACCOUNT_H
