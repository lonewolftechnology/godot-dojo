//
// Created by hazel on 1/08/25.
//

#ifndef OPTION_CLAUSE_H
#define OPTION_CLAUSE_H

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "dojo_types.h"
#include "option_field_element.h"
#include "tools/dojo_helper.h"

using namespace godot;

class OptionClause : public DojoOption
{
    GDCLASS(OptionClause, DojoOption);

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

    static DOJO::Primitive to_native_primitive(const Variant& p_value, DOJO::Primitive_Tag p_tag) {
        DOJO::Primitive primitive = {};
        primitive.tag = p_tag;
        String str_val = p_value;

        switch (p_tag) {
            case DOJO::Primitive_Tag::I8: DojoHelpers::string_to_i8(str_val, &primitive.i8); break;
            case DOJO::Primitive_Tag::I16: DojoHelpers::string_to_i16(str_val, &primitive.i16); break;
            case DOJO::Primitive_Tag::I32: DojoHelpers::string_to_i32(str_val, &primitive.i32); break;
            case DOJO::Primitive_Tag::I64: DojoHelpers::string_to_i64(str_val, &primitive.i64); break;
            case DOJO::Primitive_Tag::I128: DojoHelpers::string_to_i128(str_val, primitive.i128); break;
            case DOJO::Primitive_Tag::U8: primitive.u8 = static_cast<uint8_t>(str_val.to_int()); break;
            case DOJO::Primitive_Tag::U16: primitive.u16 = static_cast<uint16_t>(str_val.to_int()); break;
            case DOJO::Primitive_Tag::U32: primitive.u32 = static_cast<uint32_t>(str_val.to_int()); break;
            case DOJO::Primitive_Tag::U64: primitive.u64 = str_val.to_int(); break;
            case DOJO::Primitive_Tag::U128: DojoHelpers::string_to_u128(str_val, primitive.u128); break;
            case DOJO::Primitive_Tag::U256_: primitive.u256 = DojoHelpers::string_to_u256(str_val); break;
            case DOJO::Primitive_Tag::Bool: primitive.bool_ = static_cast<bool>(p_value); break;
            case DOJO::Primitive_Tag::Felt252:
            case DOJO::Primitive_Tag::ClassHash:
            case DOJO::Primitive_Tag::ContractAddress:
            case DOJO::Primitive_Tag::EthAddress: {
                DOJO::U256 u256_val = DojoHelpers::string_to_u256(str_val);
                memcpy(primitive.felt252.data, u256_val.data, 32);
                break;
            }
        }
        return primitive;
    }

    static DOJO::MemberValue to_native_member_value(const Variant& p_value, DOJO::MemberValue_Tag p_member_tag, DOJO::Primitive_Tag p_primitive_tag) {
        DOJO::MemberValue native_value = {};
        native_value.tag = p_member_tag;

        switch (p_member_tag) {
            case DOJO::MemberValue_Tag::PrimitiveValue:
                native_value.primitive_value = to_native_primitive(p_value, p_primitive_tag);
                break;
            case DOJO::MemberValue_Tag::String: {
                String str = p_value;
                char* c_str = new char[str.utf8().length() + 1];
                strcpy(c_str, str.utf8().get_data());
                native_value.string = c_str;
                break;
            }
            case DOJO::MemberValue_Tag::List: {
                Array arr = p_value;
                auto* native_list = new DOJO::MemberValue[arr.size()];
                for (int i = 0; i < arr.size(); ++i) {
                    native_list[i].tag = DOJO::MemberValue_Tag::PrimitiveValue;
                    native_list[i].primitive_value = to_native_primitive(arr[i], p_primitive_tag);
                }
                native_value.list = {native_list, (uintptr_t)arr.size()};
                break;
            }
        }
        return native_value;
    }

public:
    OptionClause() {
        tag = DOJO::Clause_Tag::HashedKeys;
        pattern_matching = DOJO::PatternMatching::FixedLen;
        comparison_operator = DOJO::ComparisonOperator::Eq;
        member_tag = DOJO::MemberValue_Tag::PrimitiveValue;
        primitive_tag = DOJO::Primitive_Tag::U256_;
        logical_operator = DOJO::LogicalOperator::And;
    }

