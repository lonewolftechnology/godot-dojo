class_name PlayerEntity
extends GenericEntity

@onready var camera: Camera2D = $Camera2D

func _move(_pos:Vector2):
	camera.position = Vector2.ZERO

func _setup(_id:String) -> void:
	camera.make_current()
