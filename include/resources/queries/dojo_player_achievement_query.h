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
    // CArrayFieldElement
    // CArrayc_char
    // CArrayFieldElement

    TypedArray<String> world_addresses;
    TypedArray<String> namespaces;
    TypedArray<String> player_addresses;

public:
    DojoPlayerAchievementQuery() {
    }

    ~DojoPlayerAchievementQuery() {
    }

    void set_world_addresses(const TypedArray<String> &p_world_addresses) { world_addresses = p_world_addresses; }
    void set_namespaces(const TypedArray<String> &p_namespaces) { namespaces = p_namespaces; }
    void set_player_addresses(const TypedArray<String> &p_player_addresses) { player_addresses = p_player_addresses; }

    TypedArray<String> get_world_addresses() { return world_addresses; }
    TypedArray<String> get_namespaces() { return namespaces; }
    TypedArray<String> get_player_addresses() { return player_addresses; }


    void *get_native_query() const {
        // Setup pagination
        auto *query = new DOJO::PlayerAchievementQuery();

        query->pagination = get_native_pagination();

        if (world_addresses.size() > 0) {
            DOJO::FieldElement *addresses_data = new DOJO::FieldElement[world_addresses.size()];
            for (int i = 0; i < world_addresses.size(); ++i) {
                String address_str = world_addresses[i];
                addresses_data[i] = DojoHelpers::string_to_field_element(address_str)->get_felt_no_ptr();
            }
            query->world_addresses.data = addresses_data;
            query->world_addresses.data_len = world_addresses.size();
        } else {
            query->world_addresses.data = nullptr;
            query->world_addresses.data_len = 0;
        }

        if (namespaces.size() > 0) {
            const char **namespaces_data = new const char *[namespaces.size()];
            for (int i = 0; i < namespaces.size(); ++i) {
                String ns_str = namespaces[i];
                char *c_str = new char[ns_str.utf8().length() + 1];
                strcpy(c_str, ns_str.utf8().get_data());
                namespaces_data[i] = c_str;
            }

            query->namespaces.data = namespaces_data;
            query->namespaces.data_len = namespaces.size();
        } else {
            query->namespaces.data = nullptr;
            query->namespaces.data_len = 0;
        }

        if (player_addresses.size() > 0) {
            DOJO::FieldElement *addresses_data = new DOJO::FieldElement[player_addresses.size()];
            for (int i = 0; i < player_addresses.size(); ++i) {
                String address_str = player_addresses[i];
                addresses_data[i] = DojoHelpers::string_to_field_element(address_str)->get_felt_no_ptr();
            }
            query->player_addresses.data = addresses_data;
            query->player_addresses.data_len = player_addresses.size();
        } else {
            query->player_addresses.data = nullptr;
            query->player_addresses.data_len = 0;
        }
        return query;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_world_addresses"), &DojoPlayerAchievementQuery::get_world_addresses);
        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_world_addresses"),
                             &DojoPlayerAchievementQuery::set_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "world_addresses"), "set_world_addresses",
                     "get_world_addresses");

        ClassDB::bind_method(D_METHOD("get_namespaces"), &DojoPlayerAchievementQuery::get_namespaces);
        ClassDB::bind_method(D_METHOD("set_namespaces", "p_namespaces"), &DojoPlayerAchievementQuery::set_namespaces);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "namespaces"), "set_namespaces", "get_namespaces");

        ClassDB::bind_method(D_METHOD("get_player_addresses"), &DojoPlayerAchievementQuery::get_player_addresses);
        ClassDB::bind_method(D_METHOD("set_player_addresses", "p_player_addresses"),
                             &DojoPlayerAchievementQuery::set_player_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "player_addresses"), "set_player_addresses",
                     "get_player_addresses");
    }
};

#endif //DOJO_PLAYER_ACHIEVEMENT_QUERY_H
