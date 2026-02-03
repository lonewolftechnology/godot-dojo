#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class TransactionQuery : public QueryBuilder {
    GDCLASS(TransactionQuery, QueryBuilder)

    // TransactionFilter fields
    PackedStringArray p_transaction_hashes{};
    PackedStringArray p_caller_addresses{};
    PackedStringArray p_contract_addresses{};
    PackedStringArray p_entrypoints{};
    PackedStringArray p_model_selectors{};
    uint64_t p_from_block = 0; // Option
    uint64_t p_to_block = 0; // Option

public:
    TransactionQuery() : QueryBuilder(Transaction){}

    Ref<TransactionQuery> transaction_hashes(const PackedStringArray& hashes);
    Ref<TransactionQuery> caller_addresses(const PackedStringArray& addresses);
    Ref<TransactionQuery> contract_addresses(const PackedStringArray& addresses);
    Ref<TransactionQuery> entrypoints(const PackedStringArray& entrypoints);
    Ref<TransactionQuery> model_selectors(const PackedStringArray& selectors);
    Ref<TransactionQuery> from_block(const uint64_t& block);
    Ref<TransactionQuery> to_block(const uint64_t& block);

    dojo::TransactionQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("transaction_hashes", "hashes"), &TransactionQuery::transaction_hashes);
        ClassDB::bind_method(D_METHOD("caller_addresses", "addresses"), &TransactionQuery::caller_addresses);
        ClassDB::bind_method(D_METHOD("contract_addresses", "addresses"), &TransactionQuery::contract_addresses);
        ClassDB::bind_method(D_METHOD("entrypoints", "entrypoints"), &TransactionQuery::entrypoints);
        ClassDB::bind_method(D_METHOD("model_selectors", "selectors"), &TransactionQuery::model_selectors);
        ClassDB::bind_method(D_METHOD("from_block", "block"), &TransactionQuery::from_block);
        ClassDB::bind_method(D_METHOD("to_block", "block"), &TransactionQuery::to_block);
    }
};
