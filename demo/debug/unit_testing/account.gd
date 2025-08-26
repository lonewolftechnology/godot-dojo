extends Node

signal validation_result(result:bool, selector:String, calldata:Variant)
signal next_test

const local_rpc_url = "http://localhost:5050"
const local_torii_url = "http://localhost:8080"

const rpc_url = "https://api.cartridge.gg/x/godot-demo-rookie/katana"
const torii_url = "https://api.cartridge.gg/x/godot-demo-rookie/torii"

const contract_namespace = "dojo_starter"
const contract_address = "0x038c3535090be3807d95354db3ae8dc0ceb19a0240739a3338cbc0d6c8ee47b4"
const local_contract_address = "0x06b71e9a3aa5198aa78f7d8c407024d863e1d1263e735081c824deea349d3852" #Plain Starter
const world_address = "0x073c2b317136214562b608523812f369a05efe67257842a17c4985ce6d390be7"

# First account of contract
const account_address = "0x6677fe62ee39c7b07401f754138502bab7fac99d2d3c5d37df7d1c6fab10819"
const private_key = "0x3e3979c1ed728490308054fe357a9f49cf67f80f9721f44cc57235129e090f4"
const public_key = "0x1e8965b7d0b20b91a62fe515dd991dc9fcb748acddf6b2cf18cec3bdd0f9f9a"

# The key is the selector and the array contains the data to validate to
const to_validate = [-1,0,3.5,9]
const tests = {
	"validate_i8": to_validate,
	"validate_i16": to_validate,
	"validate_i32": to_validate,
	"validate_i64": to_validate,
	"validate_i128": to_validate,
	"validate_u8": to_validate,
	"validate_u16": to_validate,
	"validate_u32": to_validate,
	"validate_u64": to_validate,
	"validate_u128": to_validate,
	#"validate_u256": [0,1,2,3],
	#"validate_bool": [true, false],
	#"validate_felt252": [],
	#"validate_class_hash": [],
	#"validate_contract_address": [],
	#"validate_eth_address": [],
}

@onready var account: Account = $Account
@onready var torii_client: ToriiClient = $ToriiClient

@onready var output_text: RichTextLabel = %OutputText
@onready var sub_output: RichTextLabel = %SubOutput

@export var event_message_sub:MessageSubscription
@export var txn_sub:TransactionSubscription

@export var use_slot:bool = true
@export var use_custom_starter:bool = true

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")
	torii_client.world_address = world_address
	
	if use_slot:
		print_rich("[color=yellow]Using Slot[/color]")
		torii_client.torii_url = torii_url
	else:
		print_rich("[color=yellow]Using Local[/color]")
		torii_client.torii_url = local_torii_url

	torii_client.create_client()
	#print(torii_client.get_world_metadata())

func _torii_logger(_msg:String):
	prints("[TORII LOGGER]", _msg)

func _on_events_message(args:Dictionary) -> void:
	push_warning("CALLBACK EVENTS MESSAGE", args)
	sub_output.call_deferred("append_text","[color=yellow]%s[/color]\n" % args["models"])

func _on_transaction(args:Dictionary) -> void:
	push_warning("CALLBACK TXN MESSAGE", args)

func _on_torii_client_client_connected(success: bool) -> void:
	torii_client.set_logger_callback(_torii_logger)
	await get_tree().create_timer(0.1).timeout
	torii_client.on_event_message_update(_on_events_message, event_message_sub)
	await get_tree().process_frame
	torii_client.on_transaction(_on_transaction, txn_sub)

	if use_slot:
		account.create(rpc_url, account_address, private_key)
	else:
		account.create(local_rpc_url, account_address, private_key)

	account.set_block_id()

	if account.is_account_valid():
		for selector in tests.keys():
			var data:Array = tests[selector]
			if data.is_empty():
				if use_custom_starter:
					account.execute_raw(contract_address, selector)
				else:
					account.execute_raw(local_contract_address, selector)
				continue
				
			for calldata in data:
				if use_custom_starter:
					account.execute_raw(contract_address, selector, [calldata])
					await get_tree().process_frame
				else:
					account.execute_raw(local_contract_address, selector, [calldata])
					await get_tree().process_frame
				# TODO: Sequencer
				await get_tree().create_timer(0.8).timeout # Workaround for missing events
				

func _on_account_transaction_failed(error_message: Dictionary) -> void:
	output_text.append_text("[color=cyan]Selector: [/color]%s[color=yellow] Calldata: [/color] %s | [color=Red]Check: log [/color]\n" % [error_message["selector"], error_message["calldata"]] )
	push_error(error_message["error"])

func _on_account_transaction_executed(success_message: Dictionary) -> void:
	output_text.append_text("[color=cyan]Selector: [/color]%s[color=yellow] Calldata: [/color] %s [color=Green]%s[/color]\n" % [success_message["selector"], success_message["calldata"], success_message["txn"]] )
	var selector:String = success_message["selector"]
	selector = selector.replace("validate", "validated")
	var event = "%s-%s" % [contract_namespace, selector.to_pascal_case()]
	#sub_output.append_text("[color=yellow]%s[/color]\n" % event)
	
