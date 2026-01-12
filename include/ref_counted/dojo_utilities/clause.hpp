#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "dojo/dojo.hpp"

using namespace godot;

class DojoClause : public RefCounted {
    GDCLASS(DojoClause, RefCounted)

public:
    enum Type
    {
        HashedKeys,
        Keys,
        Member,
        Composite
    };

private:
    Type p_type;

public:
    DojoClause() : p_type(Keys) {}
    DojoClause(Type type) : p_type(type) {}
    ~DojoClause() {}

    Type get_type() const { return p_type; }

    virtual dojo::Clause get_native() const {
        return dojo::Clause(dojo::Clause::kHashedKeys{}); // Dummy return
    }

    virtual Dictionary to_dict() const {
        return Dictionary();
    }

protected:
    static void _bind_methods() {
        BIND_ENUM_CONSTANT(HashedKeys)
        BIND_ENUM_CONSTANT(Keys)
        BIND_ENUM_CONSTANT(Member)
        BIND_ENUM_CONSTANT(Composite)

        ClassDB::bind_method(D_METHOD("get_type"), &DojoClause::get_type);
        ClassDB::bind_method(D_METHOD("to_dict"), &DojoClause::to_dict);

        ADD_PROPERTY(PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM, "HashedKeys, Keys, Member, Composite"), "", "get_type");
    }
};

VARIANT_ENUM_CAST(DojoClause::Type)