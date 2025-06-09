extends Node2D

@onready var label: Label = $Label

func _process(delta: float) -> void:
	$Label.text = "%dx%d"%[roundi(position.x),roundi(position.y)]
