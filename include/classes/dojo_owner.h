#ifndef DOJO_OWNER_H
#define DOJO_OWNER_H

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/string.hpp"

#include "controller.hpp"

using namespace godot;

class DojoOwner : public RefCounted {
    GDCLASS(DojoOwner, RefCounted)

    std::shared_ptr<controller::Owner> internal;

public:
    DojoOwner() {}
    ~DojoOwner() {}

    static Ref<DojoOwner> init(const String &private_key) {
        Ref<DojoOwner> owner;
        owner.instantiate();
        const std::string private_key_str = private_key.utf8().get_data();
        owner->internal = controller::Owner::init(private_key_str);
        return owner;
    }

    std::shared_ptr<controller::Owner> get_internal() { return internal; }

protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("DojoOwner", D_METHOD("init", "private_key"), &DojoOwner::init);
    }
};

#endif // DOJO_OWNER_H