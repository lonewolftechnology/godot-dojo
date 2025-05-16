//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
DojoC* DojoC::singleton = nullptr;


void DojoC::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoC::set_enabled);
	ClassDB::bind_method(D_METHOD("get_enabled"), &DojoC::get_enabled);
	// ClassDB::bind_method(D_METHOD("client_new"), &DojoC::client_new);
	ClassDB::bind_method(D_METHOD("create_client", "p_world"), &DojoC::create_client);
	ClassDB::bind_method(D_METHOD("controller_connect"), &DojoC::controller_connect);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");
}

DojoC::DojoC()
{
	// Initialize any variables here.
	singleton = this;
	enabled = true;
}

DojoC* DojoC::get_singleton()
{
	return singleton;
}

DojoC::~DojoC()
{
	// Add your cleanup here.
	singleton = nullptr;
}

void hex_to_bytes(const String _addr, dojo_bindings::FieldElement* felt)
{
	const char* hex = _addr.utf8().get_data();

	if (hex[0] == '0' && hex[1] == 'x')
	{
		hex += 2;
	}

	// handle hex less than 64 characters - pad with 0
	size_t len = strlen(hex);
	if (len < 64)
	{
		char* padded = static_cast<char*>(malloc(65));
		memset(padded, '0', 64 - len);
		padded[64 - len] = '\0';
		strcat(padded, hex);
		hex = padded;
	}

	UtilityFunctions::prints("hex: ", hex);

	for (size_t i = 0; i < 32; i++)
	{
		sscanf(hex + 2 * i, "%2hhx", &(*felt).data[i]);
	}
}

String DojoC::create_client(const String& world_addr)
{
	String _msg = "";
	const char* torii_url = "http://localhost:8080";
	const char* relay_url = "/ip4/127.0.0.1/tcp/9090";

	dojo_bindings::ToriiClient* client;

	UtilityFunctions::print("Attempting to create client...");

	dojo_bindings::FieldElement world;
	hex_to_bytes(world_addr, &world);
	// Log las direcciones que estamos usando
	UtilityFunctions::prints("RPC Endpoint: ", torii_url);
	UtilityFunctions::prints("World Endpoint: ", relay_url);

	dojo_bindings::ResultToriiClient resClient = dojo_bindings::client_new(
		torii_url,
		relay_url,
		world);

	if (resClient.tag == dojo_bindings::ErrToriiClient)
	{
		UtilityFunctions::printerr("Error: ", resClient.err.message);
		_msg = String("[Error] create_client: ") + resClient.err.message;
	}
	else
	{
		UtilityFunctions::print_verbose("Client creado");
		client = resClient.ok;
		_msg = "Client creado";
	}


	return _msg;
}



void string_to_bytes(const std::string& hex_str, uint8_t* out_bytes, size_t max_bytes) {
	// Skip "0x" prefix if present
	size_t start_idx = (hex_str.substr(0, 2) == "0x") ? 2 : 0;

	// Calculate actual string length without prefix
	size_t hex_length = hex_str.length() - start_idx;

	// Handle odd number of characters by assuming leading zero
	bool is_odd = hex_length % 2 != 0;
	size_t num_bytes = (hex_length + is_odd) / 2;

	// Ensure we don't overflow the output buffer
	if (num_bytes > max_bytes) {
		return;
		//        throw std::runtime_error("Input hex string too long for output buffer");
	}

	size_t out_idx = 0;

	// Handle first nibble separately if we have odd number of characters
	if (is_odd) {
		std::string nibble = hex_str.substr(start_idx, 1);
		out_bytes[out_idx++] = static_cast<uint8_t>(std::stoul(nibble, nullptr, 16));
	}

	// Process two hex digits at a time
	for (size_t i = is_odd ? 1 : 0; i < hex_length; i += 2) {
		std::string byte_str = hex_str.substr(start_idx + i, 2);
		out_bytes[out_idx++] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
	}
}

// callback al iniciar sesion en el controller de Cartridge
void on_account(dojo_bindings::ControllerAccount* account)
{
// Implementar
	UtilityFunctions::print_verbose("on_account callback Triggered");
}

void DojoC::controller_connect(const String &controller_addr)
{
	const char* rpc_url = "http://localhost:5050";

	dojo_bindings::FieldElement target;
	hex_to_bytes(controller_addr, &target);

	dojo_bindings::Policy policies[] = {
		{ target, "spawn", "spawn" },
		{ target, "move", "move" },
	};
	
	uintptr_t policies_len = 2;

	dojo_bindings::controller_connect(rpc_url, policies, policies_len, on_account);


}

