#pragma once

#include "dojo/dojo.hpp"
#include "godot_cpp/classes/node.hpp"
#include "ref_counted/dojo_utilities/callback.hpp"
#include "ref_counted/dojo_utilities/clause.hpp"

// Forward declare query classes
class AchievementQuery;
class ActivityQuery;
class AggregationQuery;
class ContractQuery;
class ControllerQuery;
class DojoQuery;
class EventQuery;
class PlayerAchievementQuery;
class SearchQuery;
class TokenBalanceQuery;
class TokenContractQuery;
class TokenTransferQuery;
class TokenQuery;
class TransactionQuery;


using namespace godot;

class ToriiClient : public Node
{
    GDCLASS(ToriiClient, Node)
    std::shared_ptr<dojo::ToriiClient> client;

public:
    ToriiClient();
    ~ToriiClient();

    bool connect(const String& torii_url, int64_t max_message_size = -1);
    static ToriiClient* create(const String& torii_url, int64_t max_message_size = -1);

    // Queries
    Dictionary achievements(const Ref<AchievementQuery>& query) const;
    Dictionary activities(const Ref<ActivityQuery>& query) const;
    Dictionary aggregations(const Ref<AggregationQuery>& query) const;
    TypedArray<Dictionary> contracts(const Ref<ContractQuery>& query) const;
    Dictionary controllers(const Ref<ControllerQuery>& query) const;
    Dictionary entities(const Ref<DojoQuery>& query) const;
    Dictionary event_messages(const Ref<DojoQuery>& query) const;
    Dictionary player_achievements(const Ref<PlayerAchievementQuery>& query) const;
    Dictionary search(const Ref<SearchQuery>& query) const;
    Dictionary starknet_events(const Ref<EventQuery>& query) const;
    Dictionary token_balances(const Ref<TokenBalanceQuery>& query) const;
    Dictionary token_contracts(const Ref<TokenContractQuery>& query) const;
    Dictionary token_transfers(const Ref<TokenTransferQuery>& query) const;
    Dictionary tokens(const Ref<TokenQuery>& query) const;
    Dictionary transactions(const Ref<TransactionQuery>& query) const;

    void cancel_subscription(uint64_t subscription_id) const;

    String publish_message(const String& message, const PackedStringArray& signature, const String& world_address) const;
    PackedStringArray publish_message_batch(const Array& messages) const;

    TypedArray<Dictionary> sql(const String& query) const;

    // Subscriptions
    uint64_t subscribe_entity_updates(const Ref<DojoClause> &clause, const PackedStringArray &world_addresses, const Ref<DojoCallback> &callback) const;
    uint64_t subscribe_event_updates(const Array &keys, const Ref<DojoCallback> &callback) const;
    uint64_t subscribe_token_balance_updates(const PackedStringArray &contract_addresses, const PackedStringArray &account_addresses, const PackedStringArray &token_ids, const Ref<DojoCallback> &callback) const;
    uint64_t subscribe_token_updates(const PackedStringArray &contract_addresses, const PackedStringArray &token_ids, const Ref<DojoCallback> &callback) const;
    uint64_t subscribe_transaction_updates(const Dictionary &filter, const Ref<DojoCallback> &callback) const;

    TypedArray<Dictionary> worlds(const PackedStringArray& world_addresses) const;

    std::shared_ptr<dojo::ToriiClient> get_client() const;

    bool is_connected(bool verbose = false) const;

protected:
    static void _bind_methods();
};
