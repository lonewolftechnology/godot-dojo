#include "node/dojo_controller.hpp"
#include "tools/controller_helper.h"

void DojoController::set_internal(const std::shared_ptr<controller::ControllerAccount>& p_internal) {
    internal = p_internal;
}

DojoController *DojoController::init(const String &app_id, const String &username, const String &class_hash, const String &rpc_url,
                                     const Ref<DojoOwner> &owner, const String &address,
                                     const String &chain_id) {
    DojoController *dojo_controller = memnew(DojoController);

    if (!dojo_controller->initialize(app_id, username, class_hash, rpc_url, owner, address, chain_id)) {
        memdelete(dojo_controller);
        return nullptr;
    }

    return dojo_controller;
}

DojoController *DojoController::from_storage(const String &app_id) {
    DojoController *dojo_controller = memnew(DojoController);

    CharString app_id_str = app_id.utf8();

    try {
        auto internal_controller = controller::ControllerAccount::from_storage(app_id_str.get_data());
        dojo_controller->set_internal(internal_controller);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.from_storage failed:", e.what());
        memdelete(dojo_controller);
        return nullptr;
    }

    return dojo_controller;
}

DojoController *DojoController::new_headless(const String &app_id, const String &username, const String &class_hash,
                                             const String &rpc_url, const Ref<DojoOwner> &owner,
                                             const String &chain_id) {
    DojoController *dojo_controller = memnew(DojoController);

    if (!dojo_controller->initialize_headless(app_id, username, class_hash, rpc_url, owner, chain_id)) {
        memdelete(dojo_controller);
        return nullptr;
    }

    return dojo_controller;
}

bool DojoController::initialize(const String &app_id, const String &username, const String &class_hash, const String &rpc_url,
                                     const Ref<DojoOwner> &owner, const String &address,
                                     const String &chain_id) {
    if (!owner.is_valid()) {
        Logger::error("DojoController.initialize failed: owner is null.");
        return false;
    }

    // Persist data to ensure lifetime matches DojoController
    owner_ = owner;
    app_id_ = app_id.utf8().get_data();
    username_ = username.utf8().get_data();
    class_hash_ = class_hash.utf8().get_data();
    rpc_url_ = rpc_url.utf8().get_data();
    address_ = address.utf8().get_data();
    chain_id_ = chain_id.utf8().get_data();

    try {
        auto internal_controller = controller::ControllerAccount::init(
            app_id_.c_str(),
            username_.c_str(),
            class_hash_.c_str(),
            rpc_url_.c_str(),
            owner_->get_internal(),
            address_.c_str(),
            chain_id_.c_str()
        );
        set_internal(internal_controller);
        emit_signal("controller_created");
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.initialize failed:", e.what());
        return false;
    }

    return true;
}

bool DojoController::initialize_headless(const String &app_id, const String &username, const String &class_hash,
                                             const String &rpc_url, const Ref<DojoOwner> &owner,
                                             const String &chain_id) {
    if (!owner.is_valid()) {
        Logger::error("DojoController.initialize_headless failed: owner is null.");
        return false;
    }

    // Persist data
    owner_ = owner;
    app_id_ = app_id.utf8().get_data();
    username_ = username.utf8().get_data();
    class_hash_ = class_hash.utf8().get_data();
    rpc_url_ = rpc_url.utf8().get_data();
    chain_id_ = chain_id.utf8().get_data();

    try {
        auto internal_controller = controller::ControllerAccount::new_headless(
            app_id_,
            username_,
            class_hash_,
            rpc_url_,
            owner_->get_internal(),
            chain_id_
        );
        set_internal(internal_controller);
        emit_signal("controller_created");
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.initialize_headless failed:", e.what());
        return false;
    }

    return true;
}

String DojoController::address() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return String(internal->address().c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.address failed:", e.what());
        return "";
    }
}

String DojoController::app_id() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return String(internal->app_id().c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.app_id failed:", e.what());
        return "";
    }
}

String DojoController::chain_id() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return String(internal->chain_id().c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.chain_id failed:", e.what());
        return "";
    }
}

void DojoController::clear_last_error() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return;
    }
    try {
        internal->clear_last_error();
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.clear_last_error failed:", e.what());
    }
}

String DojoController::delegate_account() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return {internal->delegate_account().c_str()};
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.delegate_account failed:", e.what());
        return "";
    }
}

void DojoController::disconnect() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return;
    }
    try {
        internal->disconnect();
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.disconnect failed:", e.what());
    }
}

String DojoController::error_message() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return String(internal->error_message().c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.error_message failed:", e.what());
        return "";
    }
}

String DojoController::execute(const TypedArray<Dictionary> &calls) const{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }

    std::vector<std::shared_ptr<controller::Call>> c_calls = ControllerHelper::prepare_calls(calls);

    try {
        return internal->execute(c_calls).c_str();
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.execute failed:", e.what());
        return "";
    }
}

String DojoController::execute_single(const String &contract_address, const String &entrypoint, const Array &calldata) const {
    Dictionary _call_dict;
    _call_dict["contract_address"] = contract_address;
    _call_dict["entrypoint"] = entrypoint;
    _call_dict["calldata"] = calldata;

    return execute(Array::make(_call_dict));
}

void DojoController::signup(const int &signer_type, const Variant &session_expiration, const Variant &cartridge_api_url) const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return;
    }

    std::optional<uint64_t> c_session_expiration = std::nullopt;
    if (session_expiration.get_type() != Variant::NIL) {
        if (session_expiration.get_type() == Variant::INT || session_expiration.get_type() == Variant::FLOAT) {
            c_session_expiration = static_cast<uint64_t>(session_expiration);
        } else {
            Logger::error("DojoController::signup failed: session_expiration should be numeric.");
            return;
        }
    }

    std::optional<std::string> c_cartridge_api_url = std::nullopt;
    if (cartridge_api_url.get_type() != Variant::NIL) {
        if (cartridge_api_url.get_type() == Variant::STRING || cartridge_api_url.get_type() == Variant::STRING_NAME) {
            String url_str = cartridge_api_url;
            if (!url_str.is_empty()) {
                c_cartridge_api_url = url_str.utf8().get_data();
            }
        } else {
            Logger::error("DojoController::signup failed: cartridge_api_url should be a String.");
            return;
        }
    }

    try {
        internal->signup(static_cast<controller::SignerType>(signer_type), c_session_expiration, c_cartridge_api_url);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.signup failed:", e.what());
    } catch (const std::exception &e) {
        Logger::error("DojoController.signup failed with unexpected error:", e.what());
    }
}

void DojoController::switch_chain(const String &rpc_url) const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return;
    }
    try {
        internal->switch_chain(rpc_url.utf8().get_data());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.switch_chain failed:", e.what());
    }
}

String DojoController::transfer(const String &recipient, const String &amount) const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return String(internal->transfer(recipient.utf8().get_data(), amount.utf8().get_data()).c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.transfer failed:", e.what());
        return "";
    }
}

String DojoController::username() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return {internal->username().c_str()};
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.username failed:", e.what());
        return "";
    }
}

Dictionary DojoController::get_info() const
{
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return {};
    }

    Dictionary info;
    info["address"] = address();
    info["app_id"] = app_id();
    info["chain_id"] = chain_id();
    info["username"] = username();

    return info;
}

bool DojoController::is_valid() const
{
    if (!internal) {
        return false;
    }
    return true;

}
