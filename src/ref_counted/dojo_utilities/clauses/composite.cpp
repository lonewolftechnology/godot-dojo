#include "ref_counted/dojo_utilities/clauses/composite.hpp"

CompositeClause::CompositeClause() : DojoClause(DojoClause::Composite) {}
CompositeClause::~CompositeClause() {}

Ref<CompositeClause> CompositeClause::set_operator(int64_t p_op) {
    this->p_operator = p_op;
    return this;
}

Ref<CompositeClause> CompositeClause::add(const Ref<DojoClause>& clause) {
    p_clauses.append(clause);
    return this;
}

dojo::Clause CompositeClause::get_native() const {
    dojo::CompositeClause composite_clause;
    composite_clause.operator_ = static_cast<dojo::LogicalOperator>(p_operator);

    for (int i = 0; i < p_clauses.size(); i++) {
        Ref<DojoClause> clause = p_clauses[i];
        if (clause.is_valid()) {
            composite_clause.clauses.push_back(std::make_shared<dojo::Clause>(clause->get_native()));
        }
    }
    return dojo::Clause(dojo::Clause::kComposite{std::make_shared<dojo::CompositeClause>(composite_clause)});
}

Dictionary CompositeClause::to_dict() const {
    Dictionary result;
    result["operator"] = p_operator;

    Array clauses;
    for (int i = 0; i < p_clauses.size(); i++) {
        Ref<DojoClause> clause = p_clauses[i];
        if (clause.is_valid()) {
            clauses.append(clause->to_dict());
        }
    }
    result["clauses"] = clauses;

    return result;
}