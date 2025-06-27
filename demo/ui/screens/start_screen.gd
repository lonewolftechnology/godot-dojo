extends Control

signal pressed_enter

@onready var button_enter: Button = $CenterContainer/VBoxContainer/ButtonEnter

func _ready() -> void:
	button_enter.disabled = true
	if Connection.is_node_ready():
		Connection.connected.connect(_on_connected)
		button_enter.disabled = false

func _on_connected() -> void:
	pressed_enter.emit()
	self.hide()
	button_enter.disabled = false

func _on_button_enter_pressed() -> void:
	Connection.connect_client()
