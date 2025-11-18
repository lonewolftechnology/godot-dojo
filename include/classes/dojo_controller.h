#ifndef DOJO_CONTROLLER_H
#define DOJO_CONTROLLER_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "classes/dojo_owner.h"
#include "controller/controller.hpp"
#include "tools/controller_helper.h"
#include "tools/logger.h"

using namespace godot;

class DojoController : public Node {
    GDCLASS(DojoController, Node)

    std::shared_ptr<controller::ControllerAccount> internal;

public:
    DojoController() = default;
    ~DojoController() override = default;

    void set_internal(std::shared_ptr<controller::ControllerAccount> p_internal);

    static DojoController *init(const String &app_id, const String &username, const String &class_hash, const String &rpc_url, const Ref<DojoOwner> &owner, const String &address, const String &chain_id);
    static DojoController *from_storage(const String &app_id);
    static DojoController *new_headless(const String &app_id, const String &username, const String &class_hash, const String &rpc_url, const Ref<DojoOwner> &owner, const String &chain_id);

    String address();
    String app_id();
    String chain_id();
    void clear_last_error();
    String delegate_account();
    void disconnect();
    String error_message();
    String execute(const TypedArray<Dictionary> &calls);
    void signup(ControllerHelper::SignerType signer_type, const Variant &session_expiration, const String &cartridge_api_url);
    void switch_chain(const String &rpc_url);
    String transfer(const String &recipient, const String &amount);
    String username();
    Dictionary get_info();
    bool is_valid();


protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("DojoController", D_METHOD("init", "app_id", "username", "class_hash", "rpc_url", "owner", "address", "chain_id"), &DojoController::init);
        ClassDB::bind_static_method("DojoController", D_METHOD("from_storage", "app_id"), &DojoController::from_storage);
        ClassDB::bind_static_method("DojoController", D_METHOD("new_headless", "app_id", "username", "class_hash", "rpc_url", "owner", "chain_id"), &DojoController::new_headless);

        ClassDB::bind_method(D_METHOD("address"), &DojoController::address);
        ClassDB::bind_method(D_METHOD("app_id"), &DojoController::app_id);
        ClassDB::bind_method(D_METHOD("chain_id"), &DojoController::chain_id);
        ClassDB::bind_method(D_METHOD("clear_last_error"), &DojoController::clear_last_error);
        ClassDB::bind_method(D_METHOD("delegate_account"), &DojoController::delegate_account);
        ClassDB::bind_method(D_METHOD("disconnect"), &DojoController::disconnect);
        ClassDB::bind_method(D_METHOD("error_message"), &DojoController::error_message);
        ClassDB::bind_method(D_METHOD("execute", "calls"), &DojoController::execute);
        ClassDB::bind_method(D_METHOD("signup", "signer_type", "session_expiration", "cartridge_api_url"), &DojoController::signup);
        ClassDB::bind_method(D_METHOD("switch_chain", "rpc_url"), &DojoController::switch_chain);
        ClassDB::bind_method(D_METHOD("transfer", "recipient", "amount"), &DojoController::transfer);
        ClassDB::bind_method(D_METHOD("username"), &DojoController::username);
        ClassDB::bind_method(D_METHOD("get_info"), &DojoController::get_info);
        ClassDB::bind_method(D_METHOD("is_valid"), &DojoController::is_valid);
    }
};

#endif // DOJO_CONTROLLER_H