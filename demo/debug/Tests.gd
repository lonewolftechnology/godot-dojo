extends Node
# CHAIN ID WP_GODOT_DEMO_ROOKIE
# TORII https://api.cartridge.gg/x/godot-demo-rookie/torii
# WORLD 0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5
# Actions 0x0393f8a2d0d47e384c3c61eedc08d2873f5d608f8da7ffb013e5d5aa327ac8f2
enum Directions{
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
}
@export var testing:DojoSubscription
@onready var chat_box: TextEdit = %ChatBox
@onready var tabs: TabContainer = %Tabs
@onready var reset_spawn: CheckBox = %ResetSpawn

@onready var controllers_manager: ControllersManager = $ControllersManager

@onready var button_toggle: Button = %ButtonToggle

@export_global_file("*.json") var dojo_abi: String

@onready var start_torii_client: Button = %StartToriiClient

# Status Nodes
@onready var username_status: Label = %UsernameStatus
@onready var client_status: DojoStatusIndicator = %ClientStatus
@onready var provider_status: DojoStatusIndicator = %ProviderStatus
@onready var controller_account_status: DojoStatusIndicator = %ControllerAccountStatus
@onready var account_status: DojoStatusIndicator = %AccountStatus
@onready var entities_status: DojoStatusIndicator = %EntitiesStatus
@onready var events_status: DojoStatusIndicator = %EventsStatus

# Buttons with states
@onready var disconnect_btn: Button = %Disconnect
@onready var connect_btn: Button = %ConnectController
@onready var entity_sub: Button = %EntitySub
@onready var event_sub: Button = %EventSub

# Tests Buttons
@onready var get_controllers: Button = %GetControllers
@onready var get_entities: Button = %GetEntities
@onready var get_world_metadata: Button = %GetWorldMetadata

var count: int = 0
var packed: String

@onready var client: ToriiClient = $ToriiClient
@onready var controller_account: ControllerAccount = $ControllerAccount

@export var spawn_call:DojoCall
@export var spawn_reset_call:DojoCall
@export var move_call:DojoCall

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")
	OS.set_environment("BROWSER", "chromium")
	if Engine.is_editor_hint(): return
	button_toggle.set_pressed(true)
	#if not dojo_abi.is_empty() and FileAccess.file_exists(dojo_abi):
		#var json_file = FileAccess.get_file_as_string(dojo_abi)
		#var json = JSON.parse_string(json_file)
		#var contracts = json['contracts']
		#for data in contracts:
			#print(data['systems'])
			#var abi = data['abi']
			#for abi_item in abi:
				#if abi_item['type'] == "interface" and abi_item['name'] == "dojo_starter::systems::actions::IActions":
					#var items = abi_item['items']
					#print(items)

func callable_events(args:Dictionary):
	#push_warning("Updates entities EVENT", args)
	var data = args["data"]
	# This is contract speficfic

	var result_data:Dictionary
	for entry:Dictionary in data:
		result_data.merge(entry)
	push_warning("EVENTS",result_data)
	if result_data.has("Vector2"):
		#await get_tree().process_frame
		await get_tree().process_frame
		controllers_manager.move_controller(result_data['player'], result_data['Vector2'])

func callable_entities(args:Dictionary):
	#push_warning("Updates entities", args)
	var data = args["data"]
	# This is contract speficfic and data structure may change

	var result_data:Dictionary
	for entry:Dictionary in data:
		result_data.merge(entry)
	push_warning("ENTITIES",result_data)
	if result_data.has("Vector2"):
		#await get_tree().process_frame
		await get_tree().process_frame
		controllers_manager.move_controller(result_data['player'], result_data['Vector2'])

func _on_subcribe_pressed() -> void:
	client.create_client()

func _on_spawn_pressed() -> void:
	if reset_spawn.button_pressed:
		controller_account.execute_from_outside(spawn_reset_call)
	else:
		controller_account.execute_from_outside(spawn_call)
	#dojo.spawn(reset_spawn.button_pressed,false)

func _on_spawn_raw_pressed() -> void:
	if reset_spawn.button_pressed:
		controller_account.execute_raw(spawn_reset_call)
	else:
		controller_account.execute_raw(spawn_call)

func update_event_subscription_status(_value:bool, _event:String, _status_node:HBoxContainer):
	update_status(_value, _status_node)

func update_status(_value:bool, _status_node):
	if _status_node is DojoStatusIndicator:
		_status_node.set_status(_value)

