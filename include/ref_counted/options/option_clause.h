//
// Created by hazel on 1/08/25.
//

#ifndef OPTION_CLAUSE_H
#define OPTION_CLAUSE_H

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "dojo_types.h"
#include "option_field_element.h"
#include "tools/dojo_helper.h"
#include "variant/ty/primitive.h"
#include "variant/field_element.h"
#include "variant/ty/dojo_array.h"

using namespace godot;

class DojoOptionClause : public DojoOption
{
    GDCLASS(DojoOptionClause, DojoOption);

    // Clause type tag
    DOJO::Clause_Tag tag;

    // Hashed Keys
    TypedArray<String> hashed_keys;

    // Keys
    Array keys; // Using generic Array to avoid TypedArray<Ref<T>> issues
    DOJO::PatternMatching pattern_matching;
    TypedArray<String> models;

    // Member
    String model;
    String member;
    DOJO::ComparisonOperator comparison_operator;
    DOJO::MemberValue_Tag member_tag;
    DOJO::Primitive_Tag primitive_tag;
    Variant value;

    // Composite
    DOJO::LogicalOperator logical_operator;
    Array clauses; // Using generic Array to avoid TypedArray<Ref<T>> issues

    static DOJO::Primitive to_native_primitive(const Variant& p_value, DOJO::Primitive_Tag p_tag)
    {
        DOJO::Primitive primitive = {};
        primitive.tag = p_tag;
        String str_val = p_value;

        switch (p_tag)
        {
        case DOJO::Primitive_Tag::I8: DojoHelpers::string_to_i8(str_val, &primitive.i8);
            break;
        case DOJO::Primitive_Tag::I16: DojoHelpers::string_to_i16(str_val, &primitive.i16);
            break;
        case DOJO::Primitive_Tag::I32: DojoHelpers::string_to_i32(str_val, &primitive.i32);
            break;
        case DOJO::Primitive_Tag::I64: DojoHelpers::string_to_i64(str_val, &primitive.i64);
            break;
        case DOJO::Primitive_Tag::I128: DojoHelpers::string_to_i128(str_val, primitive.i128);
            break;
        case DOJO::Primitive_Tag::U8: primitive.u8 = static_cast<uint8_t>(str_val.to_int());
            break;
        case DOJO::Primitive_Tag::U16: primitive.u16 = static_cast<uint16_t>(str_val.to_int());
            break;
        case DOJO::Primitive_Tag::U32: primitive.u32 = static_cast<uint32_t>(str_val.to_int());
            break;
        case DOJO::Primitive_Tag::U64: primitive.u64 = str_val.to_int();
            break;
        case DOJO::Primitive_Tag::U128: DojoHelpers::string_to_u128(str_val, primitive.u128);
            break;
        case DOJO::Primitive_Tag::U256_: primitive.u256 = DojoHelpers::string_to_u256(str_val);
            break;
        case DOJO::Primitive_Tag::Bool: primitive.bool_ = static_cast<bool>(p_value);
            break;
        case DOJO::Primitive_Tag::Felt252:
        case DOJO::Primitive_Tag::ClassHash:
        case DOJO::Primitive_Tag::ContractAddress:
        case DOJO::Primitive_Tag::EthAddress:
            {
                primitive.felt252 = FieldElement::from_string(str_val);
                break;
            }
        }
        return primitive;
    }

    static DOJO::MemberValue to_native_member_value(const Variant& p_value, DOJO::MemberValue_Tag p_member_tag,
                                                    DOJO::Primitive_Tag p_primitive_tag)
    {
        DOJO::MemberValue native_value = {};
        native_value.tag = p_member_tag;

        switch (p_member_tag)
        {
        case DOJO::MemberValue_Tag::PrimitiveValue:
            native_value.primitive_value = to_native_primitive(p_value, p_primitive_tag);
            break;
        case DOJO::MemberValue_Tag::String:
            {
                String str = p_value;
                char* c_str = new char[str.utf8().length() + 1];
                strcpy(c_str, str.utf8().get_data());
                native_value.string = c_str;
                break;
            }
        case DOJO::MemberValue_Tag::List:
            {
                Array arr = p_value;
                auto* native_list = new DOJO::MemberValue[arr.size()];
                for (int i = 0; i < arr.size(); ++i)
                {
                    native_list[i].tag = DOJO::MemberValue_Tag::PrimitiveValue;
                    native_list[i].primitive_value = to_native_primitive(arr[i], p_primitive_tag);
                }
                native_value.list = {native_list, (uintptr_t)arr.size()};
                break;
            }
        }
        return native_value;
    }

private:
    static char* string_to_c_char(const String& p_string)
    {
        CharString utf8_str = p_string.utf8();
        char* c_str = new char[utf8_str.length() + 1];
        strcpy(c_str, utf8_str.get_data());
        return c_str;
    }

public:
    DojoOptionClause()
    {
        tag = DOJO::Clause_Tag::HashedKeys;
        pattern_matching = DOJO::PatternMatching::FixedLen;
        comparison_operator = DOJO::ComparisonOperator::Eq;
        member_tag = DOJO::MemberValue_Tag::PrimitiveValue;
        primitive_tag = DOJO::Primitive_Tag::U256_;
        logical_operator = DOJO::LogicalOperator::And;
    }

