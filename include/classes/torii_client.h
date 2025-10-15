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
#include "resources/queries/dojo_activity_query.h"
#include "resources/queries/dojo_aggregation_query.h"
#include "resources/queries/dojo_contract_query.h"
#include "resources/queries/dojo_query.h"
#include "resources/queries/dojo_token_query.h"
#include "resources/queries/dojo_token_balance_query.h"
#include "resources/queries/dojo_controller_query.h"
#include "resources/queries/dojo_token_transfer_query.h"
#include "resources/queries/dojo_achievement_query.h"
#include "resources/queries/dojo_player_achievement_query.h"

#include "resources/subscriptions/activity.h"
#include "resources/subscriptions/aggregation.h"
#include "resources/subscriptions/entity.h"
#include "resources/subscriptions/message.h"
#include "resources/subscriptions/starknet.h"
#include "resources/subscriptions/transaction.h"
#include "resources/subscriptions/token.h"
#include "resources/subscriptions/contract.h"
#include "resources/subscriptions/token_balance.h"
#include "resources/subscriptions/token_transfer.h"
#include "resources/subscriptions/achievement_progression.h"

#ifdef WEB_ENABLED
#include "godot_cpp/classes/os.hpp"
#include "tools/dojo_bridge.h"
#include "godot_cpp/classes/java_script_bridge.hpp"
#include "godot_cpp/classes/java_script_object.hpp"
#endif


using namespace godot;

class ToriiClient : public Node {
    GDCLASS(ToriiClient, Node)
    static ToriiClient *singleton;
    DOJO::ToriiClient *client;
    bool is_connected;
    Array events;
    Array models;
    Array subscriptions;

public:
    Callable on_entity_state_update_callback;
    Callable on_event_message_update_callback;
    Callable on_starknet_event_callback;
    Callable on_transaction_callback;
    Callable on_token_update_callback;
    Callable on_contract_update_callback;
    Callable on_token_balance_update_callback;
    Callable on_token_transfer_update_callback;
    Callable on_aggregation_update_callback;
    Callable on_activity_update_callback;
    Callable on_update_achievement_update_callback;
    Callable on_achievement_progression_update_callback;

    ToriiClient();

    ~ToriiClient();

    static ToriiClient *get_singleton();

    bool create_client(const String &p_url, const TypedArray<String> &p_addresses);

    void disconnect_client(bool send_signal);

    bool is_client_connected() const;

    bool is_callable_valid() const;

    void callable_call(const char *msg) const;

    TypedArray<Dictionary> get_worlds_metadata(const TypedArray<String>& world_addresses = Array());

    bool refresh_metadata(const TypedArray<String>& p_world_addresses = Array());

    TypedArray<Dictionary> get_entities(const Ref<DojoQuery> &query);

    TypedArray<Dictionary> get_controllers(Ref<DojoControllerQuery> query);

    Dictionary get_controller_info(const String &controller_address);

    TypedArray<Dictionary> get_tokens(const Ref<DojoTokenQuery> &query) const;

    TypedArray<Dictionary> get_token_balances(const Ref<DojoTokenBalanceQuery> &query) const;

    TypedArray<Dictionary> get_token_transfers(const Ref<DojoTokenTransferQuery> &query) const;

    TypedArray<Dictionary> get_token_collections(const Ref<DojoContractQuery> &query) const;

    TypedArray<Dictionary> get_token_info(const Ref<DojoTokenQuery> &query) const;

    TypedArray<Dictionary> get_aggregations(const Ref<DojoAggregationQuery> &query) const;

    TypedArray<Dictionary> get_activities(const Ref<DojoActivityQuery> &query) const;

    TypedArray<Dictionary> get_achivements(const Ref<DojoAchievementQuery> &query) const;

    TypedArray<Dictionary> get_player_achivements(const Ref<DojoPlayerAchievementQuery> &query) const;

    // Subscriptions
    void on_entity_state_update(const Callable &callback, const Ref<EntitySubscription> &subscription);

    void on_event_message_update(const Callable &callback, const Ref<MessageSubscription> &subscription);

    void on_starknet_event(const Callable &callback, const Ref<StarknetSubscription> &subscription);

    void on_transaction(const Callable &callback, const Ref<TransactionSubscription> &subscription);

    void on_token_update(const Callable &callback, const Ref<TokenSubscription> &subscription);

    void on_contract_update(const Callable &callback, const Ref<ContractSubscription> &subscription);

    void on_token_balance_update(const Callable &callback, const Ref<TokenBalanceSubscription> &subscription);

    void on_token_transfer_update(const Callable &callback, const Ref<TokenTransferSubscription> &subscription);

    void on_aggregation_update(const Callable &callback, const Ref<AggregationSubscription> &subscription);

    void on_activity_update(const Callable &callback, const Ref<ActivitySubscription> &subscription);

    void on_achievement_progression_update(const Callable &callback,
                                           const Ref<AchievementProgressionSubscription> &subscription);


    // Subscriptions update
    void update_subscription(const Ref<DojoSubscription> &subscription, const Callable &callback = Callable());

