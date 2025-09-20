#include "tools/dojo_bridge.h"

#include "godot_cpp/classes/json.hpp"
#include "tools/logger.h"
#include "ref_counted/dojo_option.h"
#include "godot_cpp/core/version.hpp"
#include "godot_cpp/classes/java_script_object.hpp"

JavaScriptBridge* DojoBridge::js_bridge = nullptr;

// Using a function-local static to avoid the "static initialization order fiasco".
// This ensures the Dictionary is constructed only when first needed and after Godot is initialized.
static Dictionary& get_active_callbacks()
{
    static Dictionary callbacks;
#if GODOT_VERSION_MAJOR > 4 || (GODOT_VERSION_MAJOR == 4 && GODOT_VERSION_MINOR >= 3)
    callbacks = Dictionary(Dictionary(), Variant::STRING, "String", String(), Variant::STRING, "String", String());

#endif
    return callbacks;
}

DojoBridge::DojoBridge()
{
    Logger::debug_extra("DojoBridge", "Initializing JS Bridge");
    js_bridge = JavaScriptBridge::get_singleton();
}

DojoBridge::~DojoBridge()
{
    js_bridge = nullptr;
    get_active_callbacks().clear();
}

void DojoBridge::call_async(const String& function_name, const Variant& args, const Callable& callback)
{
    call_async(nullptr, function_name, args, callback);
}

Variant DojoBridge::process_js_arg(const Variant& arg) {
    if (arg.get_type() == Variant::OBJECT) {
        Ref<DojoOption> dojo_option = arg;
        if (dojo_option.is_valid()) {
            return "json:" + JSON::stringify(dojo_option->to_json());
        }
    }

    if (arg.get_type() == Variant::DICTIONARY) {
        return "json:" + JSON::stringify(arg);
    }

    if (arg.get_type() == Variant::NIL) {
        if (js_bridge) {
            return js_bridge->create_object("undefined", Variant());
        }
        return arg;
    }

    return arg;
}

void DojoBridge::call_async(const Ref<JavaScriptObject>& target, const String& function_name, const Variant& args,
                            const Callable& callback)
{
    // WEB IMPLEMENTATION
    // Lazy-initialize the JS bridge if it hasn't been already.
    if (!js_bridge)
    {
        Logger::debug("DojoBridge", "JS Bridge not found, initializing from static call.");
        js_bridge = JavaScriptBridge::get_singleton();
    }

    Array js_args;
    if (args.get_type() == Variant::ARRAY)
    {
        const Array& arr = args;
        for (int i = 0; i < arr.size(); ++i)
        {
            js_args.push_back(process_js_arg(arr[i]));
        }
    }
    else
    {
        js_args.push_back(process_js_arg(args));
    }

    if (callback.is_valid())
    {
        Logger::debug_extra("DojoBridge", "Creating callback");
        Ref<JavaScriptObject> js_callback = js_bridge->create_callback(callback);
        uint64_t id = js_callback->get_instance_id();

        js_args.push_back(js_callback);

        Array callback_data;
        callback_data.push_back(js_callback); // The JS object proxy.

        Object* target_object = callback.get_object();
        if (target_object && target_object->is_class("RefCounted"))
        {
            callback_data.push_back(Variant(target_object));
        }

        get_active_callbacks()[Variant(id)] = callback_data;
    }
    else
    {
        js_args.push_back(callback); // Push null if callback is not valid
    }

    Logger::debug_extra("DojoBridge", function_name, js_args);

    // Always use the 'callDojoAsync' helper in JS.
    // It is designed to handle both global and targeted calls.
    Ref<JavaScriptObject> window = js_bridge->get_interface("window");
    if (!window.is_valid())
    {
        Logger::error("DojoBridge: Could not get 'window' object to call JavaScript.");
        return;
    }
    Array final_args;

    if (target != nullptr)
    {
        Logger::debug_extra("DojoBridge", "adding", target);
        final_args.push_back(target);
    }

    final_args.push_back(function_name);
    final_args.append_array(js_args);

    Logger::debug_extra("DojoBridge", "Calling JS helper 'callDojoAsync'", final_args);
    window->callv("callDojoAsync", final_args);
}

void DojoBridge::release_callback(uint64_t callback_id)
{
    Logger::debug_extra("DojoBridge", "Releasing callback with ID: " + String::num_uint64(callback_id));
    // Find and remove the callback from the active list.
    if (get_active_callbacks().has(Variant(callback_id)))
    {
        get_active_callbacks().erase(Variant(callback_id));
    }
}
