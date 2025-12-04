#pragma once

#include "ref_counted/query_builder.hpp"

class PlayerAchievementQuery : public QueryBuilder {
    GDCLASS(PlayerAchievementQuery, QueryBuilder)

public:
    PlayerAchievementQuery() : QueryBuilder(PlayerAchievement){}

protected:
    void static _bind_methods() {
    }
};