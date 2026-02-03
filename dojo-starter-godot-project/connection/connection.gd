class_name DojoConnection
extends Node

signal connected
signal session_created(_data:Dictionary)
# As of time of writing, November 19 2025
# This world and contract address refers to (https://github.com/dojoengine/dojo-starter), Dojo v1.8.0
const WORLD_CONTRACT = "0x6560acafbd6c554e7458234c3bcf5636a18c72a6afe3099162fb8bdb12f4ef7"
const ACTIONS_CONTRACT = "0x16de9d5d6d7f39930330d9cccb224d741794b3723cb7d50ddf23c4837fe3360"

const rpc_url = "https://api.cartridge.gg/x/godot-dojo-starter/katana"
const torii_url = "https://api.cartridge.gg/x/godot-dojo-starter/torii"

var query : DojoQuery
var entity_sub : int
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
	#TODO: Refactor to make more readable
	var _result:bool = torii_client.connect(torii_url)
	_on_torii_client_client_connected(_result)

func connect_session() -> void:
	session_account.create_from_subscribe(
		priv_key,
		rpc_url,
		"https://api.cartridge.gg"
	)
	session_created.emit(session_account.get_info())

func create_subscriptions(entities:Callable) -> void:
	var _entities_callback:DojoCallback = DojoCallback.new()
	_entities_callback.on_update = entities
	entity_sub = torii_client.subscribe_entity_updates(DojoClause.new(), [WORLD_CONTRACT], _entities_callback)

func _ready() -> void:	
	# For debugging purposes
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")

	DisplayServer.enable_for_stealing_focus(OS.get_process_id())
	get_window().focus_entered.connect(_on_window_focus)
	
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

func _on_torii_client_client_connected(success: bool) -> void:
	_client_status.set_status(success)
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
		priv_key = ControllerHelper.generate_private_key()
	
		
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
