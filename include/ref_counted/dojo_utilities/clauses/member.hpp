#pragma once

#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;

class MemberClause : public RefCounted {
    GDCLASS(MemberClause, RefCounted)
    String p_model;
    String p_member;
    DOJO::ComparisonOperator p_operator;
    Variant p_value; // MemberValue: Primitive, String, MemberArray

public:
    MemberClause();
    ~MemberClause();

protected:
    static void _bind_methods() {

    }
};