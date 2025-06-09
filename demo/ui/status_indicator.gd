@tool
class_name DojoStatusIndicator
extends HBoxContainer

const COLOR_TRUE := Color.GREEN
const COLOR_FALSE := Color.RED

@export var type_name : String = "Type" :
	set(val):
		type_name = val
		if is_instance_valid($Type):
			$Type.text = type_name

@onready var status: ColorRect = $Status


func set_status(value:bool) -> void:
	if value:
		status.color = COLOR_TRUE
	else:
		status.color = COLOR_FALSE
