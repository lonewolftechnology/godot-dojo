# CHAIN ID WP_GODOT_DEMO_ROOKIE
# KATANA https://api.cartridge.gg/x/godot-demo-rookie/katana
# TORII https://api.cartridge.gg/x/godot-demo-rookie/torii
# WORLD 0x0393f8a2d0d47e384c3c61eedc08d2873f5d608f8da7ffb013e5d5aa327ac8f2
# Actions 0x065d58039c71b4a0fe302d68f9fad4bf49e01fe518d7912d44ba0d07b2d7f556

# hazel 0x3dc821653fa9ed84324821b26afd5775b128921b9436327211949947a888
# dtodice 0x75c10973fc5fdfa2f7ff5875f10441e546d98f37f40b97a663f89bdaf81a0

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
@export var move_to_signed : DojoCall


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
	if result_data.has("Vec2Signed"):
		await get_tree().process_frame
		var new_pos = Vector2(result_data['Vec2Signed']['x'], result_data['Vec2Signed']['y'] )
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
	if result_data.has("Vec2Signed"):
		await get_tree().process_frame
		var new_pos = Vector2(result_data['Vec2Signed']['x'], result_data['Vec2Signed']['y'] )
		controllers_manager.move_controller(result_data['player'], new_pos)
	if result_data.has("remaining"):
		await get_tree().process_frame
		label_moves.text = "Moves: %s" % result_data['remaining']

func spawn(reset:bool = false) -> void:
	if reset:
		Connection.controller_account.execute_from_outside(spawn_reset_call)
	else:
		Connection.controller_account.execute_from_outside(spawn_call)


func _on_start_screen_entered() -> void:
	await get_tree().create_timer(0.5).timeout
	Connection.create_subscriptions(_on_events,_on_entities)
	await get_tree().create_timer(0.5).timeout
	spawn()
	#await get_tree().create_timer(0.5).timeout
	#get_controllers()
	#await get_tree().create_timer(0.5).timeout
	#get_entities()


func get_controllers(addrs:Array = []) -> void:
	#addrs.append(Connection.controller_account.get_address())
	var data = Connection.client.get_controllers(addrs)
	for controller in data:
		controllers_manager.spawn_entity(controller)
	
	await get_tree().create_timer(0.2).timeout

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
				if entry.has("Vec2Signed"):
					id = entry["player"]
					var vec = entry["Vec2Signed"]
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
	var u32 = DojoHelpers.signed_to_u32_offset(int(steps))
	move_call.calldata[1] = u32
	push_warning(u32)
	
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
	var x:int = int(%Vx.text)
	var y:int = int(%Vy.text)
	#push_warning("first ", x, " ", y)
	#x = DojoHelpers.signed_to_u32_offset(x)
	#y = DojoHelpers.signed_to_u32_offset(y)
	#push_warning("second ", x, " ", y)
	#move_to.calldata[0][0] = x
	#move_to.calldata[0][1] = y
	move_to.calldata[0] = Vector2i(x,y)
	
	Connection.controller_account.execute_from_outside(move_to)


func _on_disconnect_pressed() -> void:
	Connection.controller_account.disconnect_controller()
	controllers_manager.clear_all_controllers()
	await get_tree().process_frame
	
	get_tree().reload_current_scene()

func _on_tokens_pressed() -> void:
	var client = Connection.client
	prints("[TORII]", client.get_tokens(), client.get_token_balances("0x2b1754e413c0bd1ef98ddcd99a8f9e996f3765553341d1075b153374cac51"), client.get_token_collections())

@onready var controller_btn: TextEdit = $UI/Arrows/VBoxContainer/PanelContainer/VBoxContainer/ControllerBtn

func _on_get_controller_pressed() -> void:
	#get_controllers([controller_btn.text])
	Connection.client.publish_message("AAAAAAAAAAAAAAAAAAAAAAAAAA", ["0x2b1754e413c0bd1ef98ddcd99a8f9e996f3765553341d1075b153374cac51"])


func _on_move_to_signed_pressed() -> void:
	var x:int = int(%Vx.text)
	var y:int = int(%Vy.text)
	move_to.calldata[0] = [x,y]
	
	Connection.controller_account.execute_from_outside(move_to_signed)
