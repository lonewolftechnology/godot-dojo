#include "tools/dojo_http.h"

#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/marshalls.hpp"
#include "godot_cpp/classes/time.hpp"

using namespace godot;

DojoHttp::DojoHttp() : port(27000), server(nullptr) {

} ;

DojoHttp::~DojoHttp() {

};

void DojoHttp::_ready() {
    DisplayServer::get_singleton()->enable_for_stealing_focus(OS::get_singleton()->get_process_id());
    start_server();
};

void DojoHttp::_process(double p_delta) {
    if (server->is_connection_available()) {
        Ref<StreamPeerTCP> client = server->take_connection();

        if (client.is_valid()) {
            Logger::debug_extra("DojoHttp", "Client found");
            clients.push_back(client);
        }
    }
    for (int i = 0; i < clients.size(); i++) {
        Ref<StreamPeerTCP> client = clients[i];
        if (client.is_valid()) {
            if (client->get_status() == StreamPeerTCP::STATUS_CONNECTED) {
                int available_bytes = client->get_available_bytes();
                if (available_bytes > 0) {
                    Array client_data = client->get_data(available_bytes);
                    PackedByteArray data = client_data[1];
                    String request_string = data.get_string_from_utf8();
                    String payload = filter_response(redirect_query_name + "=", request_string);
                    String parsed_payload = Marshalls::get_singleton()->base64_to_utf8(payload);

                    Dictionary response = JSON::parse_string(parsed_payload);
                    send_http_response(client, response);
                    client->disconnect_from_host();

                    DisplayServer::get_singleton()->window_request_attention();
                }
            }
        } else {
            clients.remove_at(i);
            i--;
        }
    }
}

String DojoHttp::filter_response(const String &marker, const String &p_response) {
    String end_marker = " HTTP/1.1";
    int start_pos = p_response.find(marker);
    if (start_pos == -1) return "";
    start_pos += marker.length();

    int end_pos = p_response.find(end_marker, start_pos);
    if (end_pos == -1) return "";

    String result = p_response.substr(start_pos, end_pos - start_pos);
    return result;
}

void DojoHttp::send_http_response(const Ref<StreamPeerTCP> &p_client, const Dictionary &p_response) {
    String username = p_response["username"];
    String address = p_response["address"];
    String owner_guid = p_response["ownerGuid"];
    bool already_registered = p_response["alreadyRegistered"];
    Dictionary expires_at = Time::get_singleton()->get_datetime_dict_from_unix_time(p_response["expiresAt"]);

    String html_template = R"(
            <!DOCTYPE html>
            <html>
            <head>
                <title>Godot Dojo!</title>
                <style>
                    body { font-family: sans-serif; text-align: center; margin-top: 50px; }
                </style>
            </head>
            <body>
                <h1>¡Godot Dojo!</h1>
                <p>Username: {username}</p>
                <p>Address: {address}</p>
                <p>OwnerGuid: {owner_guid}</p>
                <p>Already Registered: {already_registered}</p>
                <p>ExpiresAt: {expires_at}</p>
            </body>
            </html>
        )";

    Dictionary format_args;
    format_args["username"] = username;
    format_args["address"] = address;
    format_args["owner_guid"] = owner_guid;
    format_args["already_registered"] = already_registered;
    format_args["expires_at"] = expires_at;

    String html_content = html_template.format(format_args);

    String http_response = "HTTP/1.1 200 OK\r\n";
    http_response += "Content-Type: text/html; charset=UTF-8\r\n";
    http_response += "Content-Length: " + itos(html_content.utf8().size()) + "\r\n";
    http_response += "Connection: close\r\n";
    http_response += "\r\n";
    http_response += html_content;

    p_client->put_data(http_response.to_utf8_buffer());
}

void DojoHttp::setup(const String &p_host, int p_port) {
    set_host(p_host);
    set_port(p_port);
    start_server();
};

bool DojoHttp::start_server() {
    server = memnew(TCPServer);

    Error error = server->listen(port, host);
    if (error != OK) {
        Logger::error("Error initializing http server");
        set_process(false);
        return false;
    }

    set_process(true);
    Logger::success_extra("DojoHttp", vformat("Server listening in http://%s:%d", host, port));
    return true;
}
