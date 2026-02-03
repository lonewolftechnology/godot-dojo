#include "ref_counted/dojo_utilities/callback.hpp"
#include "ref_counted/dojo_utilities/callback_utils.hpp"

void DojoCallback::set_on_update(const Callable &p_on_update) {
    on_update = p_on_update;
}

Callable DojoCallback::get_on_update() const {
    return on_update;
}

void DojoCallback::set_on_error(const Callable &p_on_error) {
    on_error = p_on_error;
}

Callable DojoCallback::get_on_error() const {
    return on_error;
}

std::shared_ptr<dojo::EntityUpdateCallback> DojoCallback::create_entity_callback() {
    return get_callback<dojo::EntityUpdateCallback, dojo::Entity>(CallbackUtils::entity_to_dict);
}

std::shared_ptr<dojo::EventUpdateCallback> DojoCallback::create_event_callback() {
    return get_callback<dojo::EventUpdateCallback, dojo::Event>(CallbackUtils::event_to_dict);
}

std::shared_ptr<dojo::TokenBalanceUpdateCallback> DojoCallback::create_token_balance_callback() {
    return get_callback<dojo::TokenBalanceUpdateCallback, dojo::TokenBalance>(CallbackUtils::token_balance_to_dict);
}

std::shared_ptr<dojo::TokenUpdateCallback> DojoCallback::create_token_callback() {
    return get_callback<dojo::TokenUpdateCallback, dojo::Token>(CallbackUtils::token_to_dict);
}

std::shared_ptr<dojo::TransactionUpdateCallback> DojoCallback::create_transaction_callback() {
    return get_callback<dojo::TransactionUpdateCallback, dojo::Transaction>(CallbackUtils::transaction_to_dict);
}

