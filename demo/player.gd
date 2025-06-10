extends Node2D

@onready var label: Label = $Label
@onready var username: Label = $Username

func _process(delta: float) -> void:
	var pos := position / Vector2(64,64)
	$Label.text = "%dx%d"%[roundi(pos.x),roundi(pos.y)]

func move(pos):
	position = pos
	get_node("Camera2D").position = Vector2(0, 0)
