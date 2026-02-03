#pragma once

#include "dojo/dojo.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "ref_counted/dojo_utilities/clause.hpp"

using namespace godot;

class CompositeClause : public DojoClause {
    GDCLASS(CompositeClause, DojoClause)

    int64_t p_operator = static_cast<int64_t>(dojo::LogicalOperator::kAnd);
    TypedArray<DojoClause> p_clauses;

public:
    CompositeClause();
    ~CompositeClause();

    Ref<CompositeClause> set_operator(int64_t p_op);
    Ref<CompositeClause> add(const Ref<DojoClause>& clause);

    dojo::Clause get_native() const override;
    Dictionary to_dict() const override;

protected:
    static void _bind_methods() {
        ClassDB::bind_integer_constant(get_class_static(), "LogicalOperator", "And", static_cast<int>(dojo::LogicalOperator::kAnd));
        ClassDB::bind_integer_constant(get_class_static(), "LogicalOperator", "Or", static_cast<int>(dojo::LogicalOperator::kOr));

        ClassDB::bind_method(D_METHOD("set_operator", "operator"), &CompositeClause::set_operator);
        ClassDB::bind_method(D_METHOD("add", "clause"), &CompositeClause::add);
        ClassDB::bind_method(D_METHOD("to_dict"), &CompositeClause::to_dict);
    }
};