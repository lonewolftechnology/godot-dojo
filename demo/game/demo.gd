class_name DemoGame
extends Node

@export var spawn_call:DojoCall
@export var spawn_reset_call:DojoCall


@onready var label_username: Label = %LabelUsername
@onready var label_address: Label = %LabelAddress

@onready var controllers_manager: ControllersManager = %ControllersManager

func _on_start_screen_pressed_enter() -> void:
	Connection.create_subscriptions(_on_events,_on_entities)
	Connection.controller_account.current_user_info.connect(_on_controller_current_user_info)
	spawn(true)

func _on_controller_current_user_info(data:Dictionary) -> void:
	var address : String = data['address']
	var username : String = data['username']
	
	label_username.text = "Username: %s"%username
	label_address.text = "Address: %s"%address
	
	controllers_manager.spawn_entity(data,true)


func _on_events(args:Array):
	push_warning("Subscription Update: EVENT", args)

func _on_entities(args:Array):
	push_warning("Subscription Update: ENTITIES", args)


func spawn(reset:bool) -> void:
	if reset:
		Connection.controller_account.execute_from_outside(spawn_reset_call)
	else:
		Connection.controller_account.execute_from_outside(spawn_call)
