#include "ref_counted/dojo_utilities/clauses/member.hpp"
#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

MemberClause::MemberClause() : DojoClause(DojoClause::Member) {}
MemberClause::~MemberClause() {}

Dictionary MemberClause::to_dict() const {
    Dictionary result;
    result["model"] = p_model;
    result["member"] = p_member;
    result["operator"] = (int)p_operator;
    result["value"] = p_value;
    return result;
}

// Recursive helper to convert Variant (Array/String/Primitive) to dojo::MemberValue (C++)
std::shared_ptr<dojo::MemberValue> variant_to_member_value(const Variant &val, MemberClause::PrimitiveTag prim_tag) {
    if (val.get_type() == Variant::ARRAY) {
        Array arr = val;
        std::vector<std::shared_ptr<dojo::MemberValue> > list;
        list.reserve(arr.size());
        for (int i = 0; i < arr.size(); ++i) {
            list.push_back(variant_to_member_value(arr[i], prim_tag));
        }
        return std::make_shared<dojo::MemberValue>(dojo::MemberValue::kList{list});
    }

    bool is_primitive_hint = (prim_tag >= MemberClause::I8 && prim_tag <= MemberClause::EthAddress);

    if (val.get_type() == Variant::STRING && !is_primitive_hint) {
        std::string s = String(val).utf8().get_data();
        return std::make_shared<dojo::MemberValue>(dojo::MemberValue::kString{s});
    }

    // Otherwise try to convert to primitive
    dojo::Primitive p = MemberClause::to_native_primitive(val, prim_tag);
    return std::make_shared<dojo::MemberValue>(dojo::MemberValue::kPrimitive{std::make_shared<dojo::Primitive>(p)});
}

dojo::Clause MemberClause::get_native() const {
    dojo::MemberClause clause;
    clause.member = p_member.utf8().get_data();
    clause.model = p_model.utf8().get_data();

    clause.operator_ = p_operator;

    std::shared_ptr<dojo::MemberValue> val;

    if (p_value.get_type() == Variant::ARRAY) {
        val = variant_to_member_value(p_value, p_primitive_tag);
    } else if (p_value.get_type() == Variant::STRING && p_primitive_tag == Nil) {
        std::string s = static_cast<String>(p_value).utf8().get_data();
        val = std::make_shared<dojo::MemberValue>(dojo::MemberValue::kString{s});
    } else {
        // Primitive (explicit tag or implicit default)
        PrimitiveTag tag = p_primitive_tag;
        if (tag == Nil) {
            // Fallback defaults if not set explicitly
            if (p_value.get_type() == Variant::BOOL) tag = Bool;
            else if (p_value.get_type() == Variant::INT) tag = I64;
            else if (p_value.get_type() == Variant::STRING) tag = Felt252;
        }
        dojo::Primitive p = to_native_primitive(p_value, tag);
        val = std::make_shared<dojo::MemberValue>(dojo::MemberValue::kPrimitive{std::make_shared<dojo::Primitive>(p)});
    }
    clause.value = val;
    return dojo::Clause(dojo::Clause::kMember{std::make_shared<dojo::MemberClause>(clause)});
}

bool MemberClause::is_value_set() const {
    return p_value.get_type() != Variant::NIL;
}

Ref<MemberClause> MemberClause::string(const String &value) {
    this->p_value = value;
    return this;
}

Ref<MemberClause> MemberClause::list(const Variant **args, GDExtensionInt arg_count, GDExtensionCallError &error) {
    Array array;
    for (int i = 0; i < arg_count; i++) {
        array.append(*args[i]);
    }
    this->p_value = array;
    return this;
}

Ref<MemberClause> MemberClause::int_(const int &value, PrimitiveTag tag) {
    this->p_value = value;
    if (tag == Nil) {
        this->p_primitive_tag = I64;
    } else {
        this->p_primitive_tag = tag;
    }
    return this;
}

Ref<MemberClause> MemberClause::bool_(const bool &value) {
    this->p_value = value;
    this->p_primitive_tag = Bool;
    return this;
}

Ref<MemberClause> MemberClause::hex(const String &value, PrimitiveTag tag) {
    this->p_value = value;
    if (tag == Nil) {
        this->p_primitive_tag = Felt252;
    } else {
        this->p_primitive_tag = tag;
    }
    return this;
}

Ref<MemberClause> MemberClause::model(const Variant &model) {
    this->p_model = model;
    return this;
}

Ref<MemberClause> MemberClause::op(const int &operator_) {
    this->p_operator = static_cast<dojo::ComparisonOperator>(operator_);
    return this;
}

Ref<MemberClause> MemberClause::member(const Variant &model) {
    this->p_member = model;
    return this;
}

dojo::Primitive MemberClause::to_native_primitive(const Variant &p_value, PrimitiveTag p_tag) {
    switch (p_tag) {
        case I8: {
            return dojo::Primitive(dojo::Primitive::kI8{static_cast<int8_t>(p_value)});
        }
        case I16: {
            return dojo::Primitive(dojo::Primitive::kI16{static_cast<int16_t>(p_value)});
        }
        case I32: {
            return dojo::Primitive(dojo::Primitive::kI32{static_cast<int32_t>(p_value)});
        }
        case I64: {
            return dojo::Primitive(dojo::Primitive::kI64{static_cast<int64_t>(p_value)});
        }
        case I128: {
            String str_val = p_value;
            int128_t val(str_val.utf8().get_data());
            std::vector<uint8_t> vec;
            export_bits(val, std::back_inserter(vec), 8);
            return dojo::Primitive(dojo::Primitive::kI128{vec});
        }
        case U8: return dojo::Primitive(dojo::Primitive::kU8{static_cast<uint8_t>(p_value)});
        case U16: return dojo::Primitive(dojo::Primitive::kU16{static_cast<uint16_t>(p_value)});
        case U32: return dojo::Primitive(dojo::Primitive::kU32{static_cast<uint32_t>(p_value)});
        case U64: return dojo::Primitive(dojo::Primitive::kU64{static_cast<uint64_t>(p_value)});
        case U128: {
            String str_val = p_value;
            uint128_t val(str_val.utf8().get_data());
            std::vector<uint8_t> vec;
            export_bits(val, std::back_inserter(vec), 8);
            return dojo::Primitive(dojo::Primitive::kU128{vec});
        }
        case U256: {
            String str_val = p_value;
            return dojo::Primitive(dojo::Primitive::kU256{str_val.utf8().get_data()});
        }
        case Bool: return dojo::Primitive(dojo::Primitive::kBool{static_cast<bool>(p_value)});

        case Felt252: {
            String str_val = p_value;
            return dojo::Primitive(dojo::Primitive::kFelt252{str_val.utf8().get_data()});
        }
        case ClassHash: {
            String str_val = p_value;
            return dojo::Primitive(dojo::Primitive::kClassHash{str_val.utf8().get_data()});
        }
        case ContractAddress: {
            String str_val = p_value;
            return dojo::Primitive(dojo::Primitive::kContractAddress{str_val.utf8().get_data()});
        }
        case EthAddress: {
            String str_val = p_value;
            return dojo::Primitive(dojo::Primitive::kEthAddress{str_val.utf8().get_data()});
        }
        case Nil:
            break;
    }
    // Default fallback
    String str_val = p_value;
    return dojo::Primitive(dojo::Primitive::kFelt252{str_val.utf8().get_data()});
}
