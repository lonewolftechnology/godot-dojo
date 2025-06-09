extends Node2D

@onready var label: Label = $Label

func _process(delta: float) -> void:
	var pos := position / Vector2(64,64)
	$Label.text = "%dx%d"%[roundi(pos.x),roundi(pos.y)]
