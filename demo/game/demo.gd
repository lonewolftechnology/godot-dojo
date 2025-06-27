class_name DemoGame
extends Node

@export var spawn_call:DojoCall
@export var spawn_reset_call:DojoCall

func _on_start_screen_pressed_enter() -> void:
	Connection.create_subscriptions(_on_events,_on_entities)
	spawn(true)


func _on_events(args:Array):
	push_warning("Updates entities EVENT", args)
	var _packed = args.filter(func(c): return c is String)
	var _vector = args.filter(func(c): return c is Vector2 )
	
	if _vector.is_empty(): return
	
	var vec:Vector2 = _vector[0]

func _on_entities(args:Array):
	push_warning("Updates entities", args)
	var _packed = args.filter(func(c): return c is String)
	var _vector = args.filter(func(c): return c is Vector2)
	if _vector.is_empty(): return
	var vec:Vector2 = _vector[0]

func spawn(reset:bool) -> void:
	if reset:
		Connection.controller_account.execute_from_outside(spawn_reset_call)
	else:
		Connection.controller_account.execute_from_outside(spawn_call)
