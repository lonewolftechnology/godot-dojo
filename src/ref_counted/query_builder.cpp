#include "ref_counted/query_builder.hpp"

#include "ref_counted/queries/achievement.hpp"
#include "ref_counted/queries/activity.hpp"
#include "ref_counted/queries/aggregation.hpp"
#include "ref_counted/queries/contract.hpp"
#include "ref_counted/queries/controller.hpp"
#include "ref_counted/queries/entity.hpp"
#include "ref_counted/queries/event.hpp"
#include "ref_counted/queries/player_achievement.hpp"
#include "ref_counted/queries/search.hpp"
#include "ref_counted/queries/token.hpp"
#include "ref_counted/queries/token_balance.hpp"
#include "ref_counted/queries/token_contract.hpp"
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
            return memnew(DojoQuery);
        }
        case Event: {
            return memnew(EventQuery);
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
        case TokenContract: {
            return memnew(TokenContractQuery);
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

Ref<QueryBuilder> QueryBuilder::pagination(const uint32_t& limit, const String& cursor, const int64_t& direction) {
    this->p_pagination.limit = limit;
    this->p_pagination.cursor = cursor;
    this->p_pagination.direction = static_cast<PaginationDirection>(direction);
    return this;
}


Ref<QueryBuilder> QueryBuilder::order_by(const String& field, const int64_t& direction) {
    OrderBy ob;
    ob.field = field;
    ob.direction = static_cast<OrderDirection>(direction);
    p_order_by.push_back(ob);
    return this;
}

Dictionary QueryBuilder::to_dict() const {
    Dictionary result;
    result["type"] = static_cast<int>(type);
    
    Dictionary pagination;
    pagination["limit"] = p_pagination.limit;
    pagination["cursor"] = p_pagination.cursor;
    pagination["direction"] = static_cast<int>(p_pagination.direction);
    result["pagination"] = pagination;

    Array order_by_list;
    for (const auto& ob : p_order_by) {
        Dictionary order_by_dict;
        order_by_dict["field"] = ob.field;
        order_by_dict["direction"] = static_cast<int>(ob.direction);
        order_by_list.push_back(order_by_dict);
    }
    result["order_by"] = order_by_list;

    return result;
}

#ifndef WEB_ENABLED
std::shared_ptr<dojo::Pagination> QueryBuilder::get_native_pagination() const {
    auto pagination = std::make_shared<dojo::Pagination>();
    pagination->limit = get_limit();
    p_pagination.cursor_utf8 = get_cursor().utf8();
    pagination->cursor = p_pagination.cursor_utf8.get_data();
    pagination->direction = get_direction();
    return pagination;
}


std::vector<std::shared_ptr<dojo::OrderBy>> QueryBuilder::get_order_by() const {
    std::vector<std::shared_ptr<dojo::OrderBy>> result;
    for (const auto& ob : p_order_by) {
        auto native_ob = std::make_shared<dojo::OrderBy>();
        native_ob->field = ob.field.utf8().get_data();
        native_ob->direction = static_cast<dojo::OrderDirection>(ob.direction);
        result.push_back(native_ob);
    }
    return result;
}

#endif