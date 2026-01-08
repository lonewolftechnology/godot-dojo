#ifndef DOJO_HTTP_H
#define DOJO_HTTP_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/tcp_server.hpp"
#include "godot_cpp/classes/stream_peer_tcp.hpp"
#include "tools/logger.h"
using namespace godot;

class DojoHttp : public Node {
    GDCLASS(DojoHttp, Node)
    int port;
    String host;
    TCPServer* server;
    Array clients;
    String redirect_query_name;

public:
    DojoHttp();

    ~DojoHttp();

    void _ready() override;

    void _process(double p_delta) override;

    static String filter_response(const String &marker, const String &p_response);

    static void send_http_response(const Ref<StreamPeerTCP> &p_client, const Dictionary &p_response);

    void setup(const String &p_host, int p_port);

    bool start_server();

    void set_host(const String &p_host) { host = p_host; }
    void set_port(int p_port) { port = p_port; }
    void set_redirect_query_name(const String &p_redirect_query_name) { redirect_query_name = p_redirect_query_name; }
    String get_host() { return host; }
    int get_port() const { return port; }
    String get_redirect_query_name() { return redirect_query_name; }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_host", "host"), &DojoHttp::set_host);
        ClassDB::bind_method(D_METHOD("get_host"), &DojoHttp::get_host);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "host"), "set_host", "get_host");

        ClassDB::bind_method(D_METHOD("set_port", "port"), &DojoHttp::set_port);
        ClassDB::bind_method(D_METHOD("get_port"), &DojoHttp::get_port);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "port"), "set_port", "get_port");

        ClassDB::bind_method(D_METHOD("set_redirect_query_name", "redirect_query_name"), &DojoHttp::set_redirect_query_name);
        ClassDB::bind_method(D_METHOD("get_redirect_query_name", "redirect_query_name"), &DojoHttp::get_redirect_query_name);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "redirect_query_name"), "set_redirect_query_name", "get_redirect_query_name");

        ClassDB::bind_method(D_METHOD("setup", "host", "port"), &DojoHttp::setup);
        ClassDB::bind_method(D_METHOD("start_server"), &DojoHttp::start_server);
        ClassDB::bind_static_method("DojoHttp", D_METHOD("filter_response", "marker", "response"), &DojoHttp::filter_response);
        ClassDB::bind_static_method("DojoHttp", D_METHOD("send_http_response", "client", "response"), &DojoHttp::send_http_response);
    }
};

#endif //DOJO_HTTP_H
