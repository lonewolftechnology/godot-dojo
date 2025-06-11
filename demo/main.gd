extends Node

enum Directions{
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
}

const dev_world_addr = "0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5"
const dev_actions_addr = "0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf"

@export var generic_entity_scene:PackedScene
@export var player_entity_scene:PackedScene

@onready var dojo:DojoC = DojoC.new()

@onready var status: Label = %Status
@onready var input_world_addr: TextEdit = %InputWorldAddr
@onready var input_controller: TextEdit = %InputController

@onready var spawn_output: RichTextLabel = %SpawnOutput
@onready var chat_box: TextEdit = %ChatBox
@onready var tabs: TabContainer = %Tabs
@onready var reset_spawn: CheckBox = %ResetSpawn

@onready var controllers: Node2D = $Controllers

@onready var button_toggle: Button = %ButtonToggle

@export_global_file("*.json") var dojo_abi: String

# Status Nodes
@onready var username_status: Label = %UsernameStatus
@onready var client_status: DojoStatusIndicator = %ClientStatus
@onready var provider_status: DojoStatusIndicator = %ProviderStatus
@onready var controller_account_status: DojoStatusIndicator = %ControllerAccountStatus
@onready var account_status: DojoStatusIndicator = %AccountStatus
@onready var suscription_status: DojoStatusIndicator = %SuscriptionStatus
@onready var entities_status: DojoStatusIndicator = %EntitiesStatus

var count: int = 0
var packed: String

var player:PlayerEntity

var camera:Camera2D:
	get:
		if player:
			return player.camera
		else:
			return get_viewport().get_camera_2d()

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")
	OS.set_environment("BROWSER", "chromium")
	if Engine.is_editor_hint(): return
	button_toggle.set_pressed(true)
	dojo.event_update.connect(_on_event_update)
	dojo.account_status_updated.connect(account_status.set_status)
	dojo.provider_status_updated.connect(provider_status.set_status)
	dojo.subscription_status_updated.connect(suscription_status.set_status)
	dojo.controller_account_status_updated.connect(controller_account_status.set_status)
	dojo.entities_status_updated.connect(entities_status.set_status)
	dojo.client_status_updated.connect(client_status.set_status)
	if not dojo_abi.is_empty() and FileAccess.file_exists(dojo_abi):
		var json_file = FileAccess.get_file_as_string(dojo_abi)
		var json = JSON.parse_string(json_file)
		var contracts = json['contracts']
		for data in contracts:
			print(data['systems'])
			var abi = data['abi']
			for abi_item in abi:
				if abi_item['type'] == "interface" and abi_item['name'] == "dojo_starter::systems::actions::IActions":
					var items = abi_item['items']
					print(items)
	

func callable_test(args:Array):
	prints("Callable size:", args.size() ,"Result:",args)
	#prints("\n\n", packed)
	push_warning("Updates entities EVENT", args)
	var _packed = args.filter(func(c): return c is String)
	var _vector = args.filter(func(c): return c is Vector2 )
	
	if _vector.is_empty(): return
	
	var vec:Vector2 = _vector[0]
	if player:
		if _packed[0] != player.id: 
			push_warning("No es el player")
			var _id = _packed[0]
			await get_tree().process_frame
			controllers.move_controller(_id, vec)
		else:
			await get_tree().process_frame
			player.move(vec)

func call_test(args:Array):
	push_warning("Updates entities", args)
	var _packed = args.filter(func(c): return c is String)
	var _vector = args.filter(func(c): return c is Vector2)
	if _vector.is_empty(): return
	var vec:Vector2 = _vector[0]
	
	if player and _packed[0] != player.id: 
		push_warning("SON LO MISMo")
		var _id = _packed[0]
		await get_tree().process_frame
		controllers.move_controller(_id, vec)


func _on_button_pressed() -> void:
	if input_world_addr.text.is_empty():
		input_world_addr.text = dev_world_addr
	dojo.create_client(input_world_addr.text, "https://api.cartridge.gg/x/godot-demo-rookie/torii")
	
