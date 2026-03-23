#pragma once
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string.hpp"
#ifndef WEB_ENABLED
#include "controller/controller.hpp"
#endif
#include "tools/controller_helper.h"
#include "tools/logger.hpp"

using namespace godot;

class DojoOwner : public RefCounted {
    GDCLASS(DojoOwner, RefCounted)
#ifndef WEB_ENABLED
    std::shared_ptr<controller::Owner> internal;
#endif

public:
    DojoOwner() {}
    ~DojoOwner() override {}

    static Ref<DojoOwner> init(const String &private_key) {
        if (!ControllerHelper::validate_felt(private_key))
        {
            Logger::error("Invalid private key format.");
            return {};
        }
        Ref<DojoOwner> owner;
        owner.instantiate();
#ifndef WEB_ENABLED
        const std::string private_key_str = private_key.utf8().get_data();

        owner->internal = controller::Owner::init(private_key_str);
#endif

        return owner;
    }
#ifndef WEB_ENABLED

    std::shared_ptr<controller::Owner> get_internal() { return internal; }
#endif

    Dictionary to_dict() const {
        return Dictionary(); // Private key should not be exposed easily unless needed
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("DojoOwner", D_METHOD("init", "private_key"), &DojoOwner::init);
    }
};
