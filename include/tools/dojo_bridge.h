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
    static Variant process_js_arg(const Variant& arg);
    static void call_async(const Ref<JavaScriptObject>& target, const String &function_name, const Variant &args, const Callable &callback);
    
    // Method to be called from JS to release the callback
    static void release_callback(uint64_t callback_id);

protected:
    static void _bind_methods(){}
};

#endif // DOJO_BRIDGE_H