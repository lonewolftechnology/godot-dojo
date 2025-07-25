class_name PlayerEntity
extends GenericEntity

@onready var camera: Camera2D = $Camera2D

func _move(pos):
	camera.position = Vector2()


func _setup(entity_data:Dictionary) -> void:
	camera.make_current()
