#ifndef DOJO_CONTROLLER_H
#define DOJO_CONTROLLER_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"

#include "classes/dojo_owner.h"
#include "controller/controller.hpp"
#include "tools/logger.h"

using namespace godot;

class DojoController : public Node {
    GDCLASS(DojoController, Node)

    std::shared_ptr<controller::Controller> internal;

public:
    DojoController() = default;
    ~DojoController() override = default;

    void set_internal(std::shared_ptr<controller::Controller> p_internal);

    static DojoController *init(const String &app_id, const String &username, const String &class_hash, const String &rpc_url, const Ref<DojoOwner> &owner, const String &address, const String &chain_id);
    static DojoController *from_storage(const String &app_id);

    String address();
    String execute(const TypedArray<Dictionary> &calls);

protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("DojoController", D_METHOD("init", "app_id", "username", "class_hash", "rpc_url", "owner", "address", "chain_id"), &DojoController::init);
        ClassDB::bind_static_method("DojoController", D_METHOD("from_storage", "app_id"), &DojoController::from_storage);
        ClassDB::bind_method(D_METHOD("address"), &DojoController::address);
        ClassDB::bind_method(D_METHOD("execute", "calls"), &DojoController::execute);
    }
};

#endif // DOJO_CONTROLLER_H