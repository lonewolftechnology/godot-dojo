class_name GenericEntity
extends Node2D

const STEP_SIZE := Vector2(128,128)

@onready var username_label: Label = $Username
@onready var position_label: Label = $Position

var username : String = ""
var id : String = ""

func setup(_id:String) -> void:
	id = _id
	set_meta("type", name)
	_setup(id)

func set_username(uname:String) -> void:
	username = uname
	set_meta("username",username)
	if username.length() > 16:
		username_label.text = username.substr(0,16)+"..."
	else:
		username_label.text = username

func _process(delta: float) -> void:
	var pos := position / STEP_SIZE
	position_label.text = "%dx%d"%[roundi(pos.x),roundi(pos.y)]

func move(pos:Vector2) -> void:
	#await get_tree().process_frame
	position = pos * STEP_SIZE
	#await get_tree().process_frame
	_move(position)
	
#Override
func _move(pos:Vector2) -> void:
	pass

#Override
func _setup(id:String) -> void:
	pass
