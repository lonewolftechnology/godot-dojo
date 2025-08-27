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
    if (account != nullptr)
    {
        DOJO::account_free(account);
    }
    if (provider != nullptr)
    {
        DOJO::provider_free(provider);
    }
}

void Account::start_provider()
{
    DOJO::ResultProvider res_provider = DOJO::provider_new(rpc_url.utf8().get_data());
    if (res_provider.tag == DOJO::ErrProvider)
    {
        Logger::error("Error creating provider: ", GET_DOJO_ERROR(res_provider));
    }
    else
    {
        Logger::success("Provider created");
        provider = GET_DOJO_OK(res_provider);
    }
}

void Account::create(const String& _rpc_url, const String& address, const String& private_key)
{
    this->rpc_url = _rpc_url;
    if (provider == nullptr)
    {
        start_provider();
    }

    DOJO::FieldElement private_key_felt = FieldElement::from_string(private_key);
    DOJO::ResultAccount res_account = DOJO::account_new(provider, private_key_felt, address.utf8().get_data());

    if (res_account.tag == DOJO::ErrAccount)
    {
        Logger::error("Error creating account: ", GET_DOJO_ERROR(res_account));
    }
    else
    {
        // Logger::success("Account created");
        account = GET_DOJO_OK(res_account);
        DOJO::FieldElement account_address = DOJO::account_address(account);
        Logger::success("Account address: ", FieldElement::get_as_string(&account_address));
        DOJO::FieldElement chain_id = DOJO::account_chain_id(account);
        Logger::success("Account chain id: ", FieldElement::get_as_string(&chain_id));
    }
}

void Account::deploy_burner(Account* master_account, const String& signing_key)
{
    if (!master_account || !master_account->account)
    {
        Logger::error("Master account is not valid");
        return;
    }

    // DOJO::FieldElement signing_key_felt = FieldElement::from_string(signing_key);
    DOJO::FieldElement signing_key_felt = DOJO::signing_key_new();
    DOJO::ResultAccount res_burner = DOJO::account_deploy_burner(master_account->provider, master_account->account,
                                                                 signing_key_felt);

    if (res_burner.tag == DOJO::ErrAccount)
    {
        Logger::error("Error deploying burner: ", GET_DOJO_ERROR(res_burner));
    }else
    {
        account = GET_DOJO_OK(res_burner);
        provider = master_account->provider; // The burner uses the same provider
    }


}

String Account::get_address() const
{
    if (!is_account_valid())
    {
        return "";
    }
    DOJO::FieldElement address = DOJO::account_address(account);
    return FieldElement::get_as_string(&address);
}

String Account::get_chain_id(const bool& parse) const
{
    if (!is_account_valid())
    {
        return "";
    }
    DOJO::FieldElement chain_id = DOJO::account_chain_id(account);
    FieldElement chain_id_felt(chain_id);
    if (parse)
    {
        return chain_id_felt.parse_cairo();
    }
    return chain_id_felt.to_string();
}

void Account::set_block_id(const String& block_id)
{
    if (!is_account_valid())
    {
        return;
    }

    DOJO::BlockId block;
    if (block_id.begins_with("0x"))
    {
        Logger::debug("Setting block id to hash");
        block.tag = DOJO::Hash;
        block.hash = FieldElement::from_string(block_id);
    }
    else if (block_id.is_valid_int())
    {
        Logger::debug("Setting block id to number");
        block.tag = DOJO::Number;
        block.number = block_id.to_int();
    }else
    {
        Logger::debug("Setting block id to Latest");
        block.tag = DOJO::BlockTag_;
        block.block_tag = DOJO::Latest;
    }

    DOJO::account_set_block_id(account, block);
}

String Account::get_nonce() const
{
    if (!is_account_valid())
    {
        return "";
    }
    DOJO::ResultFieldElement nonce_res = DOJO::account_nonce(account);
    if (nonce_res.tag == DOJO::ErrFieldElement)
    {
        Logger::error("Error getting nonce: ", GET_DOJO_ERROR(nonce_res));
        return "";
    }
    DOJO::FieldElement nonce = GET_DOJO_OK(nonce_res);
    return FieldElement::get_as_string(&nonce);
}

bool Account::is_account_valid() const
{
    return account != nullptr;
}

void Account::execute_raw(const String& to, const String& selector, const Array& args)
{
    if (!is_account_valid())
    {
        return;
    }

    DojoCallData call_data = DojoHelpers::prepare_dojo_call_data(to, selector, args);

    if (!call_data.is_valid) {
        Dictionary error_dict;
        error_dict["error"] = "Failed to create transaction call. Check logs for details.";
        error_dict["selector"] = selector;
        error_dict["calldata"] = args;
        Logger::error_dict(error_dict);
        emit_signal("transaction_failed", error_dict);
        return;
    }

    DOJO::Call call = {
        call_data.to,
        call_data.selector_str.c_str(),
        {call_data.calldata_felts.data(), call_data.calldata_felts.size()}
    };

    DOJO::ResultFieldElement res_tx = DOJO::account_execute_raw(account, &call, 1);
    if (res_tx.tag == DOJO::ErrFieldElement)
    {
        Logger::error("Transaction failed:", GET_DOJO_ERROR(res_tx));

        Dictionary error_dict = {};
        error_dict["error"] = GET_DOJO_ERROR(res_tx);
        error_dict["selector"] = selector;
        error_dict["calldata"] = args;

        emit_signal("transaction_failed", error_dict);
    }
    else
    {
        DOJO::wait_for_transaction(provider, GET_DOJO_OK(res_tx));
        Dictionary success_dict = {};
        success_dict["selector"] = selector;
        success_dict["calldata"] = args;
        success_dict["txn"] = FieldElement::get_as_string(&GET_DOJO_OK(res_tx));
        emit_signal("transaction_executed", success_dict);
        Logger::success("Transaction sent");
    }
}
