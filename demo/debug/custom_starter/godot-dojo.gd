class_name DojoTesting
extends Node

signal torii_connected(_success: bool)
signal session_connected(_success: bool)
signal sub_created(_type: String)

@onready var dojo_session_account: DojoSessionAccount = $DojoSessionAccount
@onready var torii_client:ToriiClient = $ToriiClient

@onready var _client_status: DojoStatusIndicator = %ClientStatus
@onready var _session_account_status: DojoStatusIndicator = %SessionStatus
@onready var _entities_status: DojoStatusIndicator = %EntityStatus

@onready var _session_testing: SessionTesting = %SessionTesting

func _ready() -> void:
	_session_testing.setup()

func _on_save_log_pressed() -> void:
	ProjectSettings.save()

func _on_sub_created(_type: String) -> void:
	if _type == "entity":
		_entities_status.set_status(true)