    DOJO::COptionClause get_native_option() const {
        DOJO::COptionClause option = {};
        if (!is_some()) {
            option.tag = DOJO::NoneClause;
            return option;
        }

        option.tag = DOJO::SomeClause;
        option.some.tag = tag;

        switch (tag) {
            case DOJO::Clause_Tag::HashedKeys: {
                auto* native_hashed_keys = new DOJO::FieldElement[hashed_keys.size()];
                for (int i = 0; i < hashed_keys.size(); ++i) {
                    String key_str = hashed_keys[i];
                    DOJO::U256 u256_val = DojoHelpers::string_to_u256(key_str);
                    memcpy(native_hashed_keys[i].data, u256_val.data, 32);
                }
                option.some.hashed_keys = {native_hashed_keys, (uintptr_t)hashed_keys.size()};
                break;
            }
            case DOJO::Clause_Tag::Keys: {
                auto* native_keys = new DOJO::COptionFieldElement[keys.size()];
                for (int i = 0; i < keys.size(); ++i) {
                    Ref<OptionFieldElement> key_ref = keys[i];
                    if (key_ref.is_valid()) {
                        native_keys[i] = key_ref->get_native_option();
                    } else {
                        native_keys[i].tag = DOJO::NoneFieldElement;
                    }
                }

                const char** models_data = new const char*[models.size()];
                for (int i = 0; i < models.size(); ++i) {
                    String model_str = models[i];
                    char *c_str = new char[model_str.utf8().length() + 1];
                    strcpy(c_str, model_str.utf8().get_data());
                    models_data[i] = c_str;
                }

                option.some.keys = {
                    {native_keys, (uintptr_t)keys.size()},
                    pattern_matching,
                    {models_data, (uintptr_t)models.size()}
                };
                break;
            }
            case DOJO::Clause_Tag::CMember: {
                option.some.c_member.model = model.utf8().get_data();
                option.some.c_member.member = member.utf8().get_data();
                option.some.c_member.operator_ = comparison_operator;
                option.some.c_member.value = to_native_member_value(value, member_tag, primitive_tag);
                break;
            }
            case DOJO::Clause_Tag::Composite: {
                auto* native_clauses = new DOJO::Clause[clauses.size()];
                for (int i = 0; i < clauses.size(); ++i) {
                    Ref<OptionClause> clause_ref = clauses[i];
                    if (clause_ref.is_valid()) {
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

    DOJO::Clause get_native_clause() const {
        return get_native_option().some;
    }


    DOJO::Clause_Tag get_tag() const { return tag; }
    void set_tag(DOJO::Clause_Tag p_type) { tag = p_type; notify_property_list_changed(); }

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
    void set_member_tag(DOJO::MemberValue_Tag p_tag) { member_tag = p_tag; notify_property_list_changed(); }

    DOJO::Primitive_Tag get_primitive_tag() const { return primitive_tag; }
    void set_primitive_tag(DOJO::Primitive_Tag p_tag) { primitive_tag = p_tag; }

    Variant get_value() const { return value; }
    void set_value(const Variant& p_value) { value = p_value; }

    DOJO::LogicalOperator get_logical_operator() const { return logical_operator; }
    void set_logical_operator(DOJO::LogicalOperator p_op) { logical_operator = p_op; }

    Array get_clauses() const { return clauses; }
    void set_clauses(const Array& p_clauses) { clauses = p_clauses; }

protected:
    static void _bind_methods() {
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
        ClassDB::bind_method(D_METHOD("get_tag"), &OptionClause::get_tag);
        ClassDB::bind_method(D_METHOD("set_tag", "p_tag"), &OptionClause::set_tag);

        // HashedKeys
        ClassDB::bind_method(D_METHOD("get_hashed_keys"), &OptionClause::get_hashed_keys);
        ClassDB::bind_method(D_METHOD("set_hashed_keys", "p_keys"), &OptionClause::set_hashed_keys);

        // Keys
        ClassDB::bind_method(D_METHOD("get_keys"), &OptionClause::get_keys);
        ClassDB::bind_method(D_METHOD("set_keys", "p_keys"), &OptionClause::set_keys);
        ClassDB::bind_method(D_METHOD("get_pattern_matching"), &OptionClause::get_pattern_matching);
        ClassDB::bind_method(D_METHOD("set_pattern_matching", "p_pm"), &OptionClause::set_pattern_matching);
        ClassDB::bind_method(D_METHOD("get_models"), &OptionClause::get_models);
        ClassDB::bind_method(D_METHOD("set_models", "p_models"), &OptionClause::set_models);

        // Member
        ClassDB::bind_method(D_METHOD("get_model"), &OptionClause::get_model);
        ClassDB::bind_method(D_METHOD("set_model", "p_model"), &OptionClause::set_model);
        ClassDB::bind_method(D_METHOD("get_member"), &OptionClause::get_member);
        ClassDB::bind_method(D_METHOD("set_member", "p_member"), &OptionClause::set_member);
        ClassDB::bind_method(D_METHOD("get_comparison_operator"), &OptionClause::get_comparison_operator);
        ClassDB::bind_method(D_METHOD("set_comparison_operator", "p_op"), &OptionClause::set_comparison_operator);
        ClassDB::bind_method(D_METHOD("get_member_tag"), &OptionClause::get_member_tag);
        ClassDB::bind_method(D_METHOD("set_member_tag", "p_tag"), &OptionClause::set_member_tag);
        ClassDB::bind_method(D_METHOD("get_primitive_tag"), &OptionClause::get_primitive_tag);
        ClassDB::bind_method(D_METHOD("set_primitive_tag", "p_tag"), &OptionClause::set_primitive_tag);
        ClassDB::bind_method(D_METHOD("get_value"), &OptionClause::get_value);
        ClassDB::bind_method(D_METHOD("set_value", "p_value"), &OptionClause::set_value);

        // Composite
        ClassDB::bind_method(D_METHOD("get_logical_operator"), &OptionClause::get_logical_operator);
        ClassDB::bind_method(D_METHOD("set_logical_operator", "p_op"), &OptionClause::set_logical_operator);
        ClassDB::bind_method(D_METHOD("get_clauses"), &OptionClause::get_clauses);
        ClassDB::bind_method(D_METHOD("set_clauses", "p_clauses"), &OptionClause::set_clauses);
    }

    void _get_property_list(List<PropertyInfo>* p_list) const {
        // p_list->push_back(PropertyInfo(Variant::BOOL, "is_some"));
        // if (!is_some()) return;

        p_list->push_back(PropertyInfo(Variant::INT, "tag", PROPERTY_HINT_ENUM, "HashedKeys,Keys,Member,Composite"));

        switch (tag) {
            case DOJO::Clause_Tag::HashedKeys:
                p_list->push_back(PropertyInfo(Variant::PACKED_STRING_ARRAY, "hashed_keys"));
                break;
            case DOJO::Clause_Tag::Keys:
                p_list->push_back(PropertyInfo(Variant::ARRAY, "keys"));
                p_list->push_back(PropertyInfo(Variant::INT, "pattern_matching", PROPERTY_HINT_ENUM, "FixedLen,VariableLen"));
                p_list->push_back(PropertyInfo(Variant::PACKED_STRING_ARRAY, "models"));
                break;
            case DOJO::Clause_Tag::CMember:
                p_list->push_back(PropertyInfo(Variant::STRING, "model"));
                p_list->push_back(PropertyInfo(Variant::STRING, "member"));
                p_list->push_back(PropertyInfo(Variant::INT, "comparison_operator", PROPERTY_HINT_ENUM, "Eq,Neq,Gt,Gte,Lt,Lte,In,NotIn"));
                p_list->push_back(PropertyInfo(Variant::INT, "member_tag", PROPERTY_HINT_ENUM, "PrimitiveValue,String,List"));
                if (member_tag == DOJO::MemberValue_Tag::PrimitiveValue || member_tag == DOJO::MemberValue_Tag::List) {
                    p_list->push_back(PropertyInfo(Variant::INT, "primitive_tag", PROPERTY_HINT_ENUM, "I8,I16,I32,I64,I128,U8,U16,U32,U64,U128,U256,Bool,Felt252,ClassHash,ContractAddress,EthAddress"));
                }
                p_list->push_back(PropertyInfo(Variant::NIL, "value", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_NIL_IS_VARIANT));
                break;
            case DOJO::Clause_Tag::Composite:
                p_list->push_back(PropertyInfo(Variant::INT, "logical_operator", PROPERTY_HINT_ENUM, "And,Or"));
                p_list->push_back(PropertyInfo(Variant::ARRAY, "clauses"));
                break;
        }
    }

    bool _set(const StringName& p_name, const Variant& p_value) {

        // if (!is_some()) return false;

        if (String(p_name) == "tag")
        {
            set_tag((DOJO::Clause_Tag)(int)p_value);
            return true;
        }

        switch (tag) {
            case DOJO::Clause_Tag::HashedKeys:
                if (String(p_name) == "hashed_keys") { set_hashed_keys(p_value); return true; }
                break;
            case DOJO::Clause_Tag::Keys:
                if (String(p_name) == "keys") { set_keys(p_value); return true; }
                if (String(p_name) == "pattern_matching") { set_pattern_matching((DOJO::PatternMatching)(int)p_value); return true; }
                if (String(p_name) == "models") { set_models(p_value); return true; }
                break;
            case DOJO::Clause_Tag::CMember:
                if (String(p_name) == "model") { set_model(p_value); return true; }
                if (String(p_name) == "member") { set_member(p_value); return true; }
                if (String(p_name) == "comparison_operator") { set_comparison_operator((DOJO::ComparisonOperator)(int)p_value); return true; }
                if (String(p_name) == "member_tag") { set_member_tag((DOJO::MemberValue_Tag)(int)p_value); return true; }
                if (String(p_name) == "primitive_tag") { set_primitive_tag((DOJO::Primitive_Tag)(int)p_value); return true; }
                if (String(p_name) == "value") { set_value(p_value); return true; }
                break;
            case DOJO::Clause_Tag::Composite:
                if (String(p_name) == "logical_operator") { set_logical_operator((DOJO::LogicalOperator)(int)p_value); return true; }
                if (String(p_name) == "clauses") { set_clauses(p_value); return true; }
                break;
        }
        return false;
    }

    bool _get(const StringName& p_name, Variant& r_ret) const {
        if (String(p_name) == "is_some") {
            r_ret = is_some();
            return true;
        }

        // if (!is_some()) return false;

        if (String(p_name) == "tag") { r_ret = get_tag(); return true; }

        switch (tag) {
            case DOJO::Clause_Tag::HashedKeys:
                if (String(p_name) == "hashed_keys") { r_ret = get_hashed_keys(); return true; }
                break;
            case DOJO::Clause_Tag::Keys:
                if (String(p_name) == "keys") { r_ret = get_keys(); return true; }
                if (String(p_name) == "pattern_matching") { r_ret = get_pattern_matching(); return true; }
                if (String(p_name) == "models") { r_ret = get_models(); return true; }
                break;
            case DOJO::Clause_Tag::CMember:
                if (String(p_name) == "model") { r_ret = get_model(); return true; }
                if (String(p_name) == "member") { r_ret = get_member(); return true; }
                if (String(p_name) == "comparison_operator") { r_ret = get_comparison_operator(); return true; }
                if (String(p_name) == "member_tag") { r_ret = get_member_tag(); return true; }
                if (String(p_name) == "primitive_tag") { r_ret = get_primitive_tag(); return true; }
                if (String(p_name) == "value") { r_ret = get_value(); return true; }
                break;
            case DOJO::Clause_Tag::Composite:
                if (String(p_name) == "logical_operator") { r_ret = get_logical_operator(); return true; }
                if (String(p_name) == "clauses") { r_ret = get_clauses(); return true; }
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
