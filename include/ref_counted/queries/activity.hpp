#pragma once

#include "ref_counted/query_builder.hpp"

class ActivityQuery : public QueryBuilder {
    GDCLASS(ActivityQuery, QueryBuilder)

public:
    ActivityQuery() : QueryBuilder(Activity){}

protected:
    void static _bind_methods() {
    }
};