#include "ref_counted/query_builder.hpp"

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
#include "ref_counted/queries/token_transfer.hpp"
#include "ref_counted/queries/transaction.hpp"

QueryBuilder::QueryBuilder(const Type &p_type) : type(p_type) {
}

QueryBuilder::~QueryBuilder() {
}

Ref<QueryBuilder> QueryBuilder::create(const int64_t &p_type) {
    switch (p_type) {
        case Achievement: {
            return memnew(AchievementQuery);
        }
        case Activity: {
            return memnew(ActivityQuery);
        }
        case Aggregation: {
            return memnew(AggregationQuery);
        }
        case Contract: {
            return memnew(ContractQuery);
        }
        case Controller: {
            return memnew(ControllerQuery);
        }
        case Entity: {
            return memnew(EntityQuery);
        }
        case PlayerAchievement: {
            return memnew(PlayerAchievementQuery);
        }
        case Search: {
            return memnew(SearchQuery);
        }
        case Token: {
            return memnew(TokenQuery);
        }
        case TokenBalance: {
            return memnew(TokenBalanceQuery);
        }
        case TokenTransfer: {
            return memnew(TokenTransferQuery);
        }
        case Transaction: {
            return memnew(TransactionQuery);
        }
        default:
            return memnew(QueryBuilder(static_cast<Type>(p_type)));
    }
}

Ref<QueryBuilder> QueryBuilder::set_type(const int64_t &_type) {
    this->type = static_cast<Type>(_type);
    notify_property_list_changed();
    return this;
}

int64_t QueryBuilder::get_type() const {
    return this->type;
}