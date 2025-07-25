class_name GenericEntity
extends Node2D

const STEP_SIZE := Vector2(64,64) * 2

@onready var username_label: Label = $Username
@onready var position_label: Label = $Position

var username:String:
	set(value):
		username = value
		if username.length() > 16:
			username_label.text = username.substr(0,16)+"..."
		else:
			username_label.text = username
		
		
var id:String

func setup(entity_data:Dictionary) -> void:
	id = entity_data['address']
	if entity_data.has("username"):
		username = entity_data['username']
	else:
		username = id
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
