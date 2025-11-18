#include "classes/dojo_controller.h"
#include "tools/controller_helper.h"
#include "tools/dojo_helper.h"

void DojoController::set_internal(std::shared_ptr<controller::ControllerAccount> p_internal) {
    internal = p_internal;
}

DojoController *DojoController::init(const String &app_id, const String &username, const String &class_hash, const String &rpc_url,
                                     const Ref<DojoOwner> &owner, const String &address,
                                     const String &chain_id) {
    DojoController *dojo_controller = memnew(DojoController);

    if (!owner.is_valid()) {
        Logger::error("DojoController.init failed: owner is null.");
        return nullptr;
    }

    try {
        auto internal_controller = controller::ControllerAccount::init(
            app_id.utf8().get_data(),
            username.utf8().get_data(),
            class_hash.utf8().get_data(),
            rpc_url.utf8().get_data(),
            owner->get_internal(),
            address.utf8().get_data(),
            chain_id.utf8().get_data()
        );
        dojo_controller->set_internal(internal_controller);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.init failed:", e.what());
        return nullptr;
    }

    return dojo_controller;
}

DojoController *DojoController::from_storage(const String &app_id) {
    DojoController *dojo_controller = memnew(DojoController);

    try {
        auto internal_controller = controller::ControllerAccount::from_storage(app_id.utf8().get_data());
        dojo_controller->set_internal(internal_controller);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.from_storage failed:", e.what());
        return nullptr;
    }

    return dojo_controller;
}

DojoController *DojoController::new_headless(const String &app_id, const String &username, const String &class_hash,
                                             const String &rpc_url, const Ref<DojoOwner> &owner,
                                             const String &chain_id) {
    DojoController *dojo_controller = memnew(DojoController);

    if (!owner.is_valid()) {
        Logger::error("DojoController.new_headless failed: owner is null.");
        return nullptr;
    }

    try {
        auto internal_controller = controller::ControllerAccount::new_headless(
            app_id.utf8().get_data(),
            username.utf8().get_data(),
            class_hash.utf8().get_data(),
            rpc_url.utf8().get_data(),
            owner->get_internal(),
            chain_id.utf8().get_data()
        );
        dojo_controller->set_internal(internal_controller);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.new_headless failed:", e.what());
        return nullptr;
    }

    return dojo_controller;
}

String DojoController::address() {
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

String DojoController::app_id() {
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

String DojoController::chain_id() {
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

void DojoController::clear_last_error() {
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

String DojoController::delegate_account() {
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return String(internal->delegate_account().c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.delegate_account failed:", e.what());
        return "";
    }
}

void DojoController::disconnect() {
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

String DojoController::error_message() {
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

String DojoController::execute(const TypedArray<Dictionary> &calls) {
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }

    std::vector<std::shared_ptr<controller::Call>> c_calls = ControllerHelper::prepare_calls(calls);

    try {
        return String(internal->execute(c_calls).c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.execute failed:", e.what());
        return "";
    }
}

void DojoController::signup(ControllerHelper::SignerType signer_type, const Variant &session_expiration, const String &cartridge_api_url) {
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return;
    }

    std::optional<uint64_t> c_session_expiration;
    if (session_expiration.get_type() != Variant::NIL) {
        c_session_expiration = session_expiration;
    }

    std::optional<std::string> c_cartridge_api_url;
    if (!cartridge_api_url.is_empty()) {
        c_cartridge_api_url = cartridge_api_url.utf8().get_data();
    }

    try {
        internal->signup(static_cast<controller::SignerType>(signer_type), c_session_expiration, c_cartridge_api_url);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.signup failed:", e.what());
    }
}

void DojoController::switch_chain(const String &rpc_url) {
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

String DojoController::transfer(const String &recipient, const String &amount) {
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

String DojoController::username() {
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return String(internal->username().c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.username failed:", e.what());
        return "";
    }
}

Dictionary DojoController::get_info() {
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return {};
    }

    Dictionary info;
    info["address"] = address();
    info["app_id"] = app_id();
    info["chain_id"] = chain_id();
    info["delegate_account"] = delegate_account();
    info["username"] = username();

    return info;
}

bool DojoController::is_valid() {
    if (!internal) {
        return false;
    }
    return true;

}
