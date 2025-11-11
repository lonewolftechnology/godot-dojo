@tool
class_name DojoStatusIndicator
extends PanelContainer

const COLOR_TRUE := Color.GREEN
const COLOR_FALSE := Color.RED

@onready var type: Label = $StatusIndicator/Type
@onready var status: ColorRect = $StatusIndicator/Status

@export var type_name : String :
	set(val):
		type_name = val
		if is_instance_valid(type):
			type.text = type_name

func _ready() -> void:
	if not type_name.is_empty() and type.text != type_name:
		type.text = type_name

func set_status(value:bool) -> void:
	if value:
		status.color = COLOR_TRUE
	else:
		status.color = COLOR_FALSE
