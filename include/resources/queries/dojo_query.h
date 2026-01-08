//
// Created by hazel on 7/8/25.
//

#ifndef DOJO_QUERY_H
#define DOJO_QUERY_H
#include "ref_counted/options/option_clause.h"
#include "resources/dojo_query_base.h"
#include "variant/ty/dojo_array.h"
#include "dojo_types.h"

using namespace godot;

class DojoQuery : public DojoQueryBase
{
    GDCLASS(DojoQuery, DojoQueryBase);
    Ref<DojoOptionClause> clause;
    bool no_hashed_keys;
    TypedArray<String> models;
    bool historical;
    TypedArray<String> world_addresses;

public:
    DojoQuery()
    {
        no_hashed_keys = false;
        historical = false;
    };

    ~DojoQuery()
    {
    };

    void* get_native_query() const {
        auto* query = new DOJO::Query();
        
        // Setup pagination
        query->pagination.direction = get_direction();
        
        if (get_limit() > 0) {
            query->pagination.limit.tag = DOJO::Someu32;
            query->pagination.limit.some = get_limit();
        } else {
            query->pagination.limit.tag = DOJO::Noneu32;
        }

        if (!get_cursor().is_empty()) {
            query->pagination.cursor.tag = DOJO::Somec_char;
            char *c_str_cursor = static_cast<char*>(malloc(get_cursor().utf8().length() + 1));
            strcpy(c_str_cursor, get_cursor().utf8().get_data());
            query->pagination.cursor.some = c_str_cursor;
        } else {
            query->pagination.cursor.tag = DOJO::Nonec_char;
        }
        
        query->pagination.order_by.data = nullptr;
        query->pagination.order_by.data_len = 0;

        if (clause.is_valid()) {
            query->clause = clause->get_native_option();
        } else {
            query->clause.tag = DOJO::NoneClause;
        }

        query->no_hashed_keys = get_no_hashed_keys();
        query->historical = get_historical();

        DOJO::CArrayc_char native_models = DojoArrayHelpers::string_array_to_native_carray_str(models);
        query->models = native_models;

        return query;
    }

    static void free_native_query(void* p_query) {
        if (!p_query) return;
        auto* query = static_cast<DOJO::Query*>(p_query);

        if (query->pagination.cursor.tag == DOJO::Somec_char) {
            std::free((void*)query->pagination.cursor.some);
        }

        if (query->clause.tag == DOJO::SomeClause) {
            DojoOptionClause::free_native_clause(query->clause.some);
        }
        DojoArrayHelpers::free_native_carray_str(query->models);
        delete query; // Correctly use delete for an object created with new
    }

    Ref<DojoOptionClause> get_clause() const { return clause; }
    void set_clause(const Ref<DojoOptionClause>& p_clause) { clause = p_clause; }
    bool get_no_hashed_keys() const { return no_hashed_keys; }
    void set_no_hashed_keys(bool p_no_hashed_keys) { no_hashed_keys = p_no_hashed_keys; }
    TypedArray<String> get_models() const { return models; }
    void set_models(const TypedArray<String>& p_models) { models = p_models; }
    bool get_historical() const { return historical; }
    void set_historical(bool p_historical) { historical = p_historical; }

    DOJO::CArrayFieldElement get_native_world_addresses() const {
        if (world_addresses.is_empty()) {
            Logger::debug_extra("DojoQuery", "Empty world addresses, fetching ProjectSettings");
            TypedArray<String> setting = DojoHelpers::get_torii_setting("worlds");
            if (setting.is_empty()) {
                Logger::error("World addresses not found in Project Settings");
                static DOJO::FieldElement dummy; // A static dummy pointer.
                return {&dummy, 0};
            }
            return DojoArray::CFieldElementArrayHelper(setting).c_array;
        }
        return DojoArray::CFieldElementArrayHelper(world_addresses).c_array;
    }

    void set_world_addresses(const TypedArray<String>& p_addresses){ world_addresses = p_addresses; }
    TypedArray<String> get_world_addresses() const { return world_addresses; }
protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_clause"), &DojoQuery::get_clause);
        ClassDB::bind_method(D_METHOD("set_clause", "p_clause"), &DojoQuery::set_clause);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "clause", PROPERTY_HINT_RESOURCE_TYPE, "DojoOptionClause"), "set_clause", "get_clause");

        ClassDB::bind_method(D_METHOD("get_no_hashed_keys"), &DojoQuery::get_no_hashed_keys);
        ClassDB::bind_method(D_METHOD("set_no_hashed_keys", "p_no_hashed_keys"), &DojoQuery::set_no_hashed_keys);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "no_hashed_keys"), "set_no_hashed_keys", "get_no_hashed_keys");

        ClassDB::bind_method(D_METHOD("get_models"), &DojoQuery::get_models);
        ClassDB::bind_method(D_METHOD("set_models", "p_models"), &DojoQuery::set_models);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "models"), "set_models", "get_models");

        ClassDB::bind_method(D_METHOD("get_historical"), &DojoQuery::get_historical);
        ClassDB::bind_method(D_METHOD("set_historical", "p_historical"), &DojoQuery::set_historical);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_historical"), "set_historical", "get_historical");

        ClassDB::bind_method(D_METHOD("get_world_addresses"), &DojoQuery::get_world_addresses);
        ClassDB::bind_method(D_METHOD("set_world_addresses", "p_addresses"), &DojoQuery::set_world_addresses);
        ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "world_addresses"), "set_world_addresses", "get_world_addresses");
    };
};

#endif //DOJO_QUERY_H
