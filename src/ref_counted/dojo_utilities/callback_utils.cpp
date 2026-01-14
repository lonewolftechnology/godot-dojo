#include "ref_counted/dojo_utilities/callback_utils.hpp"

// Helper para std::visit
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

Dictionary CallbackUtils::entity_to_dict(const dojo::Entity& entity) {
    Dictionary dict;
    dict["world_address"] = String(entity.world_address.c_str());
    dict["hashed_keys"] = String(entity.hashed_keys.c_str());
    dict["created_at"] = entity.created_at;
    dict["updated_at"] = entity.updated_at;
    dict["executed_at"] = entity.executed_at;

    Dictionary models;
    for (const auto& model : entity.models) {
        if (model) {
            models[String(model->name.c_str())] = struct_to_dict(model);
        }
    }
    dict["models"] = models;

    return dict;
}

Dictionary CallbackUtils::event_to_dict(const dojo::Event& event) {
    Dictionary dict;
    dict["transaction_hash"] = String(event.transaction_hash.c_str());
    
    Array keys;
    for (const auto& k : event.keys) keys.append(String(k.c_str()));
    dict["keys"] = keys;

    Array data;
    for (const auto& d : event.data) data.append(String(d.c_str()));
    dict["data"] = data;

    return dict;
}

Dictionary CallbackUtils::token_balance_to_dict(const dojo::TokenBalance& balance) {
    Dictionary dict;
    dict["balance"] = String(balance.balance.c_str());
    dict["account_address"] = String(balance.account_address.c_str());
    dict["contract_address"] = String(balance.contract_address.c_str());
    if (balance.token_id.has_value()) {
        dict["token_id"] = String(balance.token_id.value().c_str());
    } else {
        dict["token_id"] = Variant();
    }
    return dict;
}

Dictionary CallbackUtils::token_to_dict(const dojo::Token& token) {
    Dictionary dict;
    dict["contract_address"] = String(token.contract_address.c_str());
    if (token.token_id.has_value()) {
        dict["token_id"] = String(token.token_id.value().c_str());
    }
    dict["name"] = String(token.name.c_str());
    dict["symbol"] = String(token.symbol.c_str());
    dict["decimals"] = token.decimals;
    dict["metadata"] = String(token.metadata.c_str());
    if (token.total_supply.has_value()) {
        dict["total_supply"] = String(token.total_supply.value().c_str());
    }
    return dict;
}

Dictionary CallbackUtils::transaction_to_dict(const dojo::Transaction& transaction) {
    Dictionary dict;
    dict["transaction_hash"] = String(transaction.transaction_hash.c_str());
    dict["sender_address"] = String(transaction.sender_address.c_str());
    dict["max_fee"] = String(transaction.max_fee.c_str());
    dict["nonce"] = String(transaction.nonce.c_str());
    dict["block_number"] = transaction.block_number;
    dict["transaction_type"] = String(transaction.transaction_type.c_str());
    dict["block_timestamp"] = transaction.block_timestamp;
    return dict;
}

Variant CallbackUtils::ty_to_variant(const std::shared_ptr<dojo::Ty>& ty) {
    if (!ty) return Variant();

    return std::visit(overloaded {
        [](const dojo::Ty::kPrimitive& v) { return primitive_to_variant(v.value); },
        [](const dojo::Ty::kStruct& v) { return Variant(struct_to_dict(v.value)); },
        [](const dojo::Ty::kEnum& v) { return Variant(enum_to_dict(v.value)); },
        [](const dojo::Ty::kTuple& v) {
            Array arr;
            for (const auto& item : v.values) arr.append(ty_to_variant(item));
            return Variant(arr);
        },
        [](const dojo::Ty::kArray& v) {
            Array arr;
            for (const auto& item : v.values) arr.append(ty_to_variant(item));
            return Variant(arr);
        },
        [](const dojo::Ty::kFixedSizeArray& v) {
            Array arr;
            if (v.value) {
                for (const auto& item : v.value->array) arr.append(ty_to_variant(item));
            }
            return Variant(arr);
        },
        [](const dojo::Ty::kByteArray& v) { return Variant(String(v.value.c_str())); }
    }, ty->get_variant());
}

Variant CallbackUtils::primitive_to_variant(const std::shared_ptr<dojo::Primitive>& primitive) {
    if (!primitive) return Variant();

    return std::visit(overloaded {
        [](const dojo::Primitive::kI8& v) { return Variant(v.value); },
        [](const dojo::Primitive::kI16& v) { return Variant(v.value); },
        [](const dojo::Primitive::kI32& v) { return Variant(v.value); },
        [](const dojo::Primitive::kI64& v) { return Variant(v.value); },
        [](const dojo::Primitive::kI128& v) { 
             PackedByteArray bytes;
             bytes.resize(v.value.size());
             memcpy(bytes.ptrw(), v.value.data(), v.value.size());
             return Variant(bytes);
        },
        [](const dojo::Primitive::kU8& v) { return Variant(v.value); },
        [](const dojo::Primitive::kU16& v) { return Variant(v.value); },
        [](const dojo::Primitive::kU32& v) { return Variant(v.value); },
        [](const dojo::Primitive::kU64& v) { return Variant(v.value); },
        [](const dojo::Primitive::kU128& v) {
             PackedByteArray bytes;
             bytes.resize(v.value.size());
             memcpy(bytes.ptrw(), v.value.data(), v.value.size());
             return Variant(bytes);
        },
        [](const dojo::Primitive::kU256& v) { return Variant(String(v.value.c_str())); },
        [](const dojo::Primitive::kBool& v) { return Variant(v.value); },
        [](const dojo::Primitive::kFelt252& v) { return Variant(String(v.value.c_str())); },
        [](const dojo::Primitive::kClassHash& v) { return Variant(String(v.value.c_str())); },
        [](const dojo::Primitive::kContractAddress& v) { return Variant(String(v.value.c_str())); },
        [](const dojo::Primitive::kEthAddress& v) { return Variant(String(v.value.c_str())); }
    }, primitive->get_variant());
}

Dictionary CallbackUtils::struct_to_dict(const std::shared_ptr<dojo::Struct>& struct_ptr) {
    Dictionary dict;
    if (!struct_ptr) return dict;
    
    for (const auto& member : struct_ptr->children) {
        if (member) {
            dict[String(member->name.c_str())] = ty_to_variant(member->ty);
        }
    }
    return dict;
}

Dictionary CallbackUtils::enum_to_dict(const std::shared_ptr<dojo::EnumType>& enum_ptr) {
    Dictionary dict;
    if (!enum_ptr) return dict;

    dict["name"] = String(enum_ptr->name.c_str());
    dict["option_index"] = enum_ptr->option;
    
    if (enum_ptr->option < enum_ptr->options.size()) {
        auto opt = enum_ptr->options[enum_ptr->option];
        if (opt) {
            dict["option"] = String(opt->name.c_str());
            if (opt->ty) {
                dict["value"] = ty_to_variant(opt->ty);
            }
        }
    }
    return dict;
}