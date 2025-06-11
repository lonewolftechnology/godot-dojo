class_name PlayerEntity
extends GenericEntity
@onready var camera: Camera2D = $Camera2D

func _setup(entity_data:Dictionary) -> void:
	camera.make_current()
