#pragma once

#include "dojo/dojo.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "ref_counted/dojo_utilities/clause.hpp"
#include "tools/logger.hpp"

using namespace godot;

class HashedKeysClause : public DojoClause {
    GDCLASS(HashedKeysClause, DojoClause)

    TypedArray<String> p_hashed_keys;

public:
    HashedKeysClause();
    ~HashedKeysClause();

    Ref<HashedKeysClause> add_key(const String& hashed_key);
    Ref<HashedKeysClause> remove_key(const String& hashed_key);
    Ref<HashedKeysClause> keys(const PackedStringArray& keys);

    dojo::Clause get_native() const override;
    Dictionary to_dict() const override;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("add_key", "hashed_key"), &HashedKeysClause::add_key);
        ClassDB::bind_method(D_METHOD("remove_key", "hashed_key"), &HashedKeysClause::remove_key);
        ClassDB::bind_method(D_METHOD("keys", "hashed_keys"), &HashedKeysClause::keys);
        ClassDB::bind_method(D_METHOD("to_dict"), &HashedKeysClause::to_dict);
    }
};