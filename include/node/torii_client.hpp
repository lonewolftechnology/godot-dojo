#pragma once
#include "dojo/dojo.hpp"
#include "godot_cpp/classes/node.hpp"
#include "ref_counted/queries/achievement.hpp"
#include "ref_counted/queries/activity.hpp"
#include "ref_counted/queries/aggregation.hpp"
#include "ref_counted/queries/contract.hpp"
#include "ref_counted/queries/controller.hpp"
#include "ref_counted/queries/entity.hpp"
#include "ref_counted/queries/player_achievement.hpp"
#include "ref_counted/queries/search.hpp"
#include "ref_counted/queries/token.hpp"
#include "ref_counted/queries/token_balance.hpp"
#include "ref_counted/queries/token_contract.hpp"
#include "ref_counted/queries/token_transfer.hpp"
#include "ref_counted/queries/transaction.hpp"

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
Dictionary achievements(const AchievementQuery &query);
Dictionary activities(const ActivityQuery &query);
Dictionary aggregations(const AggregationQuery &query);
void cancel_subscription(uint64_t subscription_id);
TypedArray<Dictionary> contracts(const ContractQuery &query);
Dictionary controllers(const ControllerQuery &query);
Dictionary entities(const EntityQuery &query);
Dictionary event_messages(const EntityQuery &query);
Dictionary player_achievements(const PlayerAchievementQuery &query);
String publish_message(const String &message, const PackedStringArray& signature, const String& world_address);
PackedStringArray publish_message_batch(const Array &messages); // TODO: create Message wrapper
Dictionary search(const SearchQuery &query);
TypedArray<Dictionary> sql(const String &query);
// Dictionary starknet_events(const EventQuery &query);
    // TODO: Callback wrapper
// uint64_t subscribe_entity_updates(Ref<DojoClause> clause, const PackedStringArray &world_addresses, const std::shared_ptr<EntityUpdateCallback> &callback);
// uint64_t subscribe_event_updates(const std::vector<std::shared_ptr<KeysClause>> &keys, const std::shared_ptr<EventUpdateCallback> &callback);
// uint64_t subscribe_token_balance_updates(const PackedStringArray &contract_addresses, const PackedStringArray &account_addresses, const std::vector<U256> &token_ids, const std::shared_ptr<TokenBalanceUpdateCallback> &callback);
// uint64_t subscribe_token_updates(const PackedStringArray &contract_addresses, const std::vector<U256> &token_ids, const std::shared_ptr<TokenUpdateCallback> &callback);
// uint64_t subscribe_transaction_updates(std::optional<std::shared_ptr<TransactionFilter>> filter, const std::shared_ptr<TransactionUpdateCallback> &callback);
Dictionary token_balances(const TokenBalanceQuery &query);
Dictionary token_contracts(const TokenContractQuery &query);
Dictionary token_transfers(const TokenTransferQuery &query);
Dictionary tokens(const TokenQuery &query);
Dictionary transactions(const TransactionQuery &query);
Dictionary worlds(const PackedStringArray &world_addresses);
    std::shared_ptr<dojo::ToriiClient> get_client() const;

protected:
    static void _bind_methods()
    {
        ClassDB::bind_static_method(get_class_static(), D_METHOD("create", "torii_url"), &ToriiClient::create);
    }
};