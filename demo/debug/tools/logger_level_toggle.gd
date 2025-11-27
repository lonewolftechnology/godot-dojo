@tool
class_name LoggerLevelToggle
extends CheckButton

@export_enum("error", "warning", "info", "debug", "success") var _level = "":
	set(value):
		_level = value
		text = _level.to_pascal_case()
		name = text + "Status"

func _ready() -> void:
	if Engine.is_editor_hint(): return
	set_pressed_no_signal(_get_level_status())
	toggled.connect(_set_btn_level)

func _get_level_status() -> bool:
	return DojoHelpers.get_log_level_enabled(_level)
	
func _set_btn_level(_pressed:bool):
	DojoHelpers.set_log_level_enabled(_level, _pressed)
	print_rich("[color=Green]%s[/color] log level changed to [color=Yellow]%s[/color]" % [_level.to_upper(),_pressed])