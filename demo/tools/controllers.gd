class_name ControllersManager
extends Node2D

@export var generic_entity_scene:PackedScene
@export var player_entity_scene:PackedScene
var player:PlayerEntity

var camera:Camera2D:
	get:
		if player:
			return player.camera
		else:
			return get_viewport().get_camera_2d()

func find_user(id:String) -> GenericEntity:
	var controller:Array = get_children().filter(
		func(c): return c.id == id
		)
	if controller.is_empty():
		print("NOT FOUND")
		return null
	else:
		print("FOUND")
		return controller[0]

func move_controller(id:String, vec:Vector2)->void:
	var entity = find_user(id)
	await get_tree().process_frame
	if entity:
		await get_tree().process_frame
		entity.move(vec)

func spawn_entity(_data:Dictionary, is_player:bool = false):
	var new_entity:GenericEntity
	if is_player:
		new_entity = player_entity_scene.instantiate()
		player = new_entity
	else:
		new_entity = generic_entity_scene.instantiate()
	await get_tree().process_frame
	add_child(new_entity)
	new_entity.setup(_data)

func _unhandled_input(event: InputEvent) -> void:
				
	if event.is_action_pressed("zoom_in"):
		camera.zoom += Vector2(0.1, 0.1)

	if event.is_action_pressed("zoom_out"):
		camera.zoom -= Vector2(0.1, 0.1)

	if event is InputEventMouseMotion && Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		camera.position -= event.relative * 1/camera.zoom
		