    void update_entity_subscription(const Ref<EntitySubscription> &subscription, const Callable &callback = Callable());

    void update_event_message_subscription(const Ref<MessageSubscription> &subscription,
                                           const Callable &callback = Callable());

    void update_starknet_event_subscription(const Ref<StarknetSubscription> &subscription,
                                            const Callable &callback = Callable());

    void update_transaction_subscription(const Ref<TransactionSubscription> &subscription,
                                         const Callable &callback = Callable());

    void update_token_subscription(const Ref<TokenSubscription> &subscription, const Callable &callback = Callable());

    void update_contract_subscription(const Ref<ContractSubscription> &subscription,
                                      const Callable &callback = Callable());

    void update_token_balance_subscription(const Ref<TokenBalanceSubscription> &subscription,
                                           const Callable &callback = Callable());

    void update_token_transfer_subscription(const Ref<TokenTransferSubscription> &subscription,
                                            const Callable &callback = Callable());

    void update_aggregation_subscription(const Ref<AggregationSubscription> &subscription,
                                         const Callable &callback = Callable());

    void update_activity_subscription(const Ref<ActivitySubscription> &subscription,
                                      const Callable &callback = Callable());

    void update_achievement_progression(const Ref<AchievementProgressionSubscription> &subscription,
                                        const Callable &callback = Callable());

    void cancel_all_subscriptions();

    bool publish_message(const String &message_data, const Array &signature_felts);

    bool publish_typed_message(const Dictionary &typed_data, const Array &signature_felts);


    Dictionary get_client_info() const;

    Dictionary get_connection_status() const;

    DOJO::ToriiClient *get_client() const { return client; }

    Callable get_logger_callback() const { return logger_callback; }

    void set_logger_callback(const Callable &p_logger_callback);

    String get_torii_url() const { return torii_url; }
    void set_torii_url(const String &p_torii_url) { torii_url = p_torii_url; }

    void set_world_addresses(const TypedArray<String> &p_world_address) { world_addresses = p_world_address; }
    TypedArray<String> get_world_addresses() const { return world_addresses; }

    TypedArray<String> get_worlds();
    String get_url();

    Array get_events() const { return events; }
    void set_events(const Array &p_events) { events = p_events; }

    Array get_models() const { return models; }
    void set_models(const Array &p_models) { models = p_models; }

private:
    // Private helpers
    bool _is_ready_for_query(const Ref<Resource> &query) const;

    bool _is_ready_for_subscription(const Ref<DojoSubscription> &subscription) const;


#ifdef WEB_ENABLED
    void _on_client_created(const Variant &result);
    void _on_get_entities_completed(const Variant &result);
    void _on_get_world_metadata_completed(const Variant &result);
    void _on_entity_state_update_emitted(const Variant &entity_data);
    void _on_event_message_update_emitted(const Variant &message_data);

#endif

protected:
    String torii_url;
    TypedArray<String> world_addresses;
    Callable logger_callback;

