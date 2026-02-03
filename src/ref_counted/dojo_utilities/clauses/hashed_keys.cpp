#include "ref_counted/dojo_utilities/clauses/hashed_keys.hpp"

HashedKeysClause::HashedKeysClause() : DojoClause(DojoClause::HashedKeys) {}
HashedKeysClause::~HashedKeysClause() {}

Ref<HashedKeysClause> HashedKeysClause::add_key(const String& hashed_key) {
    if(!hashed_key.begins_with("0x"))
    {
        Logger::error("Invalid hex");
        return this;
    }

    this->p_hashed_keys.push_back(hashed_key);
    return this;
}

Ref<HashedKeysClause> HashedKeysClause::remove_key(const String& hashed_key) {
    this->p_hashed_keys.erase(hashed_key);
    return this;
}

Ref<HashedKeysClause> HashedKeysClause::keys(const PackedStringArray& keys) {
    //TODO: validate felts
    this->p_hashed_keys = keys;
    return this;
}

dojo::Clause HashedKeysClause::get_native() const {
    std::vector<dojo::FieldElement> keys;
    for (int i = 0; i < p_hashed_keys.size(); i++) {
        String key = p_hashed_keys[i];
        keys.push_back(key.utf8().get_data());
    }
    return dojo::Clause(dojo::Clause::kHashedKeys{keys});
}

Dictionary HashedKeysClause::to_dict() const {
    Dictionary result = {};
    result["hashed_keys"] = p_hashed_keys;
    return result;
}