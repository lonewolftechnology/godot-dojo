#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "dojo/dojo.hpp"

using namespace godot;

class QueryBuilder : public RefCounted {
    GDCLASS(QueryBuilder, RefCounted)

public:
    enum Type {
        None,
        Achievement,
        Activity,
        Aggregation,
        Contract,
        Controller,
        Entity,
        Event,
        PlayerAchievement,
        Search,
        Token,
        TokenBalance,
        TokenContract,
        TokenTransfer,
        Transaction
    };

private:
    // Base
    Type type = None;

    // Pagination
    struct Pagination {
        uint32_t limit = 0;
        String cursor = "";
        dojo::PaginationDirection direction = dojo::PaginationDirection::kForward;
    };
    Pagination p_pagination;

    // OrderBy
    struct OrderBy {
        String field;
        dojo::OrderDirection direction;
    };
    std::vector<OrderBy> p_order_by;

public:
    QueryBuilder() = default;

    QueryBuilder(const Type &p_type);

    ~QueryBuilder();

    static Ref<QueryBuilder> create(const int64_t &p_type);

    // Base
    Ref<QueryBuilder> set_type(const int64_t &p_type);
    int64_t get_type() const;

    // Pagination
    Ref<QueryBuilder> pagination(const uint32_t& limit, const String& cursor, const int64_t& direction);

    // OrderBy
    Ref<QueryBuilder> order_by(const String& field, const int64_t& direction);

    uint32_t get_limit() const { return p_pagination.limit; }
    String get_cursor() const { return p_pagination.cursor; }
    dojo::PaginationDirection get_direction() const { return p_pagination.direction; }

    std::vector<std::shared_ptr<dojo::OrderBy>> get_order_by() const;

protected:
    static void _bind_methods() {
        // Type
        BIND_ENUM_CONSTANT(None);
        BIND_ENUM_CONSTANT(Achievement);
        BIND_ENUM_CONSTANT(Activity);
        BIND_ENUM_CONSTANT(Aggregation);
        BIND_ENUM_CONSTANT(Contract);
        BIND_ENUM_CONSTANT(Controller);
        BIND_ENUM_CONSTANT(Entity);
        BIND_ENUM_CONSTANT(Event);
        BIND_ENUM_CONSTANT(PlayerAchievement);
        BIND_ENUM_CONSTANT(Search);
        BIND_ENUM_CONSTANT(Token);
        BIND_ENUM_CONSTANT(TokenBalance);
        BIND_ENUM_CONSTANT(TokenContract);
        BIND_ENUM_CONSTANT(TokenTransfer);
        BIND_ENUM_CONSTANT(Transaction);

        // OrderDirection
        ClassDB::bind_integer_constant(get_class_static(), "OrderDirection", "Asc", static_cast<int>(dojo::OrderDirection::kAsc));
        ClassDB::bind_integer_constant(get_class_static(), "OrderDirection", "Desc", static_cast<int>(dojo::OrderDirection::kDesc));

        // PaginationDirection
        ClassDB::bind_integer_constant(get_class_static(), "PaginationDirection", "Forward", static_cast<int>(dojo::PaginationDirection::kForward));
        ClassDB::bind_integer_constant(get_class_static(), "PaginationDirection", "Backward", static_cast<int>(dojo::PaginationDirection::kBackward));

        // Static Methods
        ClassDB::bind_static_method("QueryBuilder", D_METHOD("create", "type"), &QueryBuilder::create);

        // Instance Methods
        ClassDB::bind_method(D_METHOD("set_type", "type"), &QueryBuilder::set_type);
        ClassDB::bind_method(D_METHOD("get_type"), &QueryBuilder::get_type);
        ADD_PROPERTY(
            PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM,
                "None, Achievement, Activity, Aggregation, Contract, Controller, Entity, Event, PlayerAchievement, Search, Token, TokenBalance, TokenContract, TokenTransfer, Transaction"
            ), "set_type", "get_type");

        ClassDB::bind_method(D_METHOD("pagination", "limit", "cursor", "direction"), &QueryBuilder::pagination);
        ClassDB::bind_method(D_METHOD("order_by", "field", "direction"), &QueryBuilder::order_by);
    }
};

VARIANT_ENUM_CAST(QueryBuilder::Type);