func _on_subcribe_pressed() -> void:
	_on_button_pressed()
	await get_tree().process_frame

	dojo.create_entity_subscription(call_test)
	await get_tree().process_frame
	
	dojo.entity_subscription(callable_test)
	
	await get_tree().process_frame
	#await get_tree().process_frame
	#dojo.client_metadata()

func _on_connect_controller_pressed() -> void:
	if input_controller.text.is_empty():
		input_controller.text = dev_actions_addr
	dojo.controller_new(input_controller.text, "https://api.cartridge.gg/x/godot-demo-rookie/katana")
	await dojo.on_account
	username_status.text = dojo.get_username()
	await get_tree().process_frame
	var users = dojo.get_controllers()
	users.any(
		func(user):
		spawn_entity(user, user['player'])
		)
	await get_tree().process_frame
	var entities = dojo.get_entities()
	entities.any(
		func(entity):
			controllers.move_controller(entity['id'], entity['pos'])
	)
	

func _on_spawn_pressed() -> void:
	dojo.spawn(reset_spawn.button_pressed,false)

func _on_event_update(_msg:String):
	spawn_output.append_text(_msg)
	spawn_output.newline()

func update_event_subscription_status(_value:bool, _event:String, _status_node:HBoxContainer):
	update_status(_value, _status_node)

func update_status(_value:bool, _status_node):
	if _status_node is DojoStatusIndicator:
		_status_node.set_status(_value)

func _on_move_pressed() -> void:
	var random_direction = FieldElement.from_enum(randi_range(0,3))
	dojo.move(random_direction,false, false)
	
func _unhandled_input(event: InputEvent) -> void:
	var direction:FieldElement
	if event.is_action_pressed("ui_up"):
		direction = FieldElement.from_enum(Directions.UP)
		get_viewport().set_input_as_handled()
	elif event.is_action_pressed("ui_down"):
		direction = FieldElement.from_enum(Directions.DOWN)
		get_viewport().set_input_as_handled()
	elif event.is_action_pressed("ui_left"):
		direction = FieldElement.from_enum(Directions.LEFT)
		get_viewport().set_input_as_handled()
	elif event.is_action_pressed("ui_right"):
		direction = FieldElement.from_enum(Directions.RIGHT)
		get_viewport().set_input_as_handled()
	
	if not direction == null:
		dojo.move(direction,false,false)
		count+=1
		print(count)
	
	if event.is_action("ui_accept"):
		get_viewport().set_input_as_handled()
		if chat_box.is_visible_in_tree() and not chat_box.text.is_empty():
			dojo.send_message(chat_box.text)
			
	if event.is_action_pressed("zoom_in"):
		camera.zoom += Vector2(0.1, 0.1)

	if event.is_action_pressed("zoom_out"):
		camera.zoom -= Vector2(0.1, 0.1)

	if event is InputEventMouseMotion && Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		camera.position -= event.relative * 1/camera.zoom
		

func _on_button_toggle_toggled(toggled_on: bool) -> void:
	tabs.visible = toggled_on

func _move(dir:Directions) -> void:
	var direction:FieldElement
	direction = FieldElement.from_enum(int(dir))
	dojo.move(direction,false,false)
	count += 1
	
func _on_arrow_left_pressed() -> void:
	_move(Directions.LEFT)

func _on_arrow_up_pressed() -> void:
	_move(Directions.UP)

func _on_arrow_down_pressed() -> void:
	_move(Directions.DOWN)

func _on_arrow_right_pressed() -> void:
	_move(Directions.RIGHT)


func spawn_entity(_data:Dictionary, is_player:bool = false):
	var new_entity:GenericEntity
	if is_player:
		new_entity = player_entity_scene.instantiate()
		player = new_entity
	else:
		new_entity = generic_entity_scene.instantiate()
	await get_tree().process_frame
	controllers.add_child(new_entity)
	new_entity.setup(_data)	
	
	
	


func _on_testing_pressed() -> void:
	dojo.move(FieldElement.from_enum(0),true,false)
