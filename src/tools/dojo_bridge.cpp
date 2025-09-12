#include "tools/dojo_bridge.h"

#include "godot_cpp/classes/json.hpp"
#include <tools/logger.h>
#include "godot_cpp/classes/java_script_object.hpp"

JavaScriptBridge* DojoBridge::js_bridge = nullptr;

DojoBridge::DojoBridge() {
    Logger::debug_extra("DojoBridge", "Initializing JS Bridge");
    js_bridge = JavaScriptBridge::get_singleton();
}

DojoBridge::~DojoBridge() {
    js_bridge = nullptr;
}

void DojoBridge::call_async(const String &function_name, const Variant &args, const Callable &callback) {
    // Lazily initialize the JS bridge if it hasn't been already.
    if (!js_bridge) {
        Logger::debug_extra("DojoBridge", "Initializing JS Bridge");
        js_bridge = JavaScriptBridge::get_singleton();
    }

    Array js_args;
    js_args.push_back(function_name);
    // If args is an array, append its elements. Otherwise, push the whole variant.
    if (args.get_type() == Variant::ARRAY) {
        const Array& arr = args;
        for (int i = 0; i < arr.size(); ++i) {
            Variant item = arr[i];
            if (item.get_type() == Variant::DICTIONARY) {
                js_args.push_back("json:" + JSON::stringify(item));
            } else {
                js_args.push_back(item);
            }
        }
    } else if (args.get_type() == Variant::DICTIONARY) {
        js_args.push_back("json:" + JSON::stringify(args));
    } else {
        js_args.push_back(args);
    }

    // Only create a callback if the provided Callable is valid.
    if (callback.is_valid()) {
        Logger::debug_extra("DojoBridge", "Creating callback");
        Ref<JavaScriptObject> js_callback = js_bridge->create_callback(callback);
        js_args.push_back(js_callback);
    } else {
        js_args.push_back(callback); // Push null if callback is not valid
    }

    Logger::debug_extra("DojoBridge",function_name, js_args);

    // In Godot 4, we must first get the 'window' object and then call the method on it.
    // The 'call' method is on JavaScriptObject, not on the JavaScriptBridge singleton.
    Ref<JavaScriptObject> window = js_bridge->get_interface("window");
    if (window.is_valid()) {
        window->callv("callDojoAsync", js_args);
    } else {
        Logger::error("DojoBridge: Could not get 'window' object from JavaScriptBridge.");
    }
}