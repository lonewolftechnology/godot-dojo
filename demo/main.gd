extends Node

const dev_world_addr = "0x07cb912d0029e3799c4b8f2253b21481b2ec814c5daf72de75164ca82e7c42a5"
const dev_actions_addr = "0x00a92391c5bcde7af4bad5fd0fff3834395b1ab8055a9abb8387c0e050a34edf"
#const katana_addr = "0x4b4154414e41"
@onready var dojo:DojoC = DojoC.new()

@onready var status: Label = $"TabContainer/WIP tests/Status"
@onready var input_world_addr: TextEdit = $"TabContainer/WIP tests/InputWorldAddr"
@onready var input_controller: TextEdit = $"TabContainer/WIP tests/InputController"

@onready var provider_status: HBoxContainer = $TabContainer/SpawnTest/HBoxContainer3/ProviderStatus
@onready var account_status: HBoxContainer = $TabContainer/SpawnTest/HBoxContainer3/AccountStatus
@onready var suscription_status: HBoxContainer = $TabContainer/SpawnTest/HBoxContainer3/SuscriptionStatus
@onready var spawn_output: RichTextLabel = $TabContainer/SpawnTest/ScrollContainer/SpawnOutput
@onready var controller_account_status: HBoxContainer = $TabContainer/SpawnTest/HBoxContainer3/ControllerAccountStatus
@onready var sprite_2d: Sprite2D = $Sprite2D

@export_global_file("*.json") var dojo_abi: String

func _ready() -> void:
	OS.set_environment("RUST_BACKTRACE", "full")
	OS.set_environment("RUST_LOG", "debug")
	OS.set_environment("BROWSER", "chromium")
	if Engine.is_editor_hint(): return
	dojo.event_update.connect(_on_event_update)
	dojo.account_status_updated.connect(update_status.bind(provider_status))
	dojo.provider_status_updated.connect(update_status.bind(account_status))
	dojo.subscription_status_updated.connect(update_event_subscription_status.bind(suscription_status))
	dojo.controller_account_status_updated.connect(update_status.bind(controller_account_status))
	if not dojo_abi.is_empty() and FileAccess.file_exists(dojo_abi):
		var json_file = FileAccess.get_file_as_string(dojo_abi)
		var json = JSON.parse_string(json_file)
		#json.parse(json_file)
		var contracts = json['contracts']
		for data in contracts:
			print(data['systems'])
			var abi = data['abi']
			for abi_item in abi:
				if abi_item['type'] == "interface" and abi_item['name'] == "dojo_starter::systems::actions::IActions":
					var items = abi_item['items']
					print(items)
					#for item in items:
						#if item['type'] == "function":
							#if item['state_mutability'] == "external":
								#print(item)
								#print(item['name'])
	
	#await get_tree().create_timer(1).timeout
	#print(testing.as_packed_array())
	

func callable_test(args:Array):
	prints("\nAAAAAAAAAAAAAA", args)
	if args.size() > 1:
		var vec = args[1]
		print(args[1])
		await get_tree().process_frame
		await get_tree().process_frame
		await get_tree().process_frame
		await get_tree().process_frame
		var vector = Vector2(vec['x'], vec['y'])
		sprite_2d.position = vector
		#spawn_output.call_deferred("append_text", vector)
		#spawn_output.call_deferred("newline")
	prints("AAAAAAAAAAAAAA\n")

func _on_button_pressed() -> void:
	if input_world_addr.text.is_empty():
		input_world_addr.text = dev_world_addr
	dojo.create_client(input_world_addr.text, "https://api.cartridge.gg/x/godot-demo-rookie/torii")
	
func _on_subcribe_pressed() -> void:
	_on_button_pressed()
	await get_tree().create_timer(1).timeout
	var callable:Callable = Callable(self, "callable_test")

	dojo.create_entity_subscription(callable)
	
func _on_connect_controller_pressed() -> void:
	if input_controller.text.is_empty():
		input_controller.text = dev_actions_addr
	dojo.controller_new(input_controller.text, "https://api.cartridge.gg/x/godot-demo-rookie/katana")

func _on_testing_pressed() -> void:
	dojo.testing()

func _on_spawn_pressed() -> void:
	dojo.spawn(false)

func _on_event_update(_msg:String):
	#_msg.any(func(c): spawn_output.text += c)
	spawn_output.append_text(_msg)
	spawn_output.newline()

func update_event_subscription_status(_value:bool, _event:String, _status_node:HBoxContainer):
	update_status(_value, _status_node)

func update_status(_value:bool, _status_node:HBoxContainer):
	if _value:
		_status_node.get_node("Status").color = Color.GREEN
	else:
		_status_node.get_node("Status").color = Color.RED


func _on_move_pressed() -> void:
	var random_direction = FieldElement.from_enum(randi_range(0,3))
	dojo.move(random_direction,false)
	
var count: int = 0
func _input(event: InputEvent) -> void:
	var direction:FieldElement
	if event.is_action_pressed("ui_up"):
		direction = FieldElement.from_enum(2)
	if event.is_action_pressed("ui_down"):
		direction = FieldElement.from_enum(3)
	if event.is_action_pressed("ui_left"):
		direction = FieldElement.from_enum(0)
	if event.is_action_pressed("ui_right"):
		direction = FieldElement.from_enum(1)
	if not direction == null:
		dojo.move(direction,false)
		count+=1
		print(count)
