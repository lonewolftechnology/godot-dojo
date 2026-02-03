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
#	add_theme_color_override("font_pressed_color", Color.GREEN)
#	add_theme_color_override("font_focus_color", Color.GREEN)
#	add_theme_color_override("font_hover_pressed_color", Color.GREEN)
#	add_theme_color_override("font_color", Color.RED)
#	add_theme_color_override("font_focus_color", Color.RED)
#	add_theme_color_override("font_hover_color", Color.RED)

	set_pressed_no_signal(_get_level_status())
	_update_button_color(button_pressed)

	toggled.connect(_set_btn_level)

func _get_level_status() -> bool:
	return GodotDojoHelper.get_log_level_enabled(_level)
	
func _set_btn_level(_pressed:bool):
	GodotDojoHelper.set_log_level_enabled(_level, _pressed)
	_update_button_color(_pressed)
	print_rich("[color=Green]%s[/color] log level changed to [color=Yellow]%s[/color]" % [_level.to_upper(),_pressed])

func _update_button_color(_pressed:bool):
	if _pressed:
		modulate = Color.GREEN
	else:
		modulate = Color.RED
