//
// Created by hazel on 29/05/25.
//

#include "classes/account.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "tools/logger.h"
#include "tools/dojo_helper.h"

Account::Account()
{
    account = nullptr;
    provider = nullptr;
}

Account::~Account()
{
    if (account)
    {
        DOJO::account_free(account);
    }
    if (provider)
    {
        DOJO::provider_free(provider);
    }
}

void Account::start_provider()
{
    DOJO::ResultProvider res_provider = DOJO::provider_new(rpc_url.utf8().get_data());
    if (res_provider.tag == DOJO::ErrProvider)
    {
        UtilityFunctions::printerr("Error creating provider: ", GET_DOJO_ERROR(res_provider));
        return;
    }
    provider = GET_DOJO_OK(res_provider);
}

void Account::create(const String& _rpc_url, const String& address, const String& private_key)
{
    this->rpc_url = _rpc_url;
    if (!provider)
    {
        start_provider();
    }

    DOJO::FieldElement private_key_felt = FieldElement::from_string(private_key);
    DOJO::ResultAccount res_account = DOJO::account_new(provider, private_key_felt, address.utf8().get_data());

    if (res_account.tag == DOJO::ErrAccount)
    {
        UtilityFunctions::printerr("Error creating account: ", GET_DOJO_ERROR(res_account));
        return;
    }

    account = GET_DOJO_OK(res_account);
}

void Account::deploy_burner(Account* master_account, const String& signing_key)
{
    if (!master_account || !master_account->account)
    {
        Logger::error("Master account is not valid");
        return;
    }

    DOJO::FieldElement signing_key_felt = FieldElement::from_string(signing_key);
    DOJO::ResultAccount res_burner = DOJO::account_deploy_burner(master_account->provider, master_account->account,
                                                                 signing_key_felt);

    if (res_burner.tag == DOJO::ErrAccount)
    {
        UtilityFunctions::printerr("Error deploying burner: ", GET_DOJO_ERROR(res_burner));
        return;
    }

    account = GET_DOJO_OK(res_burner);
    provider = master_account->provider; // The burner uses the same provider
}

String Account::get_address() const
{
    if (!account)
    {
        return "";
    }
    DOJO::FieldElement address = DOJO::account_address(account);
    return FieldElement::get_as_string(&address);
}

String Account::get_chain_id() const
{
    if (!account)
    {
        return "";
    }
    DOJO::FieldElement chain_id = DOJO::account_chain_id(account);
    return FieldElement::get_as_string(&chain_id);
}

void Account::set_block_id(const String& block_id)
{
    if (!account)
    {
        return;
    }
    // This is a simplification. In a real scenario, you would need to handle different block_id types (hash, number, tag).
    // Assuming block tag for now.
    DOJO::BlockId block;
    block.tag = DOJO::BlockTag_;
    block.block_tag = DOJO::Latest; // Or Pending, based on the string

    DOJO::account_set_block_id(account, block);
}

String Account::get_nonce() const
{
    if (!account)
    {
        return "";
    }
    DOJO::ResultFieldElement nonce_res = DOJO::account_nonce(account);
    if (nonce_res.tag == DOJO::ErrFieldElement)
    {
        UtilityFunctions::printerr("Error getting nonce: ", GET_DOJO_ERROR(nonce_res));
        return "";
    }
    DOJO::FieldElement nonce = GET_DOJO_OK(nonce_res);
    return FieldElement::get_as_string(&nonce);
}

