class_name DojoConnection
extends Node

signal connected
# As of time of writing, September 1 2025
# This are from our contract deployend on slot (https://github.com/dannroda/dojo-starter) Dojo v1.7.0-alpha.3
# But it should work with official starter (https://github.com/dojoengine/dojo-starter), Dojo v1.6.2
const WORLD_CONTRACT = "0x044508ff3a94c19844702f34dd64c5a933870116a42ac1105fd324335ca7b3f9"
const ACTIONS_CONTRACT = "0x00123e4e8a65b4e1f7c6004d99764e89d065bb1ec479ad41b1c195a19d325a3f"

#TODO: Make not neccesary to setup ToriiClient/ControllearAccount through editor, so can be easily updated with code.
# The chain_id can be obtained using a burner Account
const rpc_url = "https://api.cartridge.gg/x/godot-demo-rookie/katana"
const torii_url = "https://api.cartridge.gg/x/godot-demo-rookie/torii"

@export var query:DojoQuery
@export var entity_sub:EntitySubscription
@export var message_sub:MessageSubscription

@onready var client: ToriiClient = $ToriiClient
@onready var controller_account: ControllerAccount = $ControllerAccount

@onready var _client_status: DojoStatusIndicator = %ClientStatus
@onready var _provider_status: DojoStatusIndicator = %ProviderStatus
@onready var _controller_account_status: DojoStatusIndicator = %ControllerAccountStatus

@onready var _events_status: DojoStatusIndicator = %EventUpdatesStatus
@onready var _entities_status: DojoStatusIndicator = %EntityUpdatesStatus

func _ready() -> void:
	var dojoc = DojoC.new()
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")

func _torii_logger(_msg:String):
	prints("[TORII LOGGER]", _msg)

func connect_client() -> void:
	client.create_client()

func connect_controller() -> void:
	controller_account.setup()

func _on_torii_client_client_connected(success: bool) -> void:
	_client_status.set_status(success)
	client.set_logger_callback(_torii_logger)
	if success:
		connect_controller()

func _on_torii_client_client_disconnected() -> void:
	_client_status.set_status(false)

func _on_controller_account_controller_connected(success: bool) -> void:
	_controller_account_status.set_status(success)
	if success:
		connected.emit()


func _on_controller_account_controller_disconnected() -> void:
	_controller_account_status.set_status(false)

func _on_controller_account_provider_status_updated(success: bool) -> void:
	_provider_status.set_status(success)

func _on_torii_client_subscription_created(subscription_name: String) -> void:
	if subscription_name == "entity_state_update":
		_entities_status.set_status(true)
	if subscription_name == "event_message_update":
		_events_status.set_status(true)

func create_subscriptions(events:Callable,entities:Callable) -> void:
	client.on_entity_state_update(entities, entity_sub)
	client.on_event_message_update(events, message_sub)
	