    void load_from_native(const DOJO::Clause& p_clause)
    {
        hashed_keys = TypedArray<String>();
        keys = Array();
        models = TypedArray<String>();
        model = String();
        member = String();
        value = Variant();
        clauses = Array();

        tag = p_clause.tag;
        // TODO: TypedArray<FieldElement> ?

        switch (tag)
        {
        case DOJO::Clause_Tag::HashedKeys:
            {
                TypedArray<String> out;
                for (uintptr_t i = 0; i < p_clause.hashed_keys.data_len; ++i)
                {
                    Ref<FieldElement> felt = memnew(FieldElement(p_clause.hashed_keys.data));
                    out.push_back(felt->to_string());
                }
            }
        case DOJO::Clause_Tag::Keys:
            {
                Array out_keys;
                for (uintptr_t i = 0; i < p_clause.keys.keys.data_len; ++i)
                {
                    const DOJO::COptionFieldElement& k = p_clause.keys.keys.data[i];
                    Ref<DojoOptionFieldElement> opt = memnew(DojoOptionFieldElement());
                    if (k.tag == DOJO::COptionFieldElement_Tag::SomeFieldElement)
                    {
                        Ref<FieldElement> felt = memnew(FieldElement(k.some));
                        opt->set_value(felt->to_string());
                    }
                    else
                    {
                        opt->set_value(Variant());
                    }
                    out_keys.push_back(opt);
                }
                keys = out_keys;
                pattern_matching = p_clause.keys.pattern_matching;
                TypedArray<String> out_models;
                for (uintptr_t i = 0; i < p_clause.keys.models.data_len; ++i)
                {
                    out_models.push_back(String(p_clause.keys.models.data[i]));
                }
                models = out_models;
                break;
            }
        case DOJO::Clause_Tag::CMember:
            {
                model = String(p_clause.c_member.model);
                member = String(p_clause.c_member.member);
                comparison_operator = p_clause.c_member.operator_;

                auto mv = p_clause.c_member.value;
                member_tag = mv.tag;

                switch (mv.tag)
                {
                case DOJO::MemberValue_Tag::PrimitiveValue:
                    {
                        primitive_tag = mv.primitive_value.tag;
                        if (primitive_tag == DOJO::Primitive_Tag::Felt252 ||
                            primitive_tag == DOJO::Primitive_Tag::ClassHash ||
                            primitive_tag == DOJO::Primitive_Tag::ContractAddress ||
                            primitive_tag == DOJO::Primitive_Tag::EthAddress)
                        {
                            value = DojoPrimitive::FieldElementFromPrimitive(mv.primitive_value);
                        }
                        else
                        {
                            value = DojoPrimitive::VariantFromPrimitive(mv.primitive_value);
                        }
                        break;
                    }
                case DOJO::MemberValue_Tag::String:
                    {
                        primitive_tag = DOJO::Primitive_Tag::U256_; // keep default; not used for String
                        value = String(mv.string);
                        break;
                    }
                case DOJO::MemberValue_Tag::List:
                    {
                        // Convert list of MemberValue (assumed primitives)
                        Array arr;
                        if (mv.list.data_len > 0)
                        {
                            // infer primitive tag from first element if primitive
                            const DOJO::MemberValue& first = mv.list.data[0];
                            if (first.tag == DOJO::MemberValue_Tag::PrimitiveValue)
                            {
                                primitive_tag = first.primitive_value.tag;
                            }
                        }
                        for (uintptr_t i = 0; i < mv.list.data_len; ++i)
                        {
                            const DOJO::MemberValue& elem = mv.list.data[i];
                            if (elem.tag == DOJO::MemberValue_Tag::PrimitiveValue)
                            {
                                if (elem.primitive_value.tag == DOJO::Primitive_Tag::Felt252 ||
                                    elem.primitive_value.tag == DOJO::Primitive_Tag::ClassHash ||
                                    elem.primitive_value.tag == DOJO::Primitive_Tag::ContractAddress ||
                                    elem.primitive_value.tag == DOJO::Primitive_Tag::EthAddress)
                                {
                                    arr.push_back(DojoPrimitive::FieldElementFromPrimitive(elem.primitive_value));
                                }
                                else
                                {
                                    arr.push_back(DojoPrimitive::VariantFromPrimitive(elem.primitive_value));
                                }
                            }
                        }
                        value = arr;
                        break;
                    }
                }
                break;
            }
        case DOJO::Clause_Tag::Composite:
            {
                logical_operator = p_clause.composite.operator_;
                Array out_clauses;
                for (uintptr_t i = 0; i < p_clause.composite.clauses.data_len; ++i)
                {
                    Ref<DojoOptionClause> sub = from_native(p_clause.composite.clauses.data[i]);
                    out_clauses.push_back(sub);
                }
                clauses = out_clauses;
                break;
            }
        }
    }

