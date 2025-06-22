extends Node

enum Directions{
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
}

const dev_world_addr = "0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5"
const dev_actions_addr = "0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf"

@onready var chat_box: TextEdit = %ChatBox
@onready var tabs: TabContainer = %Tabs
@onready var reset_spawn: CheckBox = %ResetSpawn

@onready var controllers_manager: Node2D = $ControllersManager

@onready var button_toggle: Button = %ButtonToggle

@export_global_file("*.json") var dojo_abi: String

@onready var start_torii_client: Button = %StartToriiClient

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

@onready var client: ToriiClient = $ToriiClient
@onready var controller_account: ControllerAccount = $ControllerAccount

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
	
func _on_subcribe_pressed() -> void:
	client.client_connected.connect(client_status.set_status)
	client.create_client()
	controller_account.controller_connected.connect(controller_account_status.set_status)
	controller_account.controller_disconnected.connect(controller_account_status.set_status.bind(false))
	controller_account.provider_status_updated.connect(provider_status.set_status)

	#controller.create(dev_actions_addr, "https://api.cartridge.gg/x/godot-demo-rookie/katana")

	#var query = {
		#"pagination":{
			#"limit": 10,
			#"cursor": "",
			#"order_by": [],
			#"direction": ToriiClient.QueryPaginationDirection.FORWARD
		#},
		#"clause": null,
		#"no_hashed_keys": true,
		#"models":[],
		#"historical": false
	#}
	#var data = client.get_entities(query)
	#print(data)
	#dojo.create_entity_subscription(call_test)
	
	#dojo.entity_subscription(callable_test)
	
	#await get_tree().process_frame
	#dojo.client_metadata()


func _on_spawn_pressed() -> void:
	controller_account.execute_from_outside(controller_account.actions[0])
	#dojo.spawn(reset_spawn.button_pressed,false)

func _on_spawn_raw_pressed() -> void:
	controller_account.execute_raw(controller_account.actions[0])

func update_event_subscription_status(_value:bool, _event:String, _status_node:HBoxContainer):
	update_status(_value, _status_node)

func update_status(_value:bool, _status_node):
	if _status_node is DojoStatusIndicator:
		_status_node.set_status(_value)

func _on_move_pressed() -> void:
	#var random_direction = FieldElement.from_enum(randi_range(0,3))
	pass
	#dojo.move(random_direction,false, false)
	
func _unhandled_input(event: InputEvent) -> void:
	var direction:FieldElement
	if event.is_action_pressed("ui_up"):
		#direction = FieldElement.from_enum(Directions.UP)
		get_viewport().set_input_as_handled()
	elif event.is_action_pressed("ui_down"):
		#direction = FieldElement.from_enum(Directions.DOWN)
		get_viewport().set_input_as_handled()
	elif event.is_action_pressed("ui_left"):
		#direction = FieldElement.from_enum(Directions.LEFT)
		get_viewport().set_input_as_handled()
	elif event.is_action_pressed("ui_right"):
		#direction = FieldElement.from_enum(Directions.RIGHT)
		get_viewport().set_input_as_handled()
		
	if event.is_action("ui_accept"):
		get_viewport().set_input_as_handled()
		#if chat_box.is_visible_in_tree() and not chat_box.text.is_empty():
			#dojo.send_message(chat_box.text)


func _on_button_toggle_toggled(toggled_on: bool) -> void:
	tabs.visible = toggled_on

func _move(dir:Directions) -> void:
	count += 1
	
func _on_arrow_left_pressed() -> void:
	_move(Directions.LEFT)

func _on_arrow_up_pressed() -> void:
	_move(Directions.UP)

func _on_arrow_down_pressed() -> void:
	_move(Directions.DOWN)

func _on_arrow_right_pressed() -> void:
	_move(Directions.RIGHT)
	
	
func _on_testing_pressed() -> void:
	pass
	#dojo.move(FieldElement.from_enum(0),true,false)


func _on_client_metadata_pressed() -> void:
	var data = client.get_world_metadata()
	print(data)


func _on_setup_policies_pressed() -> void:
	controller_account.setup()
