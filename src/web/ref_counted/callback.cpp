#include "web/ref_counted/callback.hpp"
#include "godot_cpp/classes/java_script_bridge.hpp"
#include "godot_cpp/classes/java_script_object.hpp"

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

Variant DojoCallback::create_entity_callback() {
    JavaScriptBridge *jsb = JavaScriptBridge::get_singleton();
    if (!jsb) {
        return Variant();
    }

    return jsb->create_callback(on_update);
}
