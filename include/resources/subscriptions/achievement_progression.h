//
// Created by chozo on 13-10-25.
//

#ifndef ACHIEVEMENT_PROGRESSION_H
#define ACHIEVEMENT_PROGRESSION_H

#include "dojo_types.h"
#include "resources/dojo_subscription.h"

class AchievementProgressionSubscription: public DojoSubscription{
    GDCLASS(AchievementProgressionSubscription, DojoSubscription)

public:
    AchievementProgressionSubscription(){}
    ~AchievementProgressionSubscription(){}
    Type get_type() const override {
        return ACHIEVEMENT_PROGRESSION;
    }

protected:
    static void _bind_methods() {

    }
};

#endif //ACHIEVEMENT_PROGRESSION_H