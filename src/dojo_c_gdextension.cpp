//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include <unistd.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
DojoC* DojoC::singleton = nullptr;
dojo_bindings::ControllerAccount* session_account = nullptr;


void DojoC::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoC::set_enabled);
	ClassDB::bind_method(D_METHOD("get_enabled"), &DojoC::get_enabled);
	ClassDB::bind_method(D_METHOD("testing"), &DojoC::testing);
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


// Esto deberia ir a la clase FieldElement
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

// que devuelva un msg es testing, no deberia devolver nada
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
		// UtilityFunctions::push_error("Error: ", resClient.err.message);
		_msg = String("[Error] create_client: ") + resClient.err.message;
		return "ERROR";
	}
	else
	{
		UtilityFunctions::print_verbose("Client creado");
		client = resClient.ok;
		_msg = "Client creado";
	}


	return _msg;
}




// callback al iniciar sesion en el controller de Cartridge
void on_account(dojo_bindings::ControllerAccount* account)
{
// Implementar
UtilityFunctions::print_verbose("on_account callback Triggered");
session_account = account;
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

void DojoC::testing()
{


	const char* rpc_url = "http://0.0.0.0:5050";
	const char* torii_url = "http://0.0.0.0:8080";
	const char* relay_url = "/ip4/192.168.100.49/tcp/9090";




	dojo_bindings::FieldElement world;
	hex_to_bytes("0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5", &world);
	dojo_bindings::FieldElement actions;
	hex_to_bytes("0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf", &actions);
	dojo_bindings::FieldElement player;
	hex_to_bytes("0x127fd5f1fe78a71f8bcd1fec63e3fe2f0486b6ecd5c86a0466c3a21fa5cfcec", &player);
	dojo_bindings::FieldElement katana;

	dojo_bindings::ToriiClient* client;
	dojo_bindings::Provider* provider;
	dojo_bindings::Account* account;

	dojo_bindings::ResultToriiClient resClient = dojo_bindings::client_new(torii_url, relay_url, world);
	if (resClient.tag == dojo_bindings::ErrToriiClient)
	{
		UtilityFunctions::printerr("Error: ", resClient.err.message);
		// UtilityFunctions::push_error("Error: ", resClient.err.message);
		return;
	}
	else
	{
		UtilityFunctions::print_verbose("Client creado");
		client = resClient.ok;
		// UtilityFunctions::print_verbose("Client creado");
	}
	dojo_bindings::ResultProvider resProvider = dojo_bindings::provider_new(rpc_url);
	if (resProvider.tag == dojo_bindings::ErrProvider)
	{
		UtilityFunctions::printerr("Error: ", resProvider.err.message);
		// UtilityFunctions::push_error("Error: ", resProvider.err.message);
		return;
	}else
	{
		UtilityFunctions::print_verbose("Provider creado");
		provider = resProvider.ok;
	}
	dojo_bindings::FieldElement player_signing_key;
	hex_to_bytes("0xc5b2fcab997346f3ea1c00b002ecf6f382c5f9c9659a3894eb783c5320f912", &player_signing_key);
	dojo_bindings::ResultAccount resAccount = dojo_bindings::account_new(provider, player_signing_key, "0x127fd5f1fe78a71f8bcd1fec63e3fe2f0486b6ecd5c86a0466c3a21fa5cfcec");
	if (resAccount.tag == dojo_bindings::ErrAccount)
	{
		UtilityFunctions::printerr("Error: ", resAccount.err.message);
		return;
	}else
	{
		UtilityFunctions::print_verbose("Account creado");
		account = resAccount.ok;
	}

	dojo_bindings::FieldElement addr = account_address(account);

	if (addr.data == addr.data)
	{
		UtilityFunctions::print_verbose("Es igual");
	}




}