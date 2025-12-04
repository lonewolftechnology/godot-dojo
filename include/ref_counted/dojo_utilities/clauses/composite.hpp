#pragma once

#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "ref_counted/dojo_utilities/clause.hpp"

using namespace godot;

class CompositeClause : public RefCounted {
    GDCLASS(CompositeClause, RefCounted)
    DOJO::LogicalOperator p_operator;
    TypedArray<DojoClause> p_clauses;

public:
    CompositeClause();
    ~CompositeClause();7

protected:
    static void _bind_methods() {

    }
};