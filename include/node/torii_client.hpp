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

    static ToriiClient* create(const String& torii_url);
    static ToriiClient* create_with_config(const String& torii_url, uint64_t max_message_size);

    // Queries
    Dictionary achievements(const Ref<AchievementQuery>& query);
    Dictionary activities(const Ref<ActivityQuery>& query);
    Dictionary aggregations(const Ref<AggregationQuery>& query);
    TypedArray<Dictionary> contracts(const Ref<ContractQuery>& query);
    Dictionary controllers(const Ref<ControllerQuery>& query);
    Dictionary entities(const Ref<DojoQuery>& query);
    Dictionary event_messages(const Ref<DojoQuery>& query);
    Dictionary player_achievements(const Ref<PlayerAchievementQuery>& query);
    Dictionary search(const Ref<SearchQuery>& query);
    Dictionary starknet_events(const Ref<EventQuery>& query);
    Dictionary token_balances(const Ref<TokenBalanceQuery>& query);
    Dictionary token_contracts(const Ref<TokenContractQuery>& query);
    Dictionary token_transfers(const Ref<TokenTransferQuery>& query);
    Dictionary tokens(const Ref<TokenQuery>& query);
    Dictionary transactions(const Ref<TransactionQuery>& query);

    void cancel_subscription(uint64_t subscription_id);

    String publish_message(const String& message, const PackedStringArray& signature, const String& world_address);
    PackedStringArray publish_message_batch(const Array& messages);

    TypedArray<Dictionary> sql(const String& query);

    uint64_t subscribe_entity_updates(const Ref<DojoClause> &clause, const PackedStringArray &world_addresses, const Ref<DojoCallback> &callback);
    uint64_t subscribe_event_updates(const Array &keys, const Ref<DojoCallback> &callback);
    uint64_t subscribe_token_balance_updates(const PackedStringArray &contract_addresses, const PackedStringArray &account_addresses, const PackedStringArray &token_ids, const Ref<DojoCallback> &callback);
    uint64_t subscribe_token_updates(const PackedStringArray &contract_addresses, const PackedStringArray &token_ids, const Ref<DojoCallback> &callback);
    uint64_t subscribe_transaction_updates(const Dictionary &filter, const Ref<DojoCallback> &callback);

    TypedArray<Dictionary> worlds(const PackedStringArray& world_addresses);

    std::shared_ptr<dojo::ToriiClient> get_client() const;

protected:
    static void _bind_methods();
};
