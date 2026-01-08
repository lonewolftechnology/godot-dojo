#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
using namespace godot;

class DojoClause : public RefCounted {
    GDCLASS(DojoClause, RefCounted)
    DOJO::Clause_Tag type;

public:
    DojoClause();
    ~DojoClause();

protected:
    static void _bind_methods() {
        ClassDB::bind_integer_constant(get_class_static(), "Clause", "HashedKeys", DOJO::Clause_Tag::HashedKeys);
        ClassDB::bind_integer_constant(get_class_static(), "Clause", "Keys", DOJO::Clause_Tag::Keys);
        ClassDB::bind_integer_constant(get_class_static(), "Clause", "Member", DOJO::Clause_Tag::CMember);
        ClassDB::bind_integer_constant(get_class_static(), "Clause", "Composite", DOJO::Clause_Tag::Composite);
    }
};

// VARIANT_ENUM_CAST(DOJO::Clause_Tag)