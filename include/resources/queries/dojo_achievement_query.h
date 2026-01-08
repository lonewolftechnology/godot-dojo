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
    TypedArray<String> world_addresses;
    TypedArray<String> namespaces;
    bool hidden;

public:
    DojoAchievementQuery() {}

    ~DojoAchievementQuery() {}

    void set_world_addresses(const TypedArray<String> &p_addresses) { world_addresses = p_addresses; }
    TypedArray<String> get_world_addresses() const { return world_addresses; }

    void set_namespaces(const TypedArray<String> &p_addresses) { namespaces = p_addresses; }
    TypedArray<String> get_namespaces() const { return namespaces; }

    void set_hidden(const bool &p_hidden) { hidden = p_hidden; }
    bool get_hidden() const { return hidden; }

    void* get_native_query() const {
        // Setup pagination
        auto* query = new DOJO::AchievementQuery();

        query->pagination = get_native_pagination();

        if (world_addresses.size() > 0) {
            DOJO::FieldElement* addresses_data = new DOJO::FieldElement[world_addresses.size()];
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
            const char** namespaces_data = new const char*[namespaces.size()];
            for (int i = 0; i < namespaces.size(); ++i) {
                String ns_str = namespaces[i];
                char* c_str = new char[ns_str.utf8().length() + 1];
                strcpy(c_str, ns_str.utf8().get_data());
                namespaces_data[i] = c_str;
            }

            query->namespaces.data = namespaces_data;
            query->namespaces.data_len = namespaces.size();
        } else {
            query->namespaces.data = nullptr;
            query->namespaces.data_len = 0;
        }

        query->hidden.some = hidden;

        return query;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_world_addresses"), &DojoAchievementQuery::get_world_addresses);
        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_addresses"), &DojoAchievementQuery::set_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "world_addresses"), "set_world_addresses", "get_world_addresses");

        ClassDB::bind_method(D_METHOD("get_namespaces"), &DojoAchievementQuery::get_namespaces);
        ClassDB::bind_method(D_METHOD("set_namespaces", "p_addresses"), &DojoAchievementQuery::set_namespaces);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "namespaces"), "set_namespaces", "get_namespaces");

        ClassDB::bind_method(D_METHOD("get_hidden"), &DojoAchievementQuery::get_hidden);
        ClassDB::bind_method(D_METHOD("set_hidden", "p_hidden"), &DojoAchievementQuery::set_hidden);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "hidden"), "set_hidden", "get_hidden");
    }
};

#endif //DOJO_ACHIEVEMENT_QUERY_H
