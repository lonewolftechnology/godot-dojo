#include "web/node/torii_client.hpp"
#include "tools/dojo_bridge.h"
#include "ref_counted/queries/entity.hpp"
#include "tools/logger.hpp"

ToriiClient::ToriiClient() {}

ToriiClient::~ToriiClient() {}

bool ToriiClient::connect(const String& torii_url, int64_t max_message_size) {
    client = DojoBridge::create_object("ToriiClient");
    if (max_message_size == -1) {
        client->call_deferred("init", torii_url);
        Logger::success("ToriiClient connected: ", torii_url);
    } else {
        client->call_deferred("new_with_config", torii_url, static_cast<uint64_t>(max_message_size));
        Logger::success("ToriiClient connected with config: ", torii_url);
    }
    return true;
}

// void ToriiClient::on_client_created(Variant p_client) {
//     client = p_client;
//     emit_signal("connected");
// }

Ref<JavaScriptObject> ToriiClient::create(const String& torii_url, int64_t max_message_size) {
    Ref<JavaScriptObject> _client = DojoBridge::create_object("", torii_url, max_message_size);
    return _client;
}

// Dictionary ToriiClient::entities(const Ref<DojoQuery>& query) const {
//     if (client.get_type() == Variant::OBJECT) {
//         Ref<JavaScriptObject> js_client = client;
//         if (js_client.is_valid()) {
//             DojoBridge::call_async(js_client, "entities", query, Callable());
//         }
//     }
//     return Dictionary();
// }
//
// void ToriiClient::cancel_subscription(uint64_t subscription_id) const {
//     if (client.get_type() == Variant::OBJECT) {
//         Ref<JavaScriptObject> js_client = client;
//         if (js_client.is_valid()) {
//             js_client->call("cancel_subscription", subscription_id);
//         }
//     }
// }
//
// uint64_t ToriiClient::subscribe_entity_updates(const Ref<DojoClause> &clause, const PackedStringArray &world_addresses, const Ref<DojoCallback> &callback) const {
//     if (client.get_type() == Variant::OBJECT) {
//         Ref<JavaScriptObject> js_client = client;
//         if (js_client.is_valid()) {
//             Array args;
//             args.push_back(clause);
//             args.push_back(world_addresses);
//             args.push_back(callback->create_entity_callback());
//
//             DojoBridge::call_async(js_client, "subscribe_entity_updates", args, Callable());
//         }
//     }
//     return 0;
// }

void ToriiClient::_bind_methods() {
    ClassDB::bind_method(D_METHOD("connect", "torii_url", "max_message_size"), &ToriiClient::connect, DEFVAL(-1));
    ClassDB::bind_static_method("ToriiClient", D_METHOD("create", "torii_url", "max_message_size"), &ToriiClient::create, DEFVAL(-1));
    // ClassDB::bind_method(D_METHOD("entities", "query"), &ToriiClient::entities);
    // ClassDB::bind_method(D_METHOD("cancel_subscription", "subscription_id"), &ToriiClient::cancel_subscription);
    // ClassDB::bind_method(D_METHOD("subscribe_entity_updates", "clause", "world_addresses", "callback"), &ToriiClient::subscribe_entity_updates);
    
    // ClassDB::bind_method(D_METHOD("on_client_created", "client"), &ToriiClient::on_client_created);

    // ADD_SIGNAL(MethodInfo("connected"));
}