    static Ref<DojoOptionClause> from_native(const DOJO::Clause& p_clause)
    {
        Ref<DojoOptionClause> ref = memnew(DojoOptionClause());
        ref->load_from_native(p_clause);
        return ref;
    }

    static Ref<DojoOptionClause> from_native_option(const DOJO::COptionClause& p_opt)
    {
        if (p_opt.tag == DOJO::COptionClause_Tag::SomeClause)
        {
            return from_native(p_opt.some);
        }
        return Ref<DojoOptionClause>();
    }

    DOJO::COptionClause get_native_option() const
    {
        DOJO::COptionClause option = {};

        option.tag = DOJO::SomeClause;
        option.some.tag = tag;

        switch (tag)
        {
        case DOJO::Clause_Tag::HashedKeys:
            {
                option.some.hashed_keys = DojoArrayHelpers::string_array_to_native_carray_felt(hashed_keys);
                break;
            }
        case DOJO::Clause_Tag::Keys:
            {
                DOJO::CArrayCOptionFieldElement native_keys = DojoArrayHelpers::option_field_element_array_to_native_carray(keys);
                DOJO::CArrayc_char native_models = DojoArrayHelpers::string_array_to_native_carray_str(models);

                option.some.keys = {
                    native_keys,
                    pattern_matching,
                    native_models
                };
                break;
            }
        case DOJO::Clause_Tag::CMember:
            {
                option.some.c_member.model = string_to_c_char(model);
                option.some.c_member.member = string_to_c_char(member);
                option.some.c_member.operator_ = comparison_operator;
                option.some.c_member.value = to_native_member_value(value, member_tag, primitive_tag);
                break;
            }
        case DOJO::Clause_Tag::Composite:
            {
                auto* native_clauses = new DOJO::Clause[clauses.size()];
                for (int i = 0; i < clauses.size(); ++i)
                {
                    Ref<DojoOptionClause> clause_ref = clauses[i];
                    if (clause_ref.is_valid())
                    {
                        native_clauses[i] = clause_ref->get_native_clause();
                    }
                }
                option.some.composite = {
                    logical_operator,
                    {native_clauses, (uintptr_t)clauses.size()}
                };
                break;
            }
        }
        return option;
    }

    Dictionary to_json() const override
    {
        // TODO: Implement the actual serialization to Dictionary
        return Dictionary();
    }

    DOJO::Clause get_native_clause() const
    {
        return get_native_option().some;
    }


    DOJO::Clause_Tag get_tag() const { return tag; }

    void set_tag(DOJO::Clause_Tag p_type)
    {
        tag = p_type;
        notify_property_list_changed();
    }

    TypedArray<String> get_hashed_keys() const { return hashed_keys; }
    void set_hashed_keys(const TypedArray<String>& p_keys) { hashed_keys = p_keys; }

    Array get_keys() const { return keys; }
    void set_keys(const Array& p_keys) { keys = p_keys; }

