class_name SessionTesting
extends PanelContainer

signal session_connected
signal subscription_created(subscription_name:String)

@export var dojo_session_account: DojoSessionAccount
@export var torii_client:ToriiClient

@onready var user_info:UserInfo = %UserInfo
@onready var custom_priv_key:LineEdit = %PrivateKeyCustom
@onready var output_box:RichTextLabel = %OutputBox
@onready var main_container:BoxContainer = %MainContainer
@onready var menu_container:VBoxContainer = %MenuContainer

@export var entity_sub: EntitySubscription
@export var message_sub: MessageSubscription

@onready var priv_key: String = "0x3f377826d5364b2b9740464524ed4d5758a6d45e8129cbd504704dd65468e32"

var full_policies:Dictionary = {
	Constants.CONTRACT: {
		"methods": [
			{
				"name" : "Spawn in world", # Optional
				"description": "Can Spawn in the world", # Optional
				"entrypoint": "spawn",
			},
			{
				"entrypoint": "move",
			}
		]
	}
}

var spawn_call:Dictionary = { 
		"contract_address": Constants.CONTRACT,
		"entrypoint": "spawn",
		"calldata": []
		}

var move_call:Dictionary = { 
		"contract_address": Constants.CONTRACT,
		"entrypoint": "move",
		"calldata": [0]
		}
	
var calldata:Array = [
		spawn_call,		
		move_call

	]

func callback(data:Dictionary, type:String):
	var result = data["models"]
	add_entry_to_output("%s CALLBACK" % type, result)

func _ready() -> void:
	if get_parent() == get_tree().root:
		torii_client = ToriiClient.new()
		add_child(torii_client)
		dojo_session_account = DojoSessionAccount.new()
		add_child(dojo_session_account)
	setup()
	
func setup():

	dojo_session_account.max_fee = "0x100000"
	dojo_session_account.full_policies = full_policies
	if not torii_client.client_connected.is_connected(_on_client_connected):
		torii_client.client_connected.connect(_on_client_connected)
			
	entity_sub.world_addresses.append(Constants.WORLD)
	message_sub.world_addresses.append(Constants.WORLD)
	
	if OS.has_feature("android"):
		main_container.vertical = true
		menu_container.size_flags_vertical = Control.SIZE_EXPAND_FILL
		DisplayServer.screen_set_orientation(DisplayServer.ScreenOrientation.SCREEN_PORTRAIT)
		priv_key = ""
		
	if not torii_client.is_client_connected():
		torii_client.create_client(Constants.TORII_URL)
	
	if priv_key.is_empty():
		priv_key = DojoHelpers.generate_private_key()
		
	custom_priv_key.text = priv_key

func _on_client_connected(success:bool):
	if success:
		_register_subs()

func _register_subs():
	torii_client.on_entity_state_update(
		callback.bind("Entity"),
		entity_sub
	)
	subscription_created.emit("entity_state_update")
	torii_client.on_event_message_update(
		callback.bind("Event"),
		message_sub
	)
	subscription_created.emit("event_message_update")

func _on_session_btn_pressed() -> void:
	if custom_priv_key.text != priv_key:
		priv_key = custom_priv_key.text
	#dojo_session_account.create(
	#		"https://api.cartridge.gg/x/godot-demo-rookie/katana", 
	#		priv_key, 
	#		"0x075C10973fC5Fdfa2F7fF5875F10441E546D098f370f40b97A663F89BdAF81A0",
	#		"0x57505f474f444f545f44454d4f5f524f4f4b4945", 
	#		policies,
	#		20
	#)
	dojo_session_account.create_from_subscribe(
		priv_key,
		Constants.RPC_URL,
		"https://api.cartridge.gg",
	)
	if (dojo_session_account.is_valid()):
		_on_info_btn_pressed()
		session_connected.emit()
		
func _on_login_btn_pressed() -> void:
	get_session_url()
	
func get_session_url():
	var base_url = "https://x.cartridge.gg/session"
	var public_key = ControllerHelper.get_public_key(priv_key)
	var redirect_uri = "about:blank"
	var redirect_query_name = "startapp"
	
	var session_url = dojo_session_account.generate_session_request_url(
		base_url, 
		public_key, 
		Constants.RPC_URL,
		dojo_session_account.get_register_session_policy(),
#		redirect_uri, # Optional parameter
#		redirect_query_name # Optional parameter
		)
	OS.shell_open(session_url)



func _on_info_btn_pressed() -> void:
	user_info.update_info(dojo_session_account.get_info())

func _on_call_btn_pressed() -> void:
	var result = dojo_session_account.execute([spawn_call])
	add_entry_to_output("EXECUTE SPAWN", result)


func _on_call_outside_btn_pressed() -> void:
	var result = dojo_session_account.execute_from_outside([spawn_call])
	add_entry_to_output("EXECUTE OUTSIDE SPAWN", result)

func add_entry_to_output(_identifier:String, _entry) -> void:
	var final_text = "[color=green]%s[/color] %s\n" % [_identifier,_entry]
	output_box.call_deferred("append_text",final_text)
#	print_rich(final_text)


func _on_refresh_key_pressed() -> void:
	priv_key = DojoHelpers.generate_private_key()
	custom_priv_key.text = priv_key
	
func _on_iterative_calls_pressed() -> void:
	for _call in calldata:
		var result = dojo_session_account.execute_from_outside([_call])
		if result.begins_with("0x"):
			push_warning("CURRENT CALL %s %s %s" % [_call["entrypoint"], _call["calldata"], result])
		else:
			push_error("CURRENT CALL %s %s %s" % [_call["entrypoint"], _call["calldata"], result])
		add_entry_to_output("EXECUTE OUTSIDE %s" % _call["entrypoint"], result)
