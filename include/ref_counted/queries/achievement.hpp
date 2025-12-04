#pragma once

#include "ref_counted/query_builder.hpp"

class AchievementQuery : public QueryBuilder {
    GDCLASS(AchievementQuery, QueryBuilder)

public:
    AchievementQuery() : QueryBuilder(Achievement){}

protected:
    void static _bind_methods() {
    }
};