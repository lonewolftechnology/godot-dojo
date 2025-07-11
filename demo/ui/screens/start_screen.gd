extends Control

signal entered

@onready var button_enter: Button = $CenterContainer/VBoxContainer/ButtonEnter

func _ready() -> void:
	if not Engine.is_editor_hint(): show()
	button_enter.disabled = true
	if Connection.is_node_ready():
		Connection.connected.connect(_on_connected)
		button_enter.disabled = false

func _on_connected() -> void:
	entered.emit()
	self.hide()

func _on_button_enter_pressed() -> void:
	Connection.connect_client()
