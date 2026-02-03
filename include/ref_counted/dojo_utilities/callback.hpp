#pragma once

#include "dojo/dojo.hpp"
#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;

class DojoCallback : public RefCounted
{
    GDCLASS(DojoCallback, RefCounted)

private:
    Callable on_update;
    Callable on_error;

public:
    void set_on_update(const Callable &p_on_update);
    Callable get_on_update() const;

    void set_on_error(const Callable &p_on_error);
    Callable get_on_error() const;

    std::shared_ptr<dojo::EntityUpdateCallback> create_entity_callback();
    std::shared_ptr<dojo::EventUpdateCallback> create_event_callback();
    std::shared_ptr<dojo::TokenBalanceUpdateCallback> create_token_balance_callback();
    std::shared_ptr<dojo::TokenUpdateCallback> create_token_callback();
    std::shared_ptr<dojo::TransactionUpdateCallback> create_transaction_callback();

    // Adaptador genérico (Template)
    template <typename TInterface, typename TData, typename F>
    class CallbackAdapter : public TInterface {
        Callable update_cb;
        Callable error_cb;
        F converter;

    public:
        CallbackAdapter(Callable u, Callable e, F c) : update_cb(u), error_cb(e), converter(c) {}

        void on_update(const TData &data) override {
            if (update_cb.is_valid()) {
                update_cb.call_deferred(converter(data));
            }
        }

        void on_error(const std::string &error) override {
            if (error_cb.is_valid()) {
                error_cb.call_deferred(String(error.c_str()));
            }
        }
    };

    // Helper para instanciar el adaptador
    template <typename TInterface, typename TData, typename F>
    std::shared_ptr<TInterface> get_callback(F converter) {
        return std::make_shared<CallbackAdapter<TInterface, TData, F>>(on_update, on_error, converter);
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_on_update", "callback"), &DojoCallback::set_on_update);
        ClassDB::bind_method(D_METHOD("get_on_update"), &DojoCallback::get_on_update);
        ClassDB::bind_method(D_METHOD("set_on_error", "callback"), &DojoCallback::set_on_error);
        ClassDB::bind_method(D_METHOD("get_on_error"), &DojoCallback::get_on_error);

        ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "on_update"), "set_on_update", "get_on_update");
        ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "on_error"), "set_on_error", "get_on_error");
    }
};