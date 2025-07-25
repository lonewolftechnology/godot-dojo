class_name ChatBox
extends Control

@onready var label: RichTextLabel = %Label
@onready var text_input: LineEdit = %TextInput


func _on_text_input_text_submitted(new_text: String) -> void:
	Connection.client.publish_message(new_text,[Connection.WORLD_CONTRACT])
	text_input.clear()
