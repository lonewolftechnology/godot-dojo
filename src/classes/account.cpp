//
// Created by hazel on 29/05/25.
//

#include "classes/account.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "tools/logger.h"
#include "tools/dojo_helper.h"

Account::Account() {
    account = nullptr;
    provider = nullptr;
}

Account::~Account() {
    if (account) {
        DOJO::account_free(account);
    }
    if (provider) {
        DOJO::provider_free(provider);
    }
}

void Account::create(const String& rpc_url, const String& private_key, const String& address) {
    this->rpc_url = rpc_url;
    DOJO::ResultProvider res_provider = DOJO::provider_new(rpc_url.utf8().get_data());
    if (res_provider.tag == DOJO::ErrProvider) {
        UtilityFunctions::printerr("Error creating provider: ", GET_DOJO_ERROR(res_provider));
        return;
    }
    provider = GET_DOJO_OK(res_provider);

    DOJO::FieldElement private_key_felt = FieldElement::from_string(private_key);
    DOJO::ResultAccount res_account = DOJO::account_new(provider, private_key_felt, address.utf8().get_data());

    if (res_account.tag == DOJO::ErrAccount) {
        UtilityFunctions::printerr("Error creating account: ", GET_DOJO_ERROR(res_account));
        return;
    }

    account = GET_DOJO_OK(res_account);
}

void Account::deploy_burner(Account* master_account, const String& signing_key) {
    if (!master_account || !master_account->account) {
        Logger::error("Master account is not valid");
        return;
    }

    DOJO::FieldElement signing_key_felt = FieldElement::from_string(signing_key);
    DOJO::ResultAccount res_burner = DOJO::account_deploy_burner(master_account->provider, master_account->account, signing_key_felt);

    if (res_burner.tag == DOJO::ErrAccount) {
        UtilityFunctions::printerr("Error deploying burner: ", GET_DOJO_ERROR(res_burner));
        return;
    }

    account = GET_DOJO_OK(res_burner);
    provider = master_account->provider; // The burner uses the same provider
}

String Account::get_address() const {
    if (!account) {
        return "";
    }
    DOJO::FieldElement address = DOJO::account_address(account);
    return FieldElement::get_as_string(&address);
}

String Account::get_chain_id() const {
    if (!account) {
        return "";
    }
    DOJO::FieldElement chain_id = DOJO::account_chain_id(account);
    return FieldElement::get_as_string(&chain_id);
}

void Account::set_block_id(const String& block_id) {
    if (!account) {
        return;
    }
    // This is a simplification. In a real scenario, you would need to handle different block_id types (hash, number, tag).
    // Assuming block tag for now.
    DOJO::BlockId block;
    block.tag = DOJO::BlockTag_;
    block.block_tag = DOJO::Latest; // Or Pending, based on the string

    DOJO::account_set_block_id(account, block);
}

String Account::get_nonce() const {
    if (!account) {
        return "";
    }
    DOJO::ResultFieldElement nonce_res = DOJO::account_nonce(account);
    if (nonce_res.tag == DOJO::ErrFieldElement) {
        UtilityFunctions::printerr("Error getting nonce: ", GET_DOJO_ERROR(nonce_res));
        return "";
    }
    DOJO::FieldElement nonce = GET_DOJO_OK(nonce_res);
    return FieldElement::get_as_string(&nonce);
}

String Account::execute_raw(const Ref<DojoCall>& call) {
    if (!account) {
        return "";
    }

    DOJO::Call c;
    c.to = FieldElement::from_string(call->get_to());
    c.selector = call->get_selector().utf8().get_data();

    Array calldata_array = call->get_calldata();
    int calldata_len = calldata_array.size();
    std::vector<DOJO::FieldElement> calldata_vec;
    calldata_vec.reserve(calldata_len);

    for (int i = 0; i < calldata_len; ++i) {
        calldata_vec.push_back(FieldElement::from_string(calldata_array[i]));
    }

    c.calldata = {calldata_vec.data(), (uintptr_t)calldata_len};

    DOJO::ResultFieldElement res_tx = DOJO::account_execute_raw(account, &c, 1);
    if (res_tx.tag == DOJO::ErrFieldElement) {
        UtilityFunctions::printerr("Error executing raw transaction: ", GET_DOJO_ERROR(res_tx));
        return "";
    }

    DOJO::FieldElement tx_hash = GET_DOJO_OK(res_tx);
    return FieldElement::get_as_string(&tx_hash);
}
