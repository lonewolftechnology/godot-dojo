class_name DojoConnection
extends Node

signal connected
signal session_created(_data:Dictionary)
# As of time of writing, September 1 2025
# This are from our contract deployend on slot (https://github.com/dannroda/dojo-starter) Dojo v1.7.0-alpha.3
# But it should work with official starter (https://github.com/dojoengine/dojo-starter), Dojo v1.6.2
const WORLD_CONTRACT = "0x03a7e28319f3617da4135893c711c79a1306adcd87cca4fbd0ceda50ae397683"
const ACTIONS_CONTRACT = "0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da"

#TODO: Make not neccesary to setup ToriiClient/ControllearAccount through editor, so can be easily updated with code.
# The chain_id can be obtained using a burner Account
const rpc_url = "https://api.cartridge.gg/x/godot-demo-rookie/katana"
const torii_url = "https://api.cartridge.gg/x/godot-demo-rookie/torii"

@export var query:DojoQuery
@export var entity_sub:EntitySubscription
@export var message_sub:MessageSubscription

@onready var client: ToriiClient = $ToriiClient
@onready var session_account: DojoSessionAccount = $DojoSessionAccount

@onready var _client_status: DojoStatusIndicator = %ClientStatus
@onready var _session_account_status: DojoStatusIndicator = %DojoSessionAccountStatus

@onready var _events_status: DojoStatusIndicator = %EventUpdatesStatus
@onready var _entities_status: DojoStatusIndicator = %EntityUpdatesStatus

@export_placeholder("0x0") var priv_key:String

@onready var url_open:bool = false


var policies:Dictionary = {
		"max_fee": "0x100000",
		"policies": [{
			"contract_address": ACTIONS_CONTRACT,
			"entrypoints": [
				"spawn",
				"move"
			]
		}]
	}

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")
	DisplayServer.enable_for_stealing_focus(OS.get_process_id())
	get_window().focus_entered.connect(_on_window_focus)
	
	entity_sub.world_addresses = [WORLD_CONTRACT]
	message_sub.world_addresses = [WORLD_CONTRACT]

func _on_window_focus():
	if url_open and not session_account.is_valid():
		connect_session()
		if session_account.is_valid():
			_session_account_status.set_status(true)
			connected.emit()
		url_open = false

func _torii_logger(_msg:String):
	prints("[TORII LOGGER]", _msg)

func connect_client() -> void:
	client.create_client(torii_url)

func connect_session() -> void:
	session_account.create_from_subscribe(
		priv_key,
		policies,
		rpc_url,
		"https://api.cartridge.gg"
	)
	session_created.emit(session_account.get_info())

func _on_torii_client_client_connected(success: bool) -> void:
	_client_status.set_status(success)
	client.set_logger_callback(_torii_logger)
	if success:
		url_open = true
		var session_url: String = get_session_url()
		OS.shell_open(session_url)

func _on_torii_client_client_disconnected() -> void:
	_client_status.set_status(false)

func _on_session_account_controller_connected(success: bool) -> void:
	_session_account_status.set_status(success)
	if success:
		connected.emit()


func _on_session_account_controller_disconnected() -> void:
	if not _session_account_status: return
	_session_account_status.set_status(false)

func _on_torii_client_subscription_created(subscription_name: String) -> void:
	if subscription_name == "entity_state_update":
		_entities_status.set_status(true)
	if subscription_name == "event_message_update":
		_events_status.set_status(true)

func create_subscriptions(events:Callable,entities:Callable) -> void:
	client.on_entity_state_update(entities, entity_sub)
	client.on_event_message_update(events, message_sub)
	

func get_session_url() -> String:
	if priv_key.is_empty():
		priv_key = DojoHelpers.generate_private_key()
		
	var base_url = "https://x.cartridge.gg/session"
	var public_key = ControllerHelper.get_public_key(priv_key)
	var redirect_uri = "about:blank"
	var redirect_query_name = "startapp"
	
	var policies = {
		"contracts": {
			ACTIONS_CONTRACT: {
				"methods": [
					{
						"name" : "Spawn in world", # Optional
						"description": "Can Spawn in the world", # Optional
						"entrypoint": "spawn",
					},
					{
						"entrypoint": "move",
					}
				]
			}
		}
	}

	var session_url = DojoSessionAccount.generate_session_request_url(
		base_url, 
		public_key, 
		policies, 
		rpc_url, 
#		redirect_uri, # Optional
#		redirect_query_name # Optional
		)
		
	return session_url