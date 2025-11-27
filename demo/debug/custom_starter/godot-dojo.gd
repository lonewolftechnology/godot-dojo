extends Node
@onready var dojo_session_account: DojoSessionAccount = $DojoSessionAccount
@onready var torii_client:ToriiClient = $ToriiClient

@onready var _client_status: DojoStatusIndicator = %ClientStatus
@onready var _session_account_status: DojoStatusIndicator = %SessionStatus
@onready var _events_status: DojoStatusIndicator = %EventStatus
@onready var _entities_status: DojoStatusIndicator = %EntityStatus

@onready var _session_testing: SessionTesting = %SessionTesting

func _ready() -> void:
	_session_testing.setup()

func _on_torii_client_client_connected(success: bool) -> void:
	_client_status.set_status(success)


func _on_session_testing_session_connected() -> void:
	_session_account_status.set_status(true)


func _on_session_testing_subscription_created(subscription_name: String) -> void:
	if subscription_name == "entity_state_update":
		_entities_status.set_status(true)
	if subscription_name == "event_message_update":
		_events_status.set_status(true)
