extends Node2D

func find_user(id:String) -> GenericEntity:
	var controller:Array = get_children().filter(
		func(c): return c.id == id
		)
	if controller.is_empty():
		print("NOT FOUND")
		return null
	else:
		print("FOUND")
		return controller[0]

func move_controller(id:String, vec:Vector2)->void:
	var entity = find_user(id)
	await get_tree().process_frame
	if entity:
		await get_tree().process_frame
		entity.move(vec)
