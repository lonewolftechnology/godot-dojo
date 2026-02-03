class_name DemoGame
extends Node

enum Directions {
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
}

@export var connection : DojoConnection

var _player_address : String = ""

@onready var label_username: Label = %LabelUsername
@onready var label_address: Label = %LabelAddress
@onready var label_moves: Label = %LabelMoves
@onready var controllers_manager: ControllersManager = %ControllersManager

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")
	connection.session_created.connect(_on_controller_current_user_info)

func _on_controller_current_user_info(data:Dictionary) -> void:
	var address : String = data['address']
	var username : String = data['username']
	_player_address = address
	label_username.text = "Username: %s"%username
	var player_entity := controllers_manager.spawn_entity(address,true)
	player_entity.set_username(username)

func _on_entities(args:Dictionary) -> void:
	_handle_callback(args)

func _handle_callback(args:Dictionary) -> void:
	
	var data = args["models"]
	# This is contract specific
	#var result_data : Dictionary
	#for entry in data.keys():
		#result_data.merge(data[entry])
	await get_tree().process_frame
	if data.has("dojo_starter-Position"):
		var position_model = data["dojo_starter-Position"]
		var pos := Vector2(position_model['vec']['x'], position_model['vec']['y'] )
		var id : String = position_model['player']
		controllers_manager.move_controller(id, pos)
	if data.has("dojo_starter-Moves"):
		var moves_model = data["dojo_starter-Moves"]
		var id : String = moves_model['player']
		if id == _player_address:
			label_moves.text = "Moves: %s" % moves_model['remaining']

func spawn() -> void:
	var spawn_call = {
		"contract_address": connection.ACTIONS_CONTRACT,
		"entrypoint": "spawn",
		"calldata": []
	}
	connection.session_account.execute_from_outside([spawn_call])

func _on_start_screen_entered() -> void:
	start()

func start() -> void:
	await get_tree().create_timer(0.1).timeout
	connection.create_subscriptions(_on_entities)
	
	await get_tree().create_timer(0.1).timeout
	await get_tree().create_timer(0.1).timeout
	
	var parsed : Dictionary = get_entities()
	
	for id in parsed.keys():
		controllers_manager.spawn_entity(id)
	
	_update_entities(parsed)	
	get_controllers(parsed.keys())
	spawn()

func get_controllers(addrs:Array = []) -> void:
	var data = connection.torii_client.controllers(ControllerQuery.new())
	print()
	for controller:Dictionary in data['items']:
		var id : String = controller["address"]
		var username : String = controller["username"]
		var entity : GenericEntity = controllers_manager.get_entity(id)
		if entity == null:
			entity = controllers_manager.spawn_entity(id)
		
		entity.set_username(username)

func get_entities() -> Dictionary:
	var data = connection.torii_client.entities(DojoQuery.new())
	var parsed_entities : Dictionary = {}
	var items:Array = data["items"]
	for entity in items:
		var _models:Dictionary = entity["models"]
		for key in _models.keys():
			var id : String = ""
			var position : Vector2 = Vector2.ZERO
			var can_move : bool = true
			var remaining : int = 100
			var entry:Dictionary = _models[key]
			id = entry["player"]
			match key:
				"dojo_starter-Position":
					var v : Dictionary = {}
					
					if entry.has("vec"):
						v = entry["vec"]
					elif entry.has("Vector2"):
						v = entry["Vector2"]
					
					var x := v["x"] as float
					var y := v["y"] as float
					position = Vector2(x,y)
					
					if not parsed_entities.has(id):
						parsed_entities[id] = {}
					parsed_entities[id] = {}
					parsed_entities[id].merge({"position": position})
				
				"dojo_starter-Moves":
					remaining = entry["remaining"] as int
					can_move = entry["can_move"] as bool
					
					if not parsed_entities.has(id):
						parsed_entities[id] = {}
						
					parsed_entities[id]["remaining"] = remaining
					parsed_entities[id]["can_move"] = can_move
				
				"dojo_starter-U128Value":
					pass
				"dojo_starter-U256Value":
					pass
	return parsed_entities

func _update_entities(parsed_entities:Dictionary) -> void:
	for key in parsed_entities.keys():
		var id : String = key
		if parsed_entities[key].has("position"):
			var position : Vector2 = parsed_entities[key]["position"]
			controllers_manager.move_controller(id,position)

func _move(dir:Directions) -> void:
	var move_call = {
		"contract_address": connection.ACTIONS_CONTRACT,
		"entrypoint": "move",
		"calldata": [dir]
	}
	connection.session_account.execute([move_call])

func _on_arrow_left_pressed() -> void:
	_move(Directions.LEFT)

func _on_arrow_up_pressed() -> void:
	_move(Directions.UP)

func _on_arrow_down_pressed() -> void:
	_move(Directions.DOWN)

func _on_arrow_right_pressed() -> void:
	_move(Directions.RIGHT)

func _on_disconnect_pressed() -> void:
	connection.session_account.queue_free()
	controllers_manager.clear()
	await get_tree().process_frame	
	get_tree().reload_current_scene()
