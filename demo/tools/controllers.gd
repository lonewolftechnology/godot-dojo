class_name ControllersManager
extends Node2D

const MAX_ZOOM := 2.0
const MIN_ZOOM := 0.2

@export var generic_entity_scene:PackedScene
@export var player_entity_scene:PackedScene

var player:PlayerEntity
var camera:Camera2D:
	get:
		if player:
			return player.camera
		else:
			return get_viewport().get_camera_2d()

var entities:Dictionary = {}

func get_entity(id:String) -> GenericEntity:
	return _find_user(id)

func _find_user(id:String) -> GenericEntity:
	var children = get_children()
	var filter = children.filter(
		func(c): return c.id == id
	)
	if filter.is_empty():
		return null
	else:
		return filter[0]
		
func move_controller(id:String, pos:Vector2)->void:
	var entity = _find_user(id)
	prints("Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", entity)
	if entity != null:
		entity.move(pos)

func spawn_entity(id:String, is_player:bool = false) -> GenericEntity:
	if _find_user(id): 
		return null
	
	var new_entity : GenericEntity
	if is_player:
		new_entity = player_entity_scene.instantiate()
		player = new_entity
	else:
		new_entity = generic_entity_scene.instantiate()
	
	#await get_tree().process_frame
	add_child(new_entity)
	new_entity.setup(id)
	entities[id] = new_entity
	return new_entity

func _unhandled_input(event: InputEvent) -> void:
	if event.is_action_pressed("zoom_in"):
		if camera.zoom.x <= MAX_ZOOM:
			camera.zoom += Vector2(0.1, 0.1)

	if event.is_action_pressed("zoom_out"):
		if camera.zoom.x >= MIN_ZOOM:
			camera.zoom -= Vector2(0.1, 0.1)

	if event is InputEventMouseMotion && Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		camera.position -= event.relative * 1/camera.zoom

func clear() -> void:
	get_children().any(
		func(c:Node2D):
			c.queue_free()
	)
