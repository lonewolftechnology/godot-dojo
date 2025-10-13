//
// Created by chozo on 13-10-25.
//

#ifndef DOJO_ACHIEVEMENT_QUERY_H
#define DOJO_ACHIEVEMENT_QUERY_H
#include "dojo_types.h"
#include "resources/dojo_query_base.h"
using namespace godot;

class DojoAchievementQuery : public DojoQueryBase {
    GDCLASS(DojoAchievementQuery, DojoQueryBase)

public:
    DojoAchievementQuery(){}
    ~DojoAchievementQuery(){}


protected:
    static void _bind_methods() {

    }
};

#endif //DOJO_ACHIEVEMENT_QUERY_H