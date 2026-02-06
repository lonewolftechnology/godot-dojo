#pragma once

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "ref_counted/controller_utilities/dojo_owner.hpp"
#include "controller/controller.hpp"
#include "tools/controller_helper.h"
#include "tools/logger.hpp"

using namespace godot;

class DojoController : public Node {
    GDCLASS(DojoController, Node)

    std::shared_ptr<controller::ControllerAccount> internal;
    Ref<DojoOwner> owner_;
    std::string app_id_;
    std::string username_;
    std::string class_hash_;
    std::string rpc_url_;
    std::string address_;
    std::string chain_id_;

public:
    DojoController() = default;
    ~DojoController() override = default;

    void set_internal(const std::shared_ptr<controller::ControllerAccount>& p_internal);

    static DojoController *init(const String &app_id, const String &username, const String &class_hash, const String &rpc_url, const Ref<DojoOwner> &owner, const String &address, const String &chain_id);
    static DojoController *from_storage(const String &app_id);
    static DojoController *new_headless(const String &app_id, const String &username, const String &class_hash, const String &rpc_url, const Ref<DojoOwner> &owner, const String &chain_id);

    bool initialize(const String &app_id, const String &username, const String &class_hash, const String &rpc_url, const Ref<DojoOwner> &owner, const String &address, const String &chain_id);
    bool initialize_headless(const String &app_id, const String &username, const String &class_hash, const String &rpc_url, const Ref<DojoOwner> &owner, const String &chain_id);

    String address() const;
    String app_id() const;
    String chain_id() const;
    void clear_last_error() const;
    String delegate_account() const;
    void disconnect() const;
    String error_message() const;
    String execute(const TypedArray<Dictionary> &calls) const;
    String execute_raw(const String &contract_address, const String &entrypoint, const Array &calldata) const;
    void signup(const int &signer_type, const Variant &session_expiration, const Variant &cartridge_api_url) const;
    void switch_chain(const String &rpc_url) const;
    String transfer(const String &recipient, const String &amount) const;
    String username() const;
    Dictionary get_info() const;
    bool is_valid() const;


protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("DojoController", D_METHOD("init", "app_id", "username", "class_hash", "rpc_url", "owner", "address", "chain_id"), &DojoController::init);
        ClassDB::bind_static_method("DojoController", D_METHOD("from_storage", "app_id"), &DojoController::from_storage);
        ClassDB::bind_static_method("DojoController", D_METHOD("new_headless", "app_id", "username", "class_hash", "rpc_url", "owner", "chain_id"), &DojoController::new_headless);

        ClassDB::bind_method(D_METHOD("initialize", "app_id", "username", "class_hash", "rpc_url", "owner", "address", "chain_id"), &DojoController::initialize);
        ClassDB::bind_method(D_METHOD("initialize_headless", "app_id", "username", "class_hash", "rpc_url", "owner", "chain_id"), &DojoController::initialize_headless);

        ClassDB::bind_method(D_METHOD("address"), &DojoController::address);
        ClassDB::bind_method(D_METHOD("app_id"), &DojoController::app_id);
        ClassDB::bind_method(D_METHOD("chain_id"), &DojoController::chain_id);
        ClassDB::bind_method(D_METHOD("clear_last_error"), &DojoController::clear_last_error);
        ClassDB::bind_method(D_METHOD("delegate_account"), &DojoController::delegate_account);
        ClassDB::bind_method(D_METHOD("disconnect"), &DojoController::disconnect);
        ClassDB::bind_method(D_METHOD("error_message"), &DojoController::error_message);
        ClassDB::bind_method(D_METHOD("execute", "calls"), &DojoController::execute);
        ClassDB::bind_method(D_METHOD("execute_raw", "contract_address", "entrypoint", "calldata"), &DojoController::execute_raw);
        ClassDB::bind_method(D_METHOD("signup", "signer_type", "session_expiration", "cartridge_api_url"), &DojoController::signup);
        ClassDB::bind_method(D_METHOD("switch_chain", "rpc_url"), &DojoController::switch_chain);
        ClassDB::bind_method(D_METHOD("transfer", "recipient", "amount"), &DojoController::transfer);
        ClassDB::bind_method(D_METHOD("username"), &DojoController::username);
        ClassDB::bind_method(D_METHOD("get_info"), &DojoController::get_info);
        ClassDB::bind_method(D_METHOD("is_valid"), &DojoController::is_valid);
    }
};