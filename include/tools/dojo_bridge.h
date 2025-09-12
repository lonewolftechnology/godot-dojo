#ifndef DOJO_BRIDGE_H
#define DOJO_BRIDGE_H

#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/classes/java_script_bridge.hpp"


using namespace godot;

class DojoBridge : public RefCounted{
    GDCLASS(DojoBridge, RefCounted);

    static JavaScriptBridge* js_bridge;

public:
    DojoBridge();
    ~DojoBridge();

    // Calls an async function on the dojoInstance in JS
    // and returns the result via a callback.
    static void call_async(const String &function_name, const Variant &args, const Callable &callback);
protected:
    static void _bind_methods()
    {
        ClassDB::bind_static_method("DojoBridge", D_METHOD("call_async"), &DojoBridge::call_async);
    }
};

#endif // DOJO_BRIDGE_H