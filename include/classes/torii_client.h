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
#include "resources/queries/dojo_query.h"
#include "resources/queries/dojo_token_query.h"
#include "resources/queries/dojo_token_balance_query.h"
#include "resources/queries/dojo_controller_query.h"
#include "variant/field_element.h"
#include "resources/subscriptions/entity.h"
#include "resources/subscriptions/message.h"
#include "resources/subscriptions/starknet.h"
#include "resources/subscriptions/transaction.h"
#include "resources/subscriptions/token.h"
#include "resources/subscriptions/indexer.h"
#include "resources/subscriptions/token_balance.h"


using namespace godot;

class ToriiClient : public Node
{
    GDCLASS(ToriiClient, Node)

    static ToriiClient* singleton;
    DOJO::ToriiClient* client;
    bool is_connected;
    Array event_subscriptions;

public:
    Callable on_entity_state_update_callback;
    Callable on_event_message_update_callback;
    Callable on_starknet_event_callback;
    Callable on_transaction_callback;
    Callable on_token_update_callback;
    Callable on_indexer_update_callback;
    Callable on_token_balance_update_callback;

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
    bool is_callable_valid() const;
    void callable_call(const char* msg) const;
    FieldElement get_world() const;
    void set_world(const FieldElement& n_world);

    Dictionary get_world_metadata() const;
    bool refresh_metadata();

    TypedArray<Dictionary> get_entities(const Ref<DojoQuery>& query) const;

    TypedArray<Dictionary> get_controllers(Ref<DojoControllerQuery> query);
    Dictionary get_controller_info(const String& controller_address);

    TypedArray<Dictionary> get_tokens(const Ref<DojoTokenQuery>& query) const;
    TypedArray<Dictionary> get_token_balances(const Ref<DojoTokenBalanceQuery>& query) const;
    TypedArray<Dictionary> get_token_collections(const Ref<DojoTokenBalanceQuery>& query) const;
    Dictionary get_token_info(const String& token_address) const;

    void on_entity_state_update(const Callable& callback, const Ref<EntitySubscription>& subscription);
    void on_event_message_update(const Callable& callback, const Ref<MessageSubscription>& subscription);
    void on_starknet_event(const Callable& callback, const Ref<StarknetSubscription>& subscription);
    void on_transaction(const Callable& callback, const Ref<TransactionSubscription>& subscription);
    void on_token_update(const Callable& callback, const Ref<TokenSubscription>& subscription);
    void on_indexer_update(const Callable& callback, const Ref<IndexerSubscription>& subscription);
    void on_token_balance_update(const Callable& callback, const Ref<TokenBalanceSubscription>& subscription);

    void cancel_all_subscriptions();

    bool publish_message(const String& message_data, const Array& signature_felts);
    bool publish_typed_message(const Dictionary& typed_data, const Array& signature_felts);


    Dictionary get_client_info() const;

    Dictionary get_connection_status() const;

    static DOJO::Query create_query_from_dict(const Dictionary& query_params);
    static DOJO::Pagination create_pagination_from_dict(const Dictionary& pagination_params);

    DOJO::ToriiClient* get_client() const { return client; }

    Callable get_logger_callback() const { return logger_callback; }
    void set_logger_callback(const Callable& p_logger_callback);

    String get_torii_url() const { return torii_url; }
    void set_torii_url(const String& p_torii_url) { torii_url = p_torii_url; }

    void set_world_address(const String& p_world_address) { world_address = p_world_address; }
    String get_world_address() const { return world_address; }

    Array get_events() const {return event_subscriptions;}
    void set_events(const Array& p_events){event_subscriptions = p_events;}

    protected:
    String torii_url;
    String world_address;
    Callable logger_callback;

    DOJO::FieldElement* world;

    static void _bind_methods();
};

VARIANT_ENUM_CAST(ToriiClient::QueryOrderDirection);

VARIANT_ENUM_CAST(ToriiClient::QueryPaginationDirection);

#endif // TORII_CLIENT_H
