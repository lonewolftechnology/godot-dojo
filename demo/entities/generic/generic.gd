class_name GenericEntity
extends Node2D

const STEP_SIZE := Vector2(64,64)

@onready var username_label: Label = $Username
@onready var position_label: Label = $Position

var username:String:
	set(value):
		username = value
		username_label.text = username
		
		
var id:String

func setup(entity_data:Dictionary) -> void:
	username = entity_data['username']
	id = entity_data['address']
	set_meta("type", name)
	set_meta("name", username)
	# Overwrite
	name = username
	_setup(entity_data)

func _process(delta: float) -> void:
	var pos := position / STEP_SIZE
	position_label.text = "%dx%d"%[roundi(pos.x),roundi(pos.y)]

func move(pos) -> void:
	push_warning("moving %s to %s" % [id, pos])
	await get_tree().process_frame
	position = pos * STEP_SIZE
	await get_tree().process_frame
	_move(position)
	
#Override
func _move(pos) -> void:
	pass
#Override
func _setup(entity_data:Dictionary) -> void:
	pass
