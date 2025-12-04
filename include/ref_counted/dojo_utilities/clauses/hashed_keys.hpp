#pragma once

#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;

class HashedKeysClause : public RefCounted {
    GDCLASS(HashedKeysClause, RefCounted)

    TypedArray<String> hashed_keys;

public:
    HashedKeysClause();
    ~HashedKeysClause();

protected:
    static void _bind_methods() {

    }
};