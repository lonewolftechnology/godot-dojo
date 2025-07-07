# CHAIN ID WP_GODOT_DEMO_ROOKIE
# TORII https://api.cartridge.gg/x/godot-demo-rookie/torii
# WORLD 0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5
# Actions 0x0393f8a2d0d47e384c3c61eedc08d2873f5d608f8da7ffb013e5d5aa327ac8f2

class_name DemoGame
extends Node


enum Directions {
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
}

@export var spawn_call : DojoCall
@export var spawn_reset_call : DojoCall
@export var move_call : DojoCall
@export var move_to : DojoCall


@onready var label_username: Label = %LabelUsername
@onready var label_address: Label = %LabelAddress
@onready var label_moves: Label = %LabelMoves

@onready var controllers_manager: ControllersManager = %ControllersManager

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")
	Connection.controller_account.current_user_info.connect(_on_controller_current_user_info)

func _on_controller_current_user_info(data:Dictionary) -> void:
	var address : String = data['address']
	var username : String = data['username']
	
	label_username.text = "Username: %s"%username
	#label_address.text = "Address: %s"%address
	
	controllers_manager.spawn_entity(data,true)


func _on_events(args:Dictionary):
	var data = args["data"]
	
	# This is contract specific
	var result_data : Dictionary
	for entry : Dictionary in data:
		result_data.merge(entry)
		
	print("$$$ EVENTS: %s"%str(result_data))
	if result_data.has("Vec2"):
		await get_tree().process_frame
		var new_pos = Vector2(result_data['Vec2']['x'], result_data['Vec2']['y'] )
		controllers_manager.move_controller(result_data['player'], new_pos)
	if result_data.has("remaining"):
		await get_tree().process_frame
		label_moves.text = "Moves: %s" % result_data['remaining']

func _on_entities(args:Dictionary):
	var data = args["data"]
	# This is contract specific and data structure may change
	var result_data : Dictionary
	for entry : Dictionary in data:
		result_data.merge(entry)
	
	print("$$$ ENTITIES: %s"%str(result_data))
	if result_data.has("Vec2"):
		await get_tree().process_frame
		var new_pos = Vector2(result_data['Vec2']['x'], result_data['Vec2']['y'] )
		controllers_manager.move_controller(result_data['player'], new_pos)
	if result_data.has("remaining"):
		await get_tree().process_frame
		label_moves.text = "Moves: %s" % result_data['remaining']

func spawn(reset:bool) -> void:
	if reset:
		Connection.controller_account.execute_from_outside(spawn_reset_call)
	else:
		Connection.controller_account.execute_from_outside(spawn_call)


func _on_start_screen_entered() -> void:
	await get_tree().create_timer(0.5).timeout
	Connection.create_subscriptions(_on_events,_on_entities)
	await get_tree().create_timer(0.5).timeout
	spawn(false)
	await get_tree().create_timer(0.5).timeout
	get_controllers()
	await get_tree().create_timer(0.5).timeout
	get_entities()


func get_controllers() -> void:
	var addrs:Array = []
	#addrs.append(Connection.controller_account.get_address())
	var data = Connection.client.get_controllers(addrs)
	for controller in data:
		controllers_manager.spawn_entity(controller)
	
	await get_tree().create_timer(0.2).timeout
	print(controllers_manager.get_child_count())

func get_entities() -> void:
	var query = {
		"pagination":{
			"limit": 10,
			"cursor": "",
			"order_by": [],
			"direction": ToriiClient.QueryPaginationDirection.FORWARD
		},
		"clause": null,
		"no_hashed_keys": true,
		"models":[],
		"historical": false
	}
	var data = Connection.client.get_entities(query)
	print("$$$ moving controllers")
	for entity in data:
		for model:Dictionary in entity.models:
			var id
			var position
			var can_move
			var remaining
			for key in model:
				var entry:Dictionary = model[key]
				if entry.has("Vec2"):
					id = entry["player"]
					var vec = entry["Vec2"]
					position = Vector2(vec['x'], vec['y'])
					controllers_manager.move_controller(id, position)
				if entry.has("can_move"):
					if entry.has("can_move"):
						can_move = entry["can_move"]
					if entry.has("remaining"):
						remaining = entry["remaining"]
						
func _move(dir:Directions) -> void:
	move_call.calldata[0] = dir
	var steps:String = %StepsAmount.text
	move_call.calldata[1] = steps
	Connection.controller_account.execute_from_outside(move_call)

func _on_arrow_left_pressed() -> void:
	_move(Directions.LEFT)

func _on_arrow_up_pressed() -> void:
	_move(Directions.UP)

func _on_arrow_down_pressed() -> void:
	_move(Directions.DOWN)

func _on_arrow_right_pressed() -> void:
	_move(Directions.RIGHT)


func _on_move_to_pressed() -> void:
	var x:String = %Vx.text
	var y:String = %Vy.text
	if x.is_empty() or float(x) < 0:
		x = "0"
	if y.is_empty() or float(y) < 0:
		y = "0"
	move_to.calldata[0][0] = x
	move_to.calldata[0][1] = y
	Connection.controller_account.execute_from_outside(move_to)


func _on_disconnect_pressed() -> void:
	Connection.controller_account.disconnect_controller()
	controllers_manager.clear_all_controllers()
	await get_tree().process_frame
	
	get_tree().reload_current_scene()
