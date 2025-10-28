class_name ChatBox
extends Control

@onready var label: RichTextLabel = %Label
@onready var text_input: LineEdit = %TextInput

@export var connection : DojoConnection

func _on_text_input_text_submitted(new_text: String) -> void:
	if connection != null:
		connection.client.publish_message(new_text,[DojoConnection.WORLD_CONTRACT])
		text_input.clear()
