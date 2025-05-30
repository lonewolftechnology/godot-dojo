@tool
extends Node

const dev_world_addr = "0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5"
const dev_actions_addr = "0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf"

@onready var dojo:DojoC = DojoC.new()

@onready var status: Label = $"TabContainer/WIP tests/Status"
@onready var input_world_addr: TextEdit = $"TabContainer/WIP tests/InputWorldAddr"
@onready var input_controller: TextEdit = $"TabContainer/WIP tests/InputController"

@onready var provider_status: HBoxContainer = $TabContainer/SpawnTest/HBoxContainer3/ProviderStatus
@onready var account_status: HBoxContainer = $TabContainer/SpawnTest/HBoxContainer3/AccountStatus
@onready var suscription_status: HBoxContainer = $TabContainer/SpawnTest/HBoxContainer3/SuscriptionStatus
@onready var spawn_output: RichTextLabel = $TabContainer/SpawnTest/ScrollContainer/SpawnOutput



func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	dojo.event_update.connect(_on_event_update)
	dojo.account_status_updated.connect(update_status.bind(provider_status))
	dojo.provider_status_updated.connect(update_status.bind(account_status))
	dojo.subscription_status_updated.connect(update_event_subscription_status.bind(suscription_status))
	


func _on_button_pressed() -> void:
	if input_world_addr.text.is_empty():
		input_world_addr.text = dev_world_addr
	status.text = dojo.create_client(input_world_addr.text)
	print(dojo.is_enabled)
	
func _on_connect_controller_pressed() -> void:
	if input_controller.text.is_empty():
		input_controller.text = dev_actions_addr
	dojo.controller_connect(input_controller.text)

func _on_testing_pressed() -> void:
	dojo.testing()

func _on_spawn_pressed() -> void:
	dojo.spawn()

func _on_event_update(_msg:String):
	#_msg.any(func(c): spawn_output.text += c)
	spawn_output.append_text(_msg)
	spawn_output.newline()

func update_event_subscription_status(_value:bool, _event:String, _status_node:HBoxContainer):
	update_status(_value, _status_node)

func update_status(_value:bool, _status_node:HBoxContainer):
	if _value:
		_status_node.get_node("Status").color = Color.GREEN
	else:
		_status_node.get_node("Status").color = Color.RED