void Account::execute_raw(const String& to, const String& selector, const Array& args)
{
    if (!account)
    {
        Logger::error("Account is not valid");
    }

    DOJO::FieldElement contract = FieldElement::from_string(to);

    uint8_t calldata_len = args.size();

    std::string selector_str = selector.utf8().get_data();

    std::vector<DOJO::FieldElement> felts_vec;
    Logger::debug_extra("Account", "Building Call");

    DOJO::Call call = {
        contract,
        selector_str.c_str(),
    };


    if (calldata_len > 0)
    {
        Logger::debug_extra("Controller", "Building Calldata");

        Array final_args = {};

        for (int i = 0; i < calldata_len; i++)
        {
            const Variant& arg = args[i];
            Logger::custom("Calldata", Variant::get_type_name(arg.get_type()), arg.stringify());

            switch (arg.get_type())
            {
            case Variant::Type::ARRAY:
                {
                    Array v_array = static_cast<Array>(arg);
                    // final_args.push_back(v_array.size());
                    final_args.append_array(arg);
                    break;
                }
            case Variant::Type::INT:
                {
                    final_args.push_back(arg);
                    break;
                }
            case Variant::Type::FLOAT:
                {
                    final_args.push_back(
                        DojoHelpers::double_to_variant_fp(
                            arg,
                            DojoHelpers::get_setting("dojo/config/fixed_point/default")
                        )
                    );
                    break;
                }
            case Variant::Type::PACKED_BYTE_ARRAY:
                {
                    final_args.push_back(arg);
                    break;
                }
            default:
                {
                    Logger::warning("Calldata", "Unsupported type", Variant::get_type_name(arg.get_type()), arg.stringify());
                    // final_args.push_back(arg.stringify());
                    break;
                }
            }
        }
        Logger::custom("CALLDATA", calldata_len);
        calldata_len = final_args.size();
        Logger::debug_extra("CALLDATA", calldata_len, Variant(final_args).stringify());
        felts_vec.reserve(calldata_len);
        for (int i = 0; i < calldata_len; i++)
        {
            const Variant& arg = final_args[i];
            if (arg.get_type() == Variant::Type::INT)
            {
                felts_vec.push_back(FieldElement::from_enum(arg));
            }
            else if (arg.get_type() == Variant::Type::PACKED_BYTE_ARRAY)
            {
                const PackedByteArray v_array = static_cast<PackedByteArray>(arg);
                if (v_array.size() == 32) // u256 / felt252
                {
                    DOJO::FieldElement felt;
                    memcpy(&felt, v_array.ptr(), 32);
                    felts_vec.push_back(felt);
                }
                else if (v_array.size() == 16) // u128 / i128
                {
                    DOJO::FieldElement felt;
                    memset(&felt, 0, sizeof(DOJO::FieldElement)); // Complete bytes
                    memcpy(&felt, v_array.ptr(), 16);
                    felts_vec.push_back(felt);
                }
                else
                {
                    String error_msg = "Unsupported PackedByteArray size: " + String::num_int64(v_array.size()) +
                        ". Expected 16 (for u128) or 32 (for u256/felt252).";
                    Logger::error("Calldata", error_msg);
                    emit_signal("transaction_failed", error_msg);
                    return;
                }
            }
            else if (static_cast<String>(arg).begins_with("0x"))
            {
                felts_vec.push_back(FieldElement::from_string(arg));
            }
            else
            {
                felts_vec.push_back(FieldElement::cairo_short_string_to_felt(arg));
            }
        }

        call.calldata = {felts_vec.data(), felts_vec.size()};
        Logger::debug_extra("CALLDATA", "Calldata added, size:", call.calldata.data_len);
    }

    Logger::debug_extra("CALL", Variant(call.selector));

    DOJO::ResultFieldElement res_tx = DOJO::account_execute_raw(account, &call, 1);
    if (res_tx.tag == DOJO::ErrFieldElement)
    {
        Logger::error("Transaction failed");
        Logger::error("To:", to);
        Logger::error("Selector:", selector);
        Logger::error("Error:", GET_DOJO_ERROR(res_tx));
        for (int i = 0; i < calldata_len; i++)
        {
            Logger::debug_extra("Calldata[" + String::num_int64(i) + "]", args[i].stringify());
        }
        emit_signal("transaction_failed", GET_DOJO_ERROR(res_tx));
    }
    else
    {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(res_tx));
        emit_signal("transaction_executed", FieldElement::get_as_string(&GET_DOJO_OK(res_tx)));
        Logger::success("Transaction sent");
    }
}
