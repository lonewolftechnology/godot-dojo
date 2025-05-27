@tool
extends Node

const dev_world_addr = "0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5"
const dev_actions_addr = "0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf"

@onready var dojo:DojoC = DojoC.new()
@onready var spawn_output: Label = $HBoxContainer/WorldConfig/SpawnOutput

@onready var input_controller: TextEdit = $HBoxContainer/WorldConfig/InputController
@onready var input_world_addr: TextEdit = $HBoxContainer/WorldConfig/InputWorldAddr
@onready var status: Label = $HBoxContainer/WorldConfig/Status

@export_global_file("Scarb.toml") var dojo_starter_path:String
@export_global_file("sozo") var sozo_path:String

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")

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
	#dojo.call_deferred("testing")
	dojo.testing()
	await get_tree().create_timer(2).timeout
	_on_spawn_pressed()
	await get_tree().create_timer(2).timeout
	dojo.output_message.any(
		func(_msg):
			print_rich("[color=Coral]value: ", _msg, "type: ", type_string(typeof(_msg)))
	)

func _on_spawn_pressed() -> void:
	var args:Array = [
		"execute",
		"dojo_starter-actions",
		"spawn",
		"--manifest-path",
		dojo_starter_path
	]
	var out:Array = []
	OS.execute(sozo_path,args,out,true)
	out.any(func(c): spawn_output.text += c)
