#pragma once

#include "godot_cpp/classes/http_request.hpp"

using namespace godot;

class HttpPinger : public HTTPRequest {
    GDCLASS(HttpPinger, HTTPRequest)

public:
    static HttpPinger* ping(const String& url);

    void _on_request_completed(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body);

protected:
    static void _bind_methods();
};