    // Web specific properties
#ifdef WEB_ENABLED
    bool is_web_client_initialized = false;
    // Ref<JavaScriptObject> web_client;
#endif


    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("create_client"), &ToriiClient::create_client);
        ClassDB::bind_method(D_METHOD("disconnect_client", "send_signal"), &ToriiClient::disconnect_client);
        ClassDB::bind_method(D_METHOD("is_client_connected"), &ToriiClient::is_client_connected);

        ClassDB::bind_method(D_METHOD("get_worlds_metadata", "world_addresses"), &ToriiClient::get_worlds_metadata, DEFVAL(Array()));
        ClassDB::bind_method(D_METHOD("refresh_metadata", "p_world_addresses"), &ToriiClient::refresh_metadata, DEFVAL(Array()));

        ClassDB::bind_method(D_METHOD("get_entities", "query"), &ToriiClient::get_entities);
        ClassDB::bind_method(D_METHOD("get_controllers", "query"), &ToriiClient::get_controllers);
        ClassDB::bind_method(D_METHOD("get_controller_info", "controller_address"), &ToriiClient::get_controller_info);

        ClassDB::bind_method(D_METHOD("get_tokens", "query"), &ToriiClient::get_tokens);
        ClassDB::bind_method(D_METHOD("get_token_balances", "query"), &ToriiClient::get_token_balances);
        ClassDB::bind_method(D_METHOD("get_token_collections", "query"), &ToriiClient::get_token_collections);
        ClassDB::bind_method(D_METHOD("get_activities", "query"), &ToriiClient::get_activities);
        ClassDB::bind_method(D_METHOD("get_aggregations", "query"), &ToriiClient::get_aggregations);
        ClassDB::bind_method(D_METHOD("get_token_info", "token_address"), &ToriiClient::get_token_info);
        ClassDB::bind_method(D_METHOD("get_achivements", "query"), &ToriiClient::get_achivements);
        ClassDB::bind_method(D_METHOD("get_player_achivements", "query"), &ToriiClient::get_player_achivements);

        //Subscription
        ClassDB::bind_method(D_METHOD("on_entity_state_update", "callback", "subscription"),
                             &ToriiClient::on_entity_state_update);

        ClassDB::bind_method(D_METHOD("on_event_message_update", "callback", "subscription"),
                             &ToriiClient::on_event_message_update);

        ClassDB::bind_method(D_METHOD("on_starknet_event", "callback", "subscription"),
                             &ToriiClient::on_starknet_event);

        ClassDB::bind_method(D_METHOD("on_transaction", "callback", "subscription"),
                             &ToriiClient::on_transaction);

        ClassDB::bind_method(D_METHOD("on_token_update", "callback", "subscription"),
                             &ToriiClient::on_token_update);

        ClassDB::bind_method(D_METHOD("on_contract_update", "callback", "subscription"),
                             &ToriiClient::on_contract_update);

        ClassDB::bind_method(D_METHOD("on_token_balance_update", "callback", "subscription"),
                             &ToriiClient::on_token_balance_update);

        ClassDB::bind_method(D_METHOD("on_aggregation_update", "callback", "subscription"),
                             &ToriiClient::on_aggregation_update);
        ClassDB::bind_method(D_METHOD("on_activity_update", "callback", "subscription"),
                             &ToriiClient::on_activity_update);

        ClassDB::bind_method(D_METHOD("on_achievement_progression_update", "callback", "subscription"),
                             &ToriiClient::on_achievement_progression_update);

        // Subscription Update
        ClassDB::bind_method(D_METHOD("update_subscription", "subscription", "callback"),
                             &ToriiClient::update_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_entity_subscription", "subscription", "callback"),
                             &ToriiClient::update_entity_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_event_message_subscription", "subscription", "callback"),
                             &ToriiClient::update_event_message_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_starknet_event_subscription", "subscription", "callback"),
                             &ToriiClient::update_starknet_event_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_transaction_subscription", "subscription", "callback"),
                             &ToriiClient::update_transaction_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_token_subscription", "subscription", "callback"),
                             &ToriiClient::update_token_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_contract_subscription", "subscription", "callback"),
                             &ToriiClient::update_contract_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_token_balance_subscription", "subscription", "callback"),
                             &ToriiClient::update_token_balance_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_aggregation_subscription", "subscription", "callback"),
                             &ToriiClient::update_aggregation_subscription, DEFVAL(Callable()));

        ClassDB::bind_method(D_METHOD("update_activity_subscription", "subscription", "callback"),
                             &ToriiClient::update_activity_subscription, DEFVAL(Callable()));

        //
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
        ADD_SIGNAL(MethodInfo("token_balance_update", PropertyInfo(Variant::DICTIONARY, "balance_data")));
        ADD_SIGNAL(MethodInfo("token_transfer_update", PropertyInfo(Variant::DICTIONARY, "transfer_data")));
        ADD_SIGNAL(MethodInfo("token_update", PropertyInfo(Variant::DICTIONARY, "token_data")));
        ADD_SIGNAL(MethodInfo("contract_update", PropertyInfo(Variant::DICTIONARY, "contract_data")));

        // Signals for async web calls
        ADD_SIGNAL(
            MethodInfo("entities_received", PropertyInfo(Variant::ARRAY, "entities", PROPERTY_HINT_ARRAY_TYPE,
                "Dictionary")));

        ClassDB::bind_method(D_METHOD("get_torii_url"), &ToriiClient::get_torii_url);
        ClassDB::bind_method(D_METHOD("set_torii_url", "torii_url"), &ToriiClient::set_torii_url);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "torii_url"), "set_torii_url", "get_torii_url");

        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_addresses"), &ToriiClient::set_world_addresses);
        ClassDB::bind_method(D_METHOD("get_world_addresses"), &ToriiClient::get_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "world_addresses"), "set_world_addresses", "get_world_addresses");

        ClassDB::bind_method(D_METHOD("set_logger_callback", "logger_callback"), &ToriiClient::set_logger_callback);
        ClassDB::bind_method(D_METHOD("get_logger_callback"), &ToriiClient::get_logger_callback);
        ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "logger_callback"), "set_logger_callback", "get_logger_callback");

        ClassDB::bind_method(D_METHOD("get_events"), &ToriiClient::get_events);
        ClassDB::bind_method(D_METHOD("set_events", "events"), &ToriiClient::set_events);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "events"), "set_events", "get_events");

        ClassDB::bind_method(D_METHOD("get_models"), &ToriiClient::get_models);
        ClassDB::bind_method(D_METHOD("set_models", "models"), &ToriiClient::set_models);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "models"), "set_models", "get_models");
#ifdef WEB_ENABLED
        // Private callbacks
        ClassDB::bind_method(D_METHOD("_on_get_entities_completed", "result"),
                             &ToriiClient::_on_get_entities_completed);
        ClassDB::bind_method(D_METHOD("_on_get_world_metadata_completed", "result"),
                             &ToriiClient::_on_get_world_metadata_completed);
        ClassDB::bind_method(D_METHOD("_on_entity_state_update_emitted", "entity_data"),
                             &ToriiClient::_on_entity_state_update_emitted);
        ClassDB::bind_method(D_METHOD("_on_client_created", "result"), &ToriiClient::_on_client_created);
#endif
    }
};

#endif // TORII_CLIENT_H
