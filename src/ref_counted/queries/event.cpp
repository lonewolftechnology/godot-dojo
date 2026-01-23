#include "ref_counted/queries/event.hpp"

Ref<EventQuery> EventQuery::keys(const Ref<KeysClause>& keys) {
    this->p_keys = keys;
    return this;
}

dojo::EventQuery EventQuery::get_native() const {
    dojo::EventQuery query;

    if (p_keys.is_valid()) {
        // Extract native KeysClause from wrapper
        dojo::Clause native_clause_variant = p_keys->get_native();
        if (std::holds_alternative<dojo::Clause::kKeys>(native_clause_variant.get_variant())) {
            auto keys_variant = std::get<dojo::Clause::kKeys>(native_clause_variant.get_variant());
            query.keys = keys_variant.clause;
        }
    }

    auto pagination = get_native_pagination();
    pagination->order_by = get_order_by();
    query.pagination = pagination;

    return query;
}