    DOJO::PatternMatching get_pattern_matching() const { return pattern_matching; }
    void set_pattern_matching(DOJO::PatternMatching p_pm) { pattern_matching = p_pm; }

    TypedArray<String> get_models() const { return models; }
    void set_models(const TypedArray<String>& p_models) { models = p_models; }

    String get_model() const { return model; }
    void set_model(const String& p_model) { model = p_model; }

    String get_member() const { return member; }
    void set_member(const String& p_member) { member = p_member; }

    DOJO::ComparisonOperator get_comparison_operator() const { return comparison_operator; }
    void set_comparison_operator(DOJO::ComparisonOperator p_op) { comparison_operator = p_op; }

    DOJO::MemberValue_Tag get_member_tag() const { return member_tag; }

    void set_member_tag(DOJO::MemberValue_Tag p_tag)
    {
        member_tag = p_tag;
        notify_property_list_changed();
    }

    DOJO::Primitive_Tag get_primitive_tag() const { return primitive_tag; }
    void set_primitive_tag(DOJO::Primitive_Tag p_tag) { primitive_tag = p_tag; }

    Variant get_value() const { return value; }
    void set_value(const Variant& p_value) { value = p_value; }

    DOJO::LogicalOperator get_logical_operator() const { return logical_operator; }
    void set_logical_operator(DOJO::LogicalOperator p_op) { logical_operator = p_op; }

