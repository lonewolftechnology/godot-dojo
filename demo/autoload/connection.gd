#Autoload Connection
extends Node

signal connected

@onready var client: ToriiClient = $ToriiClient
@onready var controller_account: ControllerAccount = $ControllerAccount


@onready var _client_status: DojoStatusIndicator = %ClientStatus
@onready var _provider_status: DojoStatusIndicator = %ProviderStatus
@onready var _controller_account_status: DojoStatusIndicator = %ControllerAccountStatus

@onready var _events_status: DojoStatusIndicator = %EventUpdatesStatus
@onready var _entities_status: DojoStatusIndicator = %EntityUpdatesStatus

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")

func connect_client() -> void:
	client.create_client()

func connect_controller() -> void:
	controller_account.setup()


func _on_torii_client_client_connected(success: bool) -> void:
	_client_status.set_status(success)
	if success:
		#await get_tree().create_timer(2.0).timeout
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
	if subscription_name == "entity":
		_entities_status.set_status(true)
	if subscription_name == "event":
		_events_status.set_status(true)

func create_subscriptions(events:Callable,entities:Callable) -> void:
	# For now, COptionClause is not fullly implemented
	# So an empty dictionary is used for now 
	client.create_entity_subscription(entities, {})
	client.create_event_subscription(events, {})
