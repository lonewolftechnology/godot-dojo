#include "node/torii_client.hpp"

using namespace godot;

ToriiClient::ToriiClient() {}
ToriiClient::~ToriiClient() {}

ToriiClient* ToriiClient::create(const String& torii_url) {
    ToriiClient* node = memnew(ToriiClient);
    node->client = dojo::ToriiClient::init(torii_url.utf8().get_data());
    return node;
}

std::shared_ptr<dojo::ToriiClient> ToriiClient::get_client() const {
    return client;
}