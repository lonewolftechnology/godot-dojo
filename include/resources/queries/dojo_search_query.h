#ifndef GODOT_DOJO_DOJO_SEARCH_QUERY_H
#define GODOT_DOJO_DOJO_SEARCH_QUERY_H

#include "godot_cpp/classes/resource.hpp"
#include "dojo_types.h"
using namespace godot;

class DojoSearchQuery : public Resource {
    GDCLASS(DojoSearchQuery, Resource)

    String query;
    uint32_t limit;

public:
    DojoSearchQuery() {
        query = "";
        limit = 10;
    }
    ~DojoSearchQuery() {}

    void set_query(const String& p_query){ query = p_query; }
    String get_query(){ return query; }

    void set_limit(const uint32_t p_limit){ limit = p_limit; }
    uint32_t get_limit(){ return limit; }

    DOJO::SearchQuery get_native_query() const {
        DOJO::SearchQuery native_query = {};

        char *c_str_query = new char[query.utf8().length() + 1];
        strcpy(c_str_query, query.utf8().get_data());

        native_query.query = c_str_query;
        native_query.limit = limit;
        return native_query;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_query", "query"), &DojoSearchQuery::set_query);
        ClassDB::bind_method(D_METHOD("get_query"), &DojoSearchQuery::get_query);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "query"), "set_query", "get_query");

        ClassDB::bind_method(D_METHOD("set_limit", "limit"), &DojoSearchQuery::set_limit);
        ClassDB::bind_method(D_METHOD("get_limit"), &DojoSearchQuery::get_limit);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "limit"), "set_limit", "get_limit");
    }
};

#endif //GODOT_DOJO_DOJO_SEARCH_QUERY_H