    Array get_clauses() const { return clauses; }
    void set_clauses(const Array& p_clauses) { clauses = p_clauses; }

protected:
    static void _bind_methods()
    {
        // Enums
        BIND_ENUM_CONSTANT(DOJO::Clause_Tag::HashedKeys);
        BIND_ENUM_CONSTANT(DOJO::Clause_Tag::Keys);
        BIND_ENUM_CONSTANT(DOJO::Clause_Tag::CMember);
        BIND_ENUM_CONSTANT(DOJO::Clause_Tag::Composite);

        BIND_ENUM_CONSTANT(DOJO::PatternMatching::FixedLen);
        BIND_ENUM_CONSTANT(DOJO::PatternMatching::VariableLen);

        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::Eq);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::Neq);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::Gt);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::Gte);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::Lt);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::Lte);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::In);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::NotIn);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::Contains);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::ContainsAll);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::ContainsAny);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::ArrayLengthEq);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::ArrayLengthGt);
        BIND_ENUM_CONSTANT(DOJO::ComparisonOperator::ArrayLengthLt);

        BIND_ENUM_CONSTANT(DOJO::MemberValue_Tag::PrimitiveValue);
        BIND_ENUM_CONSTANT(DOJO::MemberValue_Tag::String);
        BIND_ENUM_CONSTANT(DOJO::MemberValue_Tag::List);

        BIND_ENUM_CONSTANT(DOJO::LogicalOperator::And);
        BIND_ENUM_CONSTANT(DOJO::LogicalOperator::Or);

        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::I8);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::I16);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::I32);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::I64);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::I128);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::U8);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::U16);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::U32);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::U64);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::U128);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::U256_);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::Bool);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::Felt252);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::ClassHash);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::ContractAddress);
        BIND_ENUM_CONSTANT(DOJO::Primitive_Tag::EthAddress);

        // Tag
        ClassDB::bind_method(D_METHOD("get_tag"), &DojoOptionClause::get_tag);
        ClassDB::bind_method(D_METHOD("set_tag", "p_tag"), &DojoOptionClause::set_tag);

        // HashedKeys
        ClassDB::bind_method(D_METHOD("get_hashed_keys"), &DojoOptionClause::get_hashed_keys);
        ClassDB::bind_method(D_METHOD("set_hashed_keys", "p_keys"), &DojoOptionClause::set_hashed_keys);

        // Keys
        ClassDB::bind_method(D_METHOD("get_keys"), &DojoOptionClause::get_keys);
        ClassDB::bind_method(D_METHOD("set_keys", "p_keys"), &DojoOptionClause::set_keys);
        ClassDB::bind_method(D_METHOD("get_pattern_matching"), &DojoOptionClause::get_pattern_matching);
        ClassDB::bind_method(D_METHOD("set_pattern_matching", "p_pm"), &DojoOptionClause::set_pattern_matching);
        ClassDB::bind_method(D_METHOD("get_models"), &DojoOptionClause::get_models);
        ClassDB::bind_method(D_METHOD("set_models", "p_models"), &DojoOptionClause::set_models);

        // Member
        ClassDB::bind_method(D_METHOD("get_model"), &DojoOptionClause::get_model);
        ClassDB::bind_method(D_METHOD("set_model", "p_model"), &DojoOptionClause::set_model);
        ClassDB::bind_method(D_METHOD("get_member"), &DojoOptionClause::get_member);
        ClassDB::bind_method(D_METHOD("set_member", "p_member"), &DojoOptionClause::set_member);
        ClassDB::bind_method(D_METHOD("get_comparison_operator"), &DojoOptionClause::get_comparison_operator);
        ClassDB::bind_method(D_METHOD("set_comparison_operator", "p_op"), &DojoOptionClause::set_comparison_operator);
        ClassDB::bind_method(D_METHOD("get_member_tag"), &DojoOptionClause::get_member_tag);
        ClassDB::bind_method(D_METHOD("set_member_tag", "p_tag"), &DojoOptionClause::set_member_tag);
        ClassDB::bind_method(D_METHOD("get_primitive_tag"), &DojoOptionClause::get_primitive_tag);
        ClassDB::bind_method(D_METHOD("set_primitive_tag", "p_tag"), &DojoOptionClause::set_primitive_tag);
        ClassDB::bind_method(D_METHOD("get_value"), &DojoOptionClause::get_value);
        ClassDB::bind_method(D_METHOD("set_value", "p_value"), &DojoOptionClause::set_value);

        // Composite
        ClassDB::bind_method(D_METHOD("get_logical_operator"), &DojoOptionClause::get_logical_operator);
        ClassDB::bind_method(D_METHOD("set_logical_operator", "p_op"), &DojoOptionClause::set_logical_operator);
        ClassDB::bind_method(D_METHOD("get_clauses"), &DojoOptionClause::get_clauses);
        ClassDB::bind_method(D_METHOD("set_clauses", "p_clauses"), &DojoOptionClause::set_clauses);

        // Overridden from DojoOption
        ClassDB::bind_method(D_METHOD("to_json"), &DojoOptionClause::to_json);
    }

    void _get_property_list(List<PropertyInfo>* p_list) const
    {
        // p_list->push_back(PropertyInfo(Variant::BOOL, "is_some"));
        // if (!is_some()) return;

        p_list->push_back(PropertyInfo(Variant::INT, "tag", PROPERTY_HINT_ENUM, "HashedKeys,Keys,Member,Composite"));

        switch (tag)
        {
        case DOJO::Clause_Tag::HashedKeys:
            p_list->push_back(PropertyInfo(Variant::PACKED_STRING_ARRAY, "hashed_keys"));
            break;
        case DOJO::Clause_Tag::Keys:
            p_list->push_back(PropertyInfo(Variant::ARRAY, "keys"));
            p_list->push_back(
                PropertyInfo(Variant::INT, "pattern_matching", PROPERTY_HINT_ENUM, "FixedLen,VariableLen"));
            p_list->push_back(PropertyInfo(Variant::PACKED_STRING_ARRAY, "models"));
            break;
        case DOJO::Clause_Tag::CMember:
            p_list->push_back(PropertyInfo(Variant::STRING, "model"));
            p_list->push_back(PropertyInfo(Variant::STRING, "member"));
            p_list->push_back(PropertyInfo(Variant::INT, "comparison_operator", PROPERTY_HINT_ENUM,
                                           "Eq,Neq,Gt,Gte,Lt,Lte,In,NotIn,Contains,ContainsAll,ContainsAny,ArrayLengthEq,ArrayLengthGt,ArrayLengthLt"));
            p_list->push_back(
                PropertyInfo(Variant::INT, "member_tag", PROPERTY_HINT_ENUM, "PrimitiveValue,String,List"));
            if (member_tag == DOJO::MemberValue_Tag::PrimitiveValue || member_tag == DOJO::MemberValue_Tag::List)
            {
                p_list->push_back(PropertyInfo(Variant::INT, "primitive_tag", PROPERTY_HINT_ENUM,
                                               "I8,I16,I32,I64,I128,U8,U16,U32,U64,U128,U256,Bool,Felt252,ClassHash,ContractAddress,EthAddress"));
            }
            p_list->push_back(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "",
                                           PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_NIL_IS_VARIANT));
            break;
        case DOJO::Clause_Tag::Composite:
            p_list->push_back(PropertyInfo(Variant::INT, "logical_operator", PROPERTY_HINT_ENUM, "And,Or"));
            p_list->push_back(PropertyInfo(Variant::ARRAY, "clauses"));
            break;
        }
    }

    bool _set(const StringName& p_name, const Variant& p_value)
    {
        // if (!is_some()) return false;

        if (String(p_name) == "tag")
        {
            set_tag((DOJO::Clause_Tag)(int)p_value);
            return true;
        }

        switch (tag)
        {
        case DOJO::Clause_Tag::HashedKeys:
            if (String(p_name) == "hashed_keys")
            {
                set_hashed_keys(p_value);
                return true;
            }
            break;
        case DOJO::Clause_Tag::Keys:
            if (String(p_name) == "keys")
            {
                set_keys(p_value);
                return true;
            }
            if (String(p_name) == "pattern_matching")
            {
                set_pattern_matching((DOJO::PatternMatching)(int)p_value);
                return true;
            }
            if (String(p_name) == "models")
            {
                set_models(p_value);
                return true;
            }
            break;
        case DOJO::Clause_Tag::CMember:
            if (String(p_name) == "model")
            {
                set_model(p_value);
                return true;
            }
            if (String(p_name) == "member")
            {
                set_member(p_value);
                return true;
            }
            if (String(p_name) == "comparison_operator")
            {
                set_comparison_operator((DOJO::ComparisonOperator)(int)p_value);
                return true;
            }
            if (String(p_name) == "member_tag")
            {
                set_member_tag((DOJO::MemberValue_Tag)(int)p_value);
                return true;
            }
            if (String(p_name) == "primitive_tag")
            {
                set_primitive_tag((DOJO::Primitive_Tag)(int)p_value);
                return true;
            }
            if (String(p_name) == "value")
            {
                set_value(p_value);
                return true;
            }
            break;
        case DOJO::Clause_Tag::Composite:
            if (String(p_name) == "logical_operator")
            {
                set_logical_operator((DOJO::LogicalOperator)(int)p_value);
                return true;
            }
            if (String(p_name) == "clauses")
            {
                set_clauses(p_value);
                return true;
            }
            break;
        }
        return false;
    }

    bool _get(const StringName& p_name, Variant& r_ret) const
    {

        // if (!is_some()) return false;

        if (String(p_name) == "tag")
        {
            r_ret = get_tag();
            return true;
        }

        switch (tag)
        {
        case DOJO::Clause_Tag::HashedKeys:
            if (String(p_name) == "hashed_keys")
            {
                r_ret = get_hashed_keys();
                return true;
            }
            break;
        case DOJO::Clause_Tag::Keys:
            if (String(p_name) == "keys")
            {
                r_ret = get_keys();
                return true;
            }
            if (String(p_name) == "pattern_matching")
            {
                r_ret = get_pattern_matching();
                return true;
            }
            if (String(p_name) == "models")
            {
                r_ret = get_models();
                return true;
            }
            break;
        case DOJO::Clause_Tag::CMember:
            if (String(p_name) == "model")
            {
                r_ret = get_model();
                return true;
            }
            if (String(p_name) == "member")
            {
                r_ret = get_member();
                return true;
            }
            if (String(p_name) == "comparison_operator")
            {
                r_ret = get_comparison_operator();
                return true;
            }
            if (String(p_name) == "member_tag")
            {
                r_ret = get_member_tag();
                return true;
            }
            if (String(p_name) == "primitive_tag")
            {
                r_ret = get_primitive_tag();
                return true;
            }
            if (String(p_name) == "value")
            {
                r_ret = get_value();
                return true;
            }
            break;
        case DOJO::Clause_Tag::Composite:
            if (String(p_name) == "logical_operator")
            {
                r_ret = get_logical_operator();
                return true;
            }
            if (String(p_name) == "clauses")
            {
                r_ret = get_clauses();
                return true;
            }
            break;
        }
        return false;
    }
};

VARIANT_ENUM_CAST(DOJO::Clause_Tag);

VARIANT_ENUM_CAST(DOJO::PatternMatching);

VARIANT_ENUM_CAST(DOJO::ComparisonOperator);

VARIANT_ENUM_CAST(DOJO::MemberValue_Tag);

VARIANT_ENUM_CAST(DOJO::LogicalOperator);

VARIANT_ENUM_CAST(DOJO::Primitive_Tag);

#endif //OPTION_CLAUSE_H
