//
// Created by hazel on 27/05/25.
//

#ifndef TORII_CLIENT_H
#define TORII_CLIENT_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/callable.hpp"

#include "dojo_types.h"
#include "variant/field_element.h"

using namespace godot;

class ToriiClient : public Node
{
    GDCLASS(ToriiClient, Node)

    static ToriiClient* singleton;
    DOJO::ToriiClient* client;
    bool is_connected;
    Array event_subscriptions;

public:
    ToriiClient();
    ~ToriiClient();

    enum QueryOrderDirection
    {
        ASC,
        DESC,
    };

    enum QueryPaginationDirection
    {
        FORWARD,
        BACKWARD,
    };

    static ToriiClient* get_singleton();

    bool create_client();
    void disconnect_client(bool send_signal);
    bool is_client_connected() const;
    bool is_calable_valid();
    void callable_call(const char* msg);
    FieldElement get_world() const;
    void set_world(const FieldElement& n_world);

    Dictionary get_world_metadata();
    bool refresh_metadata();

    TypedArray<Dictionary> get_entities(const Dictionary& query_params = Dictionary());
    TypedArray<Dictionary> get_contollers_by_usernames(const TypedArray<String>& usernames);
    TypedArray<Dictionary> get_contollers_by_addresses(const TypedArray<String>& addresses);

    TypedArray<Dictionary> get_controllers(const TypedArray<String>& usernames, const TypedArray<String>& addresses);
    Dictionary get_controller_info(const String& controller_address);

    TypedArray<Dictionary> get_tokens(const Dictionary& query_params = Dictionary());
    TypedArray<Dictionary> get_token_balances(const String& account_address);
    TypedArray<Dictionary> get_token_collections();
    Dictionary get_token_info(const String& token_address);

    bool create_entity_subscription(const Callable& callback, const Dictionary& filter_params = Dictionary());
    bool create_event_subscription(const Callable& callback, const Dictionary& filter_params = Dictionary());
    bool create_token_subscription(const Callable& callback, const String& account_address);
    bool create_event_messages_subscription(const Callable& callback, const String& query);
    bool create_transactions_subscription(const Callable& callback, const Dictionary& filter_params = Dictionary());

    void cancel_all_subscriptions();

    bool publish_message(const String& message_data, const Array& signature_felts);
    bool publish_typed_message(const Dictionary& typed_data, const Array& signature_felts);


    Dictionary get_client_info() const;

    Dictionary get_connection_status() const;

    static DOJO::Query create_query_from_dict(const Dictionary& query_params) ;
    static DOJO::Pagination create_pagination_from_dict(const Dictionary& pagination_params) ;
    // void on_entity_update_internal(DOJO::FieldElement entity_id, DOJO::CArrayStruct models);
    // void on_event_update_internal(DOJO::Event event);

    DOJO::ToriiClient* get_client() const { return client; }

    Callable get_logger_callback() const { return logger_callback; }
    void set_logger_callback(const Callable& p_logger_callback);

    String get_torii_url() const { return torii_url; }
    void set_torii_url(const String& p_torii_url) { torii_url = p_torii_url; }

    void set_world_address(const String& p_world_address) { world_address = p_world_address; }
    String get_world_address() const { return world_address; }

    Array get_events(){return event_subscriptions;}
    void set_events(const Array& p_events){event_subscriptions = p_events;}

    protected:
    String torii_url;
    String world_address;
    Callable logger_callback;

