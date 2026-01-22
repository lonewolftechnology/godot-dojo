#include "tools/http_pinger.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/window.hpp"
#include "godot_cpp/classes/json.hpp"
#include "tools/logger.h"

HttpPinger* HttpPinger::ping(const String& url) {
    SceneTree* tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    if (!tree) {
        Logger::error("HttpPinger: SceneTree not found.");
        return nullptr;
    }

    HttpPinger* pinger = memnew(HttpPinger);
    tree->get_root()->add_child(pinger);
    
    pinger->connect("request_completed", Callable(pinger, "_on_request_completed"));
    
    Error err = pinger->request(url);
    
    if (err != OK) {
        Logger::error("HttpPinger: Failed to start request for ", url);
        pinger->queue_free();
        return nullptr;
    }
    return pinger;
}

void HttpPinger::_on_request_completed(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body) {
    Variant res;
    if (result == HTTPRequest::RESULT_SUCCESS && response_code >= 200 && response_code < 300) {
        res = JSON::parse_string(body.get_string_from_utf8());
        if (res.get_type() != Variant::DICTIONARY) res = Variant();
    }
    emit_signal("response_received", res);
    queue_free();
}

void HttpPinger::_bind_methods() {
    ClassDB::bind_static_method(get_class_static(), D_METHOD("ping", "url"), &HttpPinger::ping);
    ClassDB::bind_method(D_METHOD("_on_request_completed", "result", "response_code", "headers", "body"), &HttpPinger::_on_request_completed);
    ADD_SIGNAL(MethodInfo("response_received", PropertyInfo(Variant::DICTIONARY, "response")));
}