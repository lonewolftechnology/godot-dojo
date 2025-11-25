class_name DojoConnection
extends Node

signal connected
signal session_created(_data:Dictionary)
# As of time of writing, November 19 2025
# This world and contract address refers to (https://github.com/dojoengine/dojo-starter), Dojo v1.8.0
const WORLD_CONTRACT = "0x026d5777eccca1861a23303ee0ba48c0e8349e849d0377a21c3801ef1d0f8cef"
const ACTIONS_CONTRACT = "0x01ae76ae877518f905b73b643d96731e71e6b848ba68824adb4f1a77760fc3c7"

const rpc_url = "https://api.cartridge.gg/x/godot-dojo-starter/katana"
const torii_url = "https://api.cartridge.gg/x/godot-dojo-starter/torii"

@export var query : DojoQuery
@export var entity_sub : EntitySubscription
@export var message_sub : MessageSubscription
@export_placeholder("0x0") var priv_key : String

@onready var torii_client: ToriiClient = $ToriiClient
@onready var session_account: DojoSessionAccount = $DojoSessionAccount
@onready var _client_status: DojoStatusIndicator = %ClientStatus
@onready var _session_account_status: DojoStatusIndicator = %DojoSessionAccountStatus
@onready var _events_status: DojoStatusIndicator = %EventUpdatesStatus
@onready var _entities_status: DojoStatusIndicator = %EntityUpdatesStatus
@onready var url_open : bool = false
@onready var http_tools: HttpTools = $HttpTools

var full_policies:Dictionary = {
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

func connect_client() -> void:
	torii_client.create_client(torii_url)

func connect_session() -> void:
	session_account.create_from_subscribe(
		priv_key,
		rpc_url,
		"https://api.cartridge.gg"
	)
	session_created.emit(session_account.get_info())

func create_subscriptions(events:Callable,entities:Callable) -> void:
	torii_client.on_entity_state_update(entities, entity_sub)
	torii_client.on_event_message_update(events, message_sub)

func _ready() -> void:	
	# For debugging purposes
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")

	DisplayServer.enable_for_stealing_focus(OS.get_process_id())
	get_window().focus_entered.connect(_on_window_focus)
	
	entity_sub.world_addresses = [WORLD_CONTRACT]
	message_sub.world_addresses = [WORLD_CONTRACT]

	session_account.max_fee = "0x100000"
	session_account.full_policies = full_policies

func _on_window_focus():
	# After user logs in and switches back to the game
	if url_open and not session_account.is_valid():
		connect_session()
		if session_account.is_valid():
			_session_account_status.set_status(true)
			connected.emit()
		url_open = false

func _torii_logger(_msg:String):
	prints("[TORII LOGGER]", _msg)

func _on_torii_client_client_connected(success: bool) -> void:
	_client_status.set_status(success)
	torii_client.set_logger_callback(_torii_logger)
	if success:
		url_open = true
		var session_url: String = _get_session_url()
		OS.shell_open(session_url)

func _on_torii_client_client_disconnected() -> void:
	_client_status.set_status(false)

func _on_torii_client_subscription_created(subscription_name: String) -> void:
	if subscription_name == "entity_state_update":
		_entities_status.set_status(true)
	if subscription_name == "event_message_update":
		_events_status.set_status(true)

func _get_session_url() -> String:
	if priv_key.is_empty():
		priv_key = DojoHelpers.generate_private_key()
	
		
	var base_url = "https://x.cartridge.gg/session"
	var public_key = ControllerHelper.get_public_key(priv_key)
	var redirect_uri = ""
	var redirect_query_name = ""
	
	if http_tools.start_server():
		redirect_uri = "http://localhost:%d" % http_tools.port
		redirect_query_name = "startapp" 

	var session_url = session_account.generate_session_request_url(
		base_url, 
		public_key, 
		rpc_url,
		session_account.get_register_session_policy(),
		redirect_uri, # Optional parameter
		redirect_query_name # Optional parameter
		)

	return session_url
	
func _on_save_settings_pressed() -> void:
	ProjectSettings.save()