    DOJO::FieldElement* world;

    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("create_client"),
                             &ToriiClient::create_client);
        ClassDB::bind_method(D_METHOD("disconnect_client", "send_signal"), &ToriiClient::disconnect_client);
        ClassDB::bind_method(D_METHOD("is_client_connected"), &ToriiClient::is_client_connected);

        ClassDB::bind_method(D_METHOD("get_world_metadata"), &ToriiClient::get_world_metadata);
        ClassDB::bind_method(D_METHOD("refresh_metadata"), &ToriiClient::refresh_metadata);

        ClassDB::bind_method(D_METHOD("get_entities", "query_params"),
                             &ToriiClient::get_entities);

        ClassDB::bind_method(D_METHOD("get_controllers", "usernames", "addresses"),
                             &ToriiClient::get_controllers, DEFVAL(Array()), DEFVAL(Array()));

        ClassDB::bind_method(D_METHOD("get_controllers_by_usernames", "usernames"),
                             &ToriiClient::get_contollers_by_usernames, DEFVAL(Array()));

        ClassDB::bind_method(D_METHOD("get_controllers_by_addresses", "addresses"),
                             &ToriiClient::get_contollers_by_addresses, DEFVAL(Array()));

        ClassDB::bind_method(D_METHOD("get_controller_info", "controller_address"),
                             &ToriiClient::get_controller_info);

        ClassDB::bind_method(D_METHOD("get_tokens", "query_params"),
                             &ToriiClient::get_tokens, DEFVAL(Dictionary()));
        ClassDB::bind_method(D_METHOD("get_token_balances", "account_address"),
                             &ToriiClient::get_token_balances);
        ClassDB::bind_method(D_METHOD("get_token_collections"),
                             &ToriiClient::get_token_collections);
        ClassDB::bind_method(D_METHOD("get_token_info", "token_address"),
                             &ToriiClient::get_token_info);

        ClassDB::bind_method(D_METHOD("create_entity_subscription", "callback", "filter_params"),
                             &ToriiClient::create_entity_subscription, DEFVAL(Dictionary()));
        ClassDB::bind_method(D_METHOD("create_event_subscription", "callback", "filter_params"),
                             &ToriiClient::create_event_subscription, DEFVAL(Dictionary()));
        ClassDB::bind_method(D_METHOD("create_token_subscription", "callback", "account_address"),
                             &ToriiClient::create_token_subscription);
        ClassDB::bind_method(D_METHOD("cancel_all_subscriptions"), &ToriiClient::cancel_all_subscriptions);

        ClassDB::bind_method(D_METHOD("publish_message", "message_data", "signature_felts"),
                             &ToriiClient::publish_message);
        ClassDB::bind_method(D_METHOD("publish_typed_message", "typed_data", "signature_felts"),
                             &ToriiClient::publish_typed_message);

        ClassDB::bind_method(D_METHOD("get_client_info"), &ToriiClient::get_client_info);

        ClassDB::bind_method(D_METHOD("get_connection_status"), &ToriiClient::get_connection_status);

        ADD_SIGNAL(MethodInfo("client_connected", PropertyInfo(Variant::BOOL, "success")));
        ADD_SIGNAL(MethodInfo("client_disconnected"));
        ADD_SIGNAL(MethodInfo("entity_updated", PropertyInfo(Variant::DICTIONARY, "entity_data")));
        ADD_SIGNAL(MethodInfo("event_received", PropertyInfo(Variant::DICTIONARY, "event_data")));
        ADD_SIGNAL(MethodInfo("subscription_error", PropertyInfo(Variant::STRING, "error_message")));
        ADD_SIGNAL(MethodInfo("subscription_created", PropertyInfo(Variant::STRING, "subscription_name")));
        ADD_SIGNAL(MethodInfo("metadata_updated", PropertyInfo(Variant::DICTIONARY, "metadata")));
        ADD_SIGNAL(MethodInfo("message_published", PropertyInfo(Variant::STRING, "message_hash")));
        ADD_SIGNAL(MethodInfo("transaction_confirmed", PropertyInfo(Variant::STRING, "transaction_hash")));
        ADD_SIGNAL(MethodInfo("token_balance_updated", PropertyInfo(Variant::DICTIONARY, "balance_data")));

        BIND_ENUM_CONSTANT(ASC);
        BIND_ENUM_CONSTANT(DESC);
        BIND_ENUM_CONSTANT(FORWARD);
        BIND_ENUM_CONSTANT(BACKWARD);

        ClassDB::bind_method(D_METHOD("get_torii_url"), &ToriiClient::get_torii_url);
        ClassDB::bind_method(D_METHOD("set_torii_url", "torii_url"), &ToriiClient::set_torii_url);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "torii_url"), "set_torii_url", "get_torii_url");

        ClassDB::bind_method(D_METHOD("set_world_address", "world_address"), &ToriiClient::set_world_address);
        ClassDB::bind_method(D_METHOD("get_world_address"), &ToriiClient::get_world_address);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "world_address"), "set_world_address", "get_world_address");

        ClassDB::bind_method(D_METHOD("set_logger_callback", "logger_callback"), &ToriiClient::set_logger_callback);
        ClassDB::bind_method(D_METHOD("get_logger_callback"), &ToriiClient::get_logger_callback);
        ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "logger_callback"), "set_logger_callback", "get_logger_callback");

        ClassDB::bind_method(D_METHOD("get_events"), &ToriiClient::get_events);
        ClassDB::bind_method(D_METHOD("set_events", "events"), &ToriiClient::set_events);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "events"), "set_events", "get_events");

    }
};

VARIANT_ENUM_CAST(ToriiClient::QueryOrderDirection);

VARIANT_ENUM_CAST(ToriiClient::QueryPaginationDirection);

#endif // TORII_CLIENT_H
