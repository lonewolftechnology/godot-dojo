#include "ref_counted/dojo_utilities/clauses/keys.hpp"

KeysClause::KeysClause() : DojoClause(DojoClause::Keys) {
    p_patter_matching = dojo::PatternMatching::kFixedLen;
}

KeysClause::~KeysClause() {}

Ref<KeysClause> KeysClause::add_key(const String& hashed_key) {
    this->p_keys.push_back(hashed_key);
    return this;
}

Ref<KeysClause> KeysClause::remove_key(const String& hashed_key) {
    this->p_keys.erase(hashed_key);
    return this;
}

Ref<KeysClause> KeysClause::keys(const PackedStringArray& keys) {
    this->p_keys = keys;
    return this;
}

Ref<KeysClause> KeysClause::add_model(const String& model) {
    this->p_models.push_back(model);
    return this;
}

Ref<KeysClause> KeysClause::remove_model(const String& model) {
    this->p_models.erase(model);
    return this;
}

Ref<KeysClause> KeysClause::models(const PackedStringArray& models) {
    this->p_models = models;
    return this;
}

Ref<KeysClause> KeysClause::pattern(const int64_t& pattern) {
    this->p_patter_matching = static_cast<dojo::PatternMatching>(pattern);
    return this;
}

dojo::Clause KeysClause::get_native() const {
    dojo::KeysClause clause = {};

    std::vector<std::optional<dojo::FieldElement>> keys;
    for (int i = 0; i < p_keys.size(); i++) {
        String key = p_keys[i];
        keys.push_back(std::string(key.utf8().get_data()));
    }
    clause.keys = keys;

    clause.pattern_matching = p_patter_matching;

    std::vector<std::string> models;
    for (int i = 0; i < p_models.size(); i++) {
        String model = p_models[i];
        models.push_back(std::string(model.utf8().get_data()));
    }
    clause.models = models;

    return dojo::Clause(dojo::Clause::kKeys{std::make_shared<dojo::KeysClause>(clause)});
}

Dictionary KeysClause::to_dict() const {
    Dictionary result = {};
    result["keys"] = p_keys;
    result["pattern"] = static_cast<int>(p_patter_matching);
    result["models"] = p_models;
    return result;
}