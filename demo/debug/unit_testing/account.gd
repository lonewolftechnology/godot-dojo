extends Node

const rpc_url = "https://api.cartridge.gg/x/godot-demo-rookie/katana"
const torii_url = "https://api.cartridge.gg/x/godot-demo-rookie/torii"
const contract_address = "0x038c3535090be3807d95354db3ae8dc0ceb19a0240739a3338cbc0d6c8ee47b4"
const world_address = "0x073c2b317136214562b608523812f369a05efe67257842a17c4985ce6d390be7"

# First account of contract
const account_address = "0x127fd5f1fe78a71f8bcd1fec63e3fe2f0486b6ecd5c86a0466c3a21fa5cfcec"
const private_key = "0xc5b2fcab997346f3ea1c00b002ecf6f382c5f9c9659a3894eb783c5320f912"

# The key is the selector and the array contains the data to validate to
const tests = {
	"validate_i8": [0,1,2,3],
	"validate_i16": [0,1,2,3],
	"validate_i32": [0,1,2,3],
	"validate_i64": [0,1,2,3],
	"validate_i128": [0,1,2,3],
	"validate_u8": [0,1,2,3],
	"validate_u16": [0,1,2,3],
	"validate_u32": [0,1,2,3],
	"validate_u64": [0,1,2,3],
	"validate_u128": [0,1,2,3],
	"validate_u256": [0,1,2,3],
	#"validate_bool": [true, false],
	#"validate_felt252": [],
	#"validate_class_hash": [],
	#"validate_contract_address": [],
	#"validate_eth_address": [],
}

@onready var account: Account = $Account
@onready var torii_client: ToriiClient = $ToriiClient

@export var entity_sub:EntitySubscription
@export var event_message_sub:MessageSubscription

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")
	torii_client.torii_url = torii_url
	torii_client.world_address = world_address
	torii_client.create_client()

func _on_events(args:Dictionary) -> void:
	push_warning("CALLBACK EVENTS", args)

func _on_events_message(args:Dictionary) -> void:
	push_warning("CALLBACK EVENTS MESSAGE", args)

func _on_torii_client_client_connected(success: bool) -> void:
	# Por alguna razón, las subscripciones quedan bloqueadas.
	# Nada cambió, esperemos que sea cosa del slot
	
	#await get_tree().create_timer(0.1).timeout
	#torii_client.on_entity_state_update(_on_events, entity_sub)
	#await get_tree().create_timer(0.1).timeout
	#torii_client.on_event_message_update(_on_events_message, event_message_sub)
	
	await get_tree().create_timer(0.1).timeout
	account.create(rpc_url, account_address, private_key)
	await get_tree().create_timer(0.1).timeout
	account.set_block_id("0x0")
	await get_tree().create_timer(0.1).timeout
	for selector in tests.keys():
		var data = tests[selector]
		for calldata in data:
			account.execute_raw(contract_address, selector, [calldata])


func _on_account_transaction_failed(error_message: String) -> void:
	print_rich("[color=Red]Error: [/color]", error_message)
