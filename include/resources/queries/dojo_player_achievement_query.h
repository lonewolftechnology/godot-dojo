//
// Created by chozo on 13-10-25.
//

#ifndef DOJO_PLAYER_ACHIEVEMENT_QUERY_H
#define DOJO_PLAYER_ACHIEVEMENT_QUERY_H
#include "dojo_types.h"
#include "resources/dojo_query_base.h"
using namespace godot;

class DojoPlayerAchievementQuery : public DojoQueryBase {
    GDCLASS(DojoPlayerAchievementQuery, DojoQueryBase)

public:
    DojoPlayerAchievementQuery(){}
    ~DojoPlayerAchievementQuery(){}


protected:
    static void _bind_methods() {

    }
};

#endif //DOJO_PLAYER_ACHIEVEMENT_QUERY_H