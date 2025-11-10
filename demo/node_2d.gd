extends Node
@onready var dojo_session_account: DojoSessionAccount = $DojoSessionAccount
@onready var torii_client:ToriiClient = $ToriiClient

@onready var user_info:UserInfo = %UserInfo
@onready var custom_priv_key:LineEdit = %PrivateKeyCustom
@onready var output_box:RichTextLabel = %OutputBox
@onready var main_container:BoxContainer = %MainContainer
@onready var menu_container:VBoxContainer = %MenuContainer
@onready var dojo_controller:DojoController = $DojoController

@export var entity_sub: EntitySubscription
@export var message_sub: MessageSubscription

const CONTRACT = "0x023b0d96f2617d1be29e5ee6ec4b7b4da312d0eb28d6c83f5ef1c2ba254f3a6f"

const WORLD = "0x026d5777eccca1861a23303ee0ba48c0e8349e849d0377a21c3801ef1d0f8cef"
#const RPC_URL = "http://localhost:5050"
const RPC_URL = "https://api.cartridge.gg/x/godot-demo-rookie/katana"
#const TORII_URL = "http://localhost:8080"
const TORII_URL = "https://api.cartridge.gg/x/godot-demo-rookie/torii"


var policies:Dictionary = {
		"max_fee": "0x100000",
		"policies": [{
			"contract_address": CONTRACT,
			"entrypoints": [
				"spawn",
				"move",
				"validate_i64",
				"validate_i128",
				"validate_u128",
				"validate_u256"
			]
		}]
	}

var spawn_call:Dictionary = { 
		"contract_address": CONTRACT,
		"entrypoint": "spawn",
		"calldata": []
		}
	
var calldata:Array = [
		{
		"contract_address": CONTRACT,
		"entrypoint": "spawn",
		},
		{
		"contract_address": CONTRACT,
		"entrypoint": "validate_i64",
		"calldata": [4]
		},
		{
		"contract_address": CONTRACT,
		"entrypoint": "validate_i64",
		"calldata": [-4]
		},
				{
		"contract_address": CONTRACT,
		"entrypoint": "validate_i128",
		"calldata": [I128.from_variant(4)]
		},
		{
		"contract_address": CONTRACT,
		"entrypoint": "validate_i128",
		"calldata": [I128.from_variant(-4)]
		},
		{
		"contract_address": CONTRACT,
		"entrypoint": "validate_u128",
		"calldata": [U128.from_variant(4)]
		},
		{
		"contract_address": CONTRACT,
		"entrypoint": "validate_u128",
		"calldata": [U128.from_variant(-4)]
		},
		{
		"contract_address": CONTRACT,
		"entrypoint": "validate_u256",
		"calldata": [U256.from_variant(4)]
		},
		{
		"contract_address": CONTRACT,
		"entrypoint": "validate_u256",
		"calldata": [U256.from_variant(-4)]
		}
	]
@onready var priv_key:String = "0x3f9f6b16ddc141157d2712176d389e43acb016f7de1744ff544e91287e8e"

func callback(data):
	print_rich("[color=RED]SUB Callback[/color] %s" % [data])

func _ready() -> void:
	entity_sub.world_addresses.append(WORLD)
	message_sub.world_addresses.append(WORLD)
	if OS.has_feature("android"):
		main_container.vertical = true
		menu_container.size_flags_vertical = Control.SIZE_EXPAND_FILL
		DisplayServer.screen_set_orientation(DisplayServer.ScreenOrientation.SCREEN_PORTRAIT)
		priv_key = ""
		
	torii_client.create_client(TORII_URL)
	
	torii_client.on_entity_state_update(
		callback,
		entity_sub
	)
	torii_client.on_event_message_update(
		callback,
		message_sub
	)
	if priv_key.is_empty():
		priv_key = DojoHelpers.generate_private_key()
		
	custom_priv_key.text = priv_key


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
		policies,
		RPC_URL,
		"https://api.cartridge.gg"
	)
	if (dojo_session_account.is_valid()):
		_on_info_btn_pressed()
		
func _on_login_btn_pressed() -> void:
	get_session_url()
	
func get_session_url():
	var base_url = "https://x.cartridge.gg/session"
	var public_key = ControllerHelper.get_public_key(priv_key)
	var rpc_url = RPC_URL
	var redirect_uri = "about:blank"
	var redirect_query_name = "startapp"
	
	var _policies = {
		"contracts": {
			CONTRACT: {
				"methods": [
					{
						"name" : "Spawn in world", # Optional
						"description": "Can Spawn in the world", # Optional
						"entrypoint": "spawn",
					},
					{
						"entrypoint": "move",
					},
					{
						"entrypoint": "validate_i64"
					},
					{
						"entrypoint": "validate_i128"
					},
					{
						"entrypoint": "validate_u128"
					},
					{
						"entrypoint": "validate_u256"
					}
				]
			}
		},
		"verified": false
	}

	var session_url = DojoSessionAccount.generate_session_request_url(
		base_url, 
		public_key, 
		_policies, 
		rpc_url, 
#		redirect_uri, # Optional
#		redirect_query_name # Optional
		)
	OS.shell_open(session_url)



func _on_info_btn_pressed() -> void:
	user_info.update_info(dojo_session_account.get_info())

func _on_call_btn_pressed() -> void:
	prints("CURRENT CALL",spawn_call)
	var result = dojo_session_account.execute([spawn_call])
	add_entry_to_output("EXECUTE SPAWN", result)


func _on_call_outside_btn_pressed() -> void:
	prints("CURRENT CALL",spawn_call)
	var result = dojo_session_account.execute_from_outside([spawn_call])
	add_entry_to_output("EXECUTE OUTSIDE SPAWN", result)

func add_entry_to_output(_identifier:String, _entry:String) -> void:
	var final_text = "[color=green]%s[/color] %s\n" % [_identifier,_entry]
	output_box.call_deferred("append_text",final_text)
#	print_rich(final_text)


func _on_refresh_key_pressed() -> void:
	priv_key = DojoHelpers.generate_private_key()
	custom_priv_key.text = priv_key
	


func _on_dojo_controller_btn_pressed() -> void:
	var dojo_owner:DojoOwner = DojoOwner.init(priv_key)
	dojo_controller = DojoController.new_headless(
	"https://x.cartridge.gg/", 
	"hazel", 
	ControllerHelper.get_controller_class_hash(7), 
	RPC_URL, 
	dojo_owner, 
	"DTO_LOCAL"
	)
#	push_warning(dojo_controller.username())


func _on_torii_client_entities_received(entities: Array[Dictionary]) -> void:
	push_warning("_on_torii_client_entities_recei,entitiesved")


func _on_torii_client_entity_updated(entity_data: Dictionary) -> void:
	push_warning("_on_torii_client_entity_updated,entity_data")


func _on_torii_client_event_received(event_data: Dictionary) -> void:
	push_warning("_on_torii_client_event_receive,event_datad")


func _on_torii_client_message_published(message_hash: String) -> void:
	push_warning("_on_torii_client_message_published",message_hash)