func _on_move_pressed() -> void:
	controller_account.execute_from_outside(move_call)
	
func _unhandled_input(event: InputEvent) -> void:
	if event.is_action("ui_accept"):
		get_viewport().set_input_as_handled()
		#if chat_box.is_visible_in_tree() and not chat_box.text.is_empty():
			#dojo.send_message(chat_box.text)


func _on_button_toggle_toggled(toggled_on: bool) -> void:
	tabs.visible = toggled_on

func _move(dir:Directions) -> void:
	move_call.calldata[0] = dir
	controller_account.execute_from_outside(move_call)
	count += 1
	
func _on_arrow_left_pressed() -> void:
	_move(Directions.LEFT)

func _on_arrow_up_pressed() -> void:
	_move(Directions.UP)

func _on_arrow_down_pressed() -> void:
	_move(Directions.DOWN)

func _on_arrow_right_pressed() -> void:
	_move(Directions.RIGHT)
	
	
func _on_disconnect_pressed() -> void:
	controller_account.disconnect_controller()

func _on_torii_client_client_connected(success: bool) -> void:
	client_status.set_status(success)
	connect_btn.disabled = false
	get_controllers.disabled = false
	get_entities.disabled = false
	get_world_metadata.disabled = false

func _on_torii_client_client_disconnected() -> void:
	client_status.set_status(false)
	connect_btn.disabled = true
	get_controllers.disabled = true
	get_entities.disabled = true
	get_world_metadata.disabled = true
	
func _on_controller_account_provider_status_updated(success: bool) -> void:
	provider_status.set_status(success)

func _on_controller_account_controller_connected(success: bool) -> void:
	controller_account_status.set_status(success)
	disconnect_btn.disabled = false

func _on_controller_account_controller_disconnected() -> void:
	controller_account_status.set_status(false)
	disconnect_btn.disabled = true

func _on_torii_client_entity_updated(entity_data: Dictionary) -> void:
	prints("Torii Entity update", entity_data)

func _on_torii_client_event_received(event_data: Dictionary) -> void:
	prints("Torii Event", event_data)

func _on_torii_client_subscription_error(error_message: String) -> void:
	push_error(error_message)

func _on_controller_account_current_user_info(user_info: Dictionary) -> void:
	var address:String = user_info['address']
	username_status.text = user_info['username']
	#controllers_manager.spawn_entity(user_info,true)
	var data = client.get_controller_info(address)


func _on_controller_account_transaction_executed(transaction_hash: String) -> void:
	pass # Replace with function body.


func _on_controller_account_transaction_failed(error_message: String) -> void:
	pass # Replace with function body.


func _on_connect_controller_pressed() -> void:
	controller_account.setup()


func _on_get_controllers_pressed() -> void:
	var data = client.get_controllers()
	for controller in data:
		controllers_manager.spawn_entity(controller)


func _on_get_entities_pressed() -> void:
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
	var data = client.get_entities(query)
	print(data)
	for entity in data:
		for model:Dictionary in entity.models:
			var id
			var position
			var can_move
			var remaining
			if model.has("vec"):
				for entry:Dictionary in model["vec"]:
					if entry.has("player"):
						print(entry)
						id = entry["player"]
					if entry.has("Vector2"):
						position = entry["Vector2"]
						
				controllers_manager.move_controller(id, position)
					
			if model.has("can_move"):
				for entry:Dictionary in model["can_move"]:
					if entry.has("can_move"):
						can_move = entry["can_move"]
					if entry.has("remaining"):
						remaining = entry["remaining"]

func _on_get_world_metadata_pressed() -> void:
	var data = client.get_world_metadata()
	print(data)


func _on_torii_client_subscription_created(subscription_name: String) -> void:
	if subscription_name == "entity":
		entities_status.set_status(true)
		entity_sub.disabled = true
	if subscription_name == "event":
		events_status.set_status(true)
		event_sub.disabled = true
		


func _on_testing_pressed() -> void:
	controller_account.execute_raw(move_call)


func _on_event_sub_pressed() -> void:
	# For now, COptionClause is not fullly implemented
	# So an empty dictionary is used for now 
	await client.create_event_subscription(callable_events, {})


func _on_entity_sub_pressed() -> void:
	# For now, COptionClause is not fullly implemented
	# So an empty dictionary is used for now 
	await client.create_entity_subscription(callable_entities, {})
