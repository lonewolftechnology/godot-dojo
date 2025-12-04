#pragma once

#include "ref_counted/query_builder.hpp"

class SearchQuery : public QueryBuilder {
    GDCLASS(SearchQuery, QueryBuilder)

public:
    SearchQuery() : QueryBuilder(Search){}

protected:
    void static _bind_methods() {
    }
};