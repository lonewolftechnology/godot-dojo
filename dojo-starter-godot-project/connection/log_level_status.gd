class_name LogLevelStatus
extends PanelContainer

# CheckButtons must have its text as the log level

@onready var btn_list:VBoxContainer = $Vbox

func _ready() -> void:
	_init_buttons_state()
	
func _init_buttons_state():
	for _btn:CheckButton in btn_list.get_children():
		var _level:String = _get_btn_level(_btn)
		_btn.set_pressed_no_signal(_get_level_status(_level))
		_btn.toggled.connect(_set_btn_level.bind(_level))
		
func _get_btn_level(_btn:CheckButton) -> String:
	return _btn.text.to_lower()
	
func _get_level_status(_level:String) -> bool:
	return DojoHelpers.get_log_level_enabled(_level)
	
func _set_btn_level(_pressed:bool, _level:String):
	DojoHelpers.set_log_level_enabled(_level, _pressed)
	print_rich("[color=Green]%s[/color] log level changed to [color=Yellow]%s[/color]" % [_level.to_upper(),_pressed])
