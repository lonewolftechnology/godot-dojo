//
// Created by chozo on 13-10-25.
//

#ifndef ACHIEVEMENT_PROGRESSION_H
#define ACHIEVEMENT_PROGRESSION_H

#include "dojo_types.h"
#include "resources/dojo_subscription.h"

class AchievementProgressionSubscription: public DojoSubscription{
    GDCLASS(AchievementProgressionSubscription, DojoSubscription)

    TypedArray<String> world_addresses;
    TypedArray<String> namespaces;
    TypedArray<String> player_addresses;
    TypedArray<String> achievements_ids;

public:
    AchievementProgressionSubscription(){}
    ~AchievementProgressionSubscription(){}
    Type get_type() const override {
        return ACHIEVEMENT_PROGRESSION;
    }

    TypedArray<String> get_world_addresses(){ return world_addresses; }
    void set_world_addresses(const TypedArray<String>& p_world_addresses){ world_addresses = p_world_addresses; }

    TypedArray<String> get_namespaces(){ return namespaces; }
    void set_namespaces(const TypedArray<String>& p_namespaces){ namespaces = p_namespaces; }

    TypedArray<String> get_player_addresses(){ return player_addresses; }
    void set_player_addresses(const TypedArray<String>& p_player_addresses){ player_addresses = p_player_addresses; }

    TypedArray<String> get_achievements_ids(){ return achievements_ids; }
    void set_achievements_ids(const TypedArray<String>& p_achievements_ids){ achievements_ids = p_achievements_ids; }

    DOJO::CArrayc_char get_native_namespaces() const {
        return DojoArrayHelpers::string_array_to_native_carray_str(namespaces);
    }

    DOJO::CArrayFieldElement get_native_world_addresses() const {
        return DojoArrayHelpers::string_array_to_native_carray_felt(world_addresses);
    }

    DOJO::CArrayFieldElement get_native_player_addresses() const {
        return DojoArrayHelpers::string_array_to_native_carray_felt(player_addresses);
    }

    DOJO::CArrayc_char get_native_achievements_ids() const {
        return DojoArrayHelpers::string_array_to_native_carray_str(achievements_ids);
    }


protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_world_addresses"), &AchievementProgressionSubscription::get_world_addresses);
        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_world_addresses"), &AchievementProgressionSubscription::set_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "world_addresses"), "set_world_addresses", "get_world_addresses");

        ClassDB::bind_method(D_METHOD("get_namespaces"), &AchievementProgressionSubscription::get_namespaces);
        ClassDB::bind_method(D_METHOD("set_namespaces", "p_namespaces"), &AchievementProgressionSubscription::set_namespaces);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "namespaces"), "set_namespaces", "get_namespaces");

        ClassDB::bind_method(D_METHOD("get_player_addresses"), &AchievementProgressionSubscription::get_player_addresses);
        ClassDB::bind_method(D_METHOD("set_player_addresses", "p_player_addresses"), &AchievementProgressionSubscription::set_player_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "player_addresses"), "set_player_addresses", "get_player_addresses");

        ClassDB::bind_method(D_METHOD("get_achievements_ids"), &AchievementProgressionSubscription::get_achievements_ids);
        ClassDB::bind_method(D_METHOD("set_achievements_ids", "p_achievements_ids"), &AchievementProgressionSubscription::set_achievements_ids);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "achievements_ids"), "set_achievements_ids", "get_achievements_ids");

    }
};

#endif //ACHIEVEMENT_PROGRESSION_H