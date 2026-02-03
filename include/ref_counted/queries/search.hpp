#pragma once

#include "ref_counted/query_builder.hpp"
#include "dojo/dojo.hpp"

class SearchQuery : public QueryBuilder {
    GDCLASS(SearchQuery, QueryBuilder)

    String p_query{};
    uint32_t p_limit = 0;

public:
    SearchQuery() : QueryBuilder(Search){}

    Ref<SearchQuery> query(const String& query);
    Ref<SearchQuery> limit(const uint32_t& limit);

    // Override to warn about usage
    Ref<SearchQuery> pagination(const uint32_t& limit, const String& cursor, const int64_t& direction);
    Ref<SearchQuery> cursor(const String& cursor);
    Ref<SearchQuery> order_by(const String& field, const int64_t& direction);

    dojo::SearchQuery get_native() const;

protected:
    void static _bind_methods() {
        ClassDB::bind_method(D_METHOD("query", "query"), &SearchQuery::query);
        ClassDB::bind_method(D_METHOD("limit", "limit"), &SearchQuery::limit);

        // Bind these to warn users
        ClassDB::bind_method(D_METHOD("pagination", "limit", "cursor"), &SearchQuery::pagination);
        ClassDB::bind_method(D_METHOD("cursor", "cursor"), &SearchQuery::cursor);
        ClassDB::bind_method(D_METHOD("order_by", "field", "direction"), &SearchQuery::order_by);
    }
};
