#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
using namespace godot;

class DojoClause : public RefCounted {
    GDCLASS(DojoClause, RefCounted)

    DOJO::Clause_Tag tag;

    // Hashed Keys
    TypedArray<String> hashed_keys;

    // Keys
    Array keys; // Using generic Array to avoid TypedArray<Ref<T>> issues
    DOJO::PatternMatching pattern_matching;
    TypedArray<String> models;

    // Member
    String model;
    String member;
    DOJO::ComparisonOperator comparison_operator;
    DOJO::MemberValue_Tag member_tag;
    DOJO::Primitive_Tag primitive_tag;
    Variant value;

    // Composite
    DOJO::LogicalOperator logical_operator;
    Array clauses; // Using generic Array to avoid TypedArray<Ref<T>> issues

public:
    DojoClause();
    ~DojoClause();

protected:
    static void _bind_methods() {

    }
};