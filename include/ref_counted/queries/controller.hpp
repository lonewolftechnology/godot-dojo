#pragma once

#include "ref_counted/query_builder.hpp"

class ControllerQuery : public QueryBuilder {
    GDCLASS(ControllerQuery, QueryBuilder)

public:
    ControllerQuery() : QueryBuilder(Controller){}

protected:
    void static _bind_methods() {
    }
};