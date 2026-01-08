#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "dojo/dojo.hpp"

using namespace godot;

class MemberClause : public RefCounted {
    GDCLASS(MemberClause, RefCounted)

public:
    enum PrimitiveTag {
        Nil = -1,
        I8 = 0,
        I16,
        I32,
        I64,
        I128,
        U8,
        U16,
        U32,
        U64,
        U128,
        U256,
        Bool,
        Felt252,
        ClassHash,
        ContractAddress,
        EthAddress
    };

private:
    String p_model;
    String p_member;
    dojo::ComparisonOperator p_operator;
    Variant p_value; // MemberValue: Primitive, String, MemberArray
    PrimitiveTag p_primitive_tag = Nil;


public:
    MemberClause(){}
    ~MemberClause(){}

    Dictionary to_dict();

    dojo::MemberClause get_native() const;

    Ref<MemberClause> string(const String &value);
    Ref<MemberClause> list(const Variant **args, GDExtensionInt arg_count, GDExtensionCallError &error);

    // Primitive
    Ref<MemberClause> int_(const int& value, PrimitiveTag tag = Nil);
    Ref<MemberClause> bool_(const bool& value);
    Ref<MemberClause> hex(const String& value, PrimitiveTag tag = Nil);

    Ref<MemberClause> model(const Variant &model);
    Ref<MemberClause> op(const int &operator_);
    Ref<MemberClause> member(const Variant &model);

    static dojo::Primitive to_native_primitive(const Variant &p_value, PrimitiveTag p_tag);

    Variant get_value(){ return p_value; }
    bool is_value_set() const;


protected:
    static void _bind_methods() {
        BIND_ENUM_CONSTANT(Nil);
        BIND_ENUM_CONSTANT(I8);
        BIND_ENUM_CONSTANT(I16);
        BIND_ENUM_CONSTANT(I32);
        BIND_ENUM_CONSTANT(I64);
        BIND_ENUM_CONSTANT(I128);
        BIND_ENUM_CONSTANT(U8);
        BIND_ENUM_CONSTANT(U16);
        BIND_ENUM_CONSTANT(U32);
        BIND_ENUM_CONSTANT(U64);
        BIND_ENUM_CONSTANT(U128);
        BIND_ENUM_CONSTANT(U256);
        BIND_ENUM_CONSTANT(Bool);
        BIND_ENUM_CONSTANT(Felt252);
        BIND_ENUM_CONSTANT(ClassHash);
        BIND_ENUM_CONSTANT(ContractAddress);
        BIND_ENUM_CONSTANT(EthAddress);

        // Bind ComparisonOperator using dojo.hpp enum class values
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "Eq", static_cast<int>(dojo::ComparisonOperator::kEq));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "Neq", static_cast<int>(dojo::ComparisonOperator::kNeq));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "Gt", static_cast<int>(dojo::ComparisonOperator::kGt));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "Gte", static_cast<int>(dojo::ComparisonOperator::kGte));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "Lt", static_cast<int>(dojo::ComparisonOperator::kLt));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "Lte", static_cast<int>(dojo::ComparisonOperator::kLte));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "In", static_cast<int>(dojo::ComparisonOperator::kIn));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "NotIn", static_cast<int>(dojo::ComparisonOperator::kNotIn));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "Contains", static_cast<int>(dojo::ComparisonOperator::kContains));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "ContainsAll", static_cast<int>(dojo::ComparisonOperator::kContainsAll));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "ContainsAny", static_cast<int>(dojo::ComparisonOperator::kContainsAny));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "ArrayLengthEq", static_cast<int>(dojo::ComparisonOperator::kArrayLengthEq));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "ArrayLengthGt", static_cast<int>(dojo::ComparisonOperator::kArrayLengthGt));
        ClassDB::bind_integer_constant(get_class_static(), "ComparisonOperator", "ArrayLengthLt", static_cast<int>(dojo::ComparisonOperator::kArrayLengthLt));

        ClassDB::bind_method(D_METHOD("get_value"), &MemberClause::get_value);

        ClassDB::bind_method(D_METHOD("string", "value"), &MemberClause::string);
        // ClassDB::bind_method(D_METHOD("list", "value"), &MemberClause::list);
        {
            MethodInfo mi;
            // mi.arguments.push_back(PropertyInfo(Variant::NIL, "arg"));
            mi.name = "list";
            ClassDB::bind_vararg_method(METHOD_FLAGS_DEFAULT, "list", &MemberClause::list, mi);
        }

        ClassDB::bind_method(D_METHOD("int", "value", "tag"), &MemberClause::int_, DEFVAL(Nil));
        ClassDB::bind_method(D_METHOD("bool", "value"), &MemberClause::bool_);
        ClassDB::bind_method(D_METHOD("hex", "value", "tag"), &MemberClause::hex, DEFVAL(Nil));

        ClassDB::bind_method(D_METHOD("model", "value"), &MemberClause::model);
        ClassDB::bind_method(D_METHOD("op", "value"), &MemberClause::op);
        ClassDB::bind_method(D_METHOD("member", "value"), &MemberClause::member);

        ClassDB::bind_method(D_METHOD("to_dict"), &MemberClause::to_dict);
    }
};
VARIANT_ENUM_CAST(MemberClause::PrimitiveTag)