#include "ref_counted/queries/search.hpp"
#include "tools/logger.h"

Ref<SearchQuery> SearchQuery::query(const String& query) {
    this->p_query = query;
    return this;
}

Ref<SearchQuery> SearchQuery::limit(const uint32_t& limit) {
    this->p_limit = limit;
    return this;
}

Ref<SearchQuery> SearchQuery::pagination(const uint32_t& limit, const String& cursor) {
    Logger::warning("SearchQuery does not support pagination (cursor). Use limit() instead.");
    this->p_limit = limit; // We can at least set the limit
    return this;
}

Ref<SearchQuery> SearchQuery::cursor(const String& cursor) {
    Logger::warning("SearchQuery does not support cursor.");
    return this;
}

Ref<SearchQuery> SearchQuery::order_by(const String& field, const int64_t& direction) {
    Logger::warning("SearchQuery does not support order_by.");
    return this;
}

dojo::SearchQuery SearchQuery::get_native() const {
    dojo::SearchQuery query;
    query.query = p_query.utf8().get_data();
    query.limit = p_limit;
    return query;
}
