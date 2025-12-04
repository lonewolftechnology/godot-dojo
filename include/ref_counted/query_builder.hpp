#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "dojo_types.h"

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
        PlayerAchievement,
        Search,
        Token,
        TokenBalance,
        TokenTransfer,
        Transaction
    };

private:
    // Base
    Type type = None;

public:
    QueryBuilder() = default;

    QueryBuilder(const Type &p_type);

    ~QueryBuilder();

    static Ref<QueryBuilder> create(const int64_t &p_type);
    // static Ref<QueryBuilder> create_from_dict(const Dictionary& dict);

    // Base
    Ref<QueryBuilder> set_type(const int64_t &p_type);

    int64_t get_type() const;

    Dictionary to_dict();


protected:
    static void _bind_methods() {
        // Type
        BIND_ENUM_CONSTANT(None);
        BIND_ENUM_CONSTANT(Achievement);
        BIND_ENUM_CONSTANT(Activity);
        BIND_ENUM_CONSTANT(Aggregation);
        BIND_ENUM_CONSTANT(Contract);
        BIND_ENUM_CONSTANT(Controller);
        BIND_ENUM_CONSTANT(PlayerAchievement);
        BIND_ENUM_CONSTANT(Entity);
        BIND_ENUM_CONSTANT(Search);
        BIND_ENUM_CONSTANT(Token);
        BIND_ENUM_CONSTANT(TokenBalance);
        BIND_ENUM_CONSTANT(TokenTransfer);
        BIND_ENUM_CONSTANT(Transaction);

        // Static Methods
        ClassDB::bind_static_method("QueryBuilder", D_METHOD("create", "type"), &QueryBuilder::create);

        // Instance Methods
        ClassDB::bind_method(D_METHOD("set_type", "type"), &QueryBuilder::set_type);
        ClassDB::bind_method(D_METHOD("get_type"), &QueryBuilder::get_type);
        ADD_PROPERTY(
            PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM,
                "None, Achievement, Activity, Aggregation, Contract, Controller, PlayerAchievement, Entity, Search, Token, TokenBalance, TokenTransfer, Transaction"
            ), "set_type", "get_type");
    }
};

VARIANT_ENUM_CAST(QueryBuilder::Type);
