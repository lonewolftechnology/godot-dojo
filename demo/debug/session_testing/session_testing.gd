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

var entity_sub: int
var message_sub

@onready var priv_key: String = "0xb6c159ded577cff3a1a734976f29ec654d44d70636ca781c62b0e11fa68d5c"

var full_policies:Dictionary = {
#	Constants.CONTRACT: {
#		"methods": [
#			{
#				"name" : "Spawn in world", # Optional
#				"description": "Can Spawn in the world", # Optional
#				"entrypoint": "spawn",
#			},
#			{
#				"entrypoint": "move",
#			}
#			]
#		},
	Constants.CONTRACT:{
		"methods": [
			{"entrypoint": "reset_spawn"},
			{"entrypoint": "spawn"},
			{"entrypoint": "move"},
			{"entrypoint": "move_signed"},
			{"entrypoint": "move_to"},
			{"entrypoint": "move_to_signed"},
			{"entrypoint": "validate"},
			{"entrypoint": "validate_i8"},
			{"entrypoint": "validate_i16"},
			{"entrypoint": "validate_i32"},
			{"entrypoint": "validate_i64"},
			{"entrypoint": "validate_i128"},
			{"entrypoint": "validate_u8"},
			{"entrypoint": "validate_u16"},
			{"entrypoint": "validate_u32"},
			{"entrypoint": "validate_u64"},
			{"entrypoint": "validate_u128"},
			{"entrypoint": "validate_u256"},
			{"entrypoint": "validate_bool"},
			{"entrypoint": "validate_felt252"},
			{"entrypoint": "validate_class_hash"},
			{"entrypoint": "validate_contract_address"},
			{"entrypoint": "validate_eth_address"},
			{"entrypoint": "validate_vec3"},
			{"entrypoint": "validate_fp_40"}
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

var is_standalone: bool:
	get:
		return get_parent() == get_tree().root

#FP40
var FRACTIONAL_BITS:
	get:
		return ProjectSettings.get_setting("dojo/config/fixed_point/default", 40)



#const events = [
#	"dojo_starter-ValidatedI8",
#	"dojo_starter-ValidatedI16",
#	"dojo_starter-ValidatedI32",
#	"dojo_starter-ValidatedI64",
#	"dojo_starter-ValidatedI128",
#	"dojo_starter-ValidatedVec3",
#	"dojo_starter-ValidatedU8",
#	"dojo_starter-ValidatedU16",
#	"dojo_starter-ValidatedU32",
#	"dojo_starter-ValidatedU64",
#	"dojo_starter-ValidatedU128",
#	"dojo_starter-ValidatedU256",
#	"dojo_starter-ValidatedBool",
#	"dojo_starter-ValidatedFP40",
#]

func callback(data:Dictionary, type:String):
	if type == "Entity":
		var result = data["models"]
		add_entry_to_output("%s CALLBACK" % type, result)
	if type == "Message":
		var _data = data["data"]
		for _key in _data:
			printt("**",_key)
#			if _key is DojoBigInt:
#				push_warning(_key.to_float())

func _ready() -> void:
	if is_standalone:
		torii_client = ToriiClient.new()
		add_child(torii_client)
		dojo_session_account = DojoSessionAccount.new()
		add_child(dojo_session_account)
		setup()
	
func setup():

	dojo_session_account.max_fee = "0x100000"
	dojo_session_account.full_policies = full_policies
	
	if OS.has_feature("android"):
		main_container.vertical = true
		menu_container.size_flags_vertical = Control.SIZE_EXPAND_FILL
		DisplayServer.screen_set_orientation(DisplayServer.ScreenOrientation.SCREEN_PORTRAIT)
		priv_key = ""
		
	if not torii_client.is_connected():
		var _success = torii_client.connect(Constants.TORII_URL)
		if _success:
			_register_subs()
		_update_parent_status_indicator("torii_connected",_success)
	
	if priv_key.is_empty():
		priv_key = ControllerHelper.generate_private_key()
		
	custom_priv_key.text = priv_key

func _update_parent_status_indicator(_signal:String, _value):
	var _current_scene:Node = get_tree().current_scene
	if not is_standalone and _current_scene is DojoTesting:
		_current_scene.emit_signal(_signal, _value)

func _register_subs():
	var _dojo_callback:DojoCallback = DojoCallback.new()
	_dojo_callback.on_update = callback.bind("Entity")
	entity_sub = torii_client.subscribe_entity_updates(DojoClause.new(), [Constants.WORLD], _dojo_callback)
	_update_parent_status_indicator("sub_created", "entity")
	
	var _dojo_callback_message:DojoCallback = DojoCallback.new()
	_dojo_callback_message.on_update = callback.bind("Message")
	message_sub = torii_client.subscribe_event_updates([], _dojo_callback_message)

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
	_update_parent_status_indicator("session_connected", dojo_session_account.is_valid())
	if (dojo_session_account.is_valid()):
		_on_info_btn_pressed()
		session_connected.emit()
		
func _on_login_btn_pressed() -> void:
	get_session_url()
	
func get_session_url():
	var base_url = "https://x.cartridge.gg/session"
	var public_key = ControllerHelper.get_public_key(priv_key)
	#var redirect_uri = "about:blank"
	#var redirect_query_name = "startapp"
	
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
	priv_key = ControllerHelper.generate_private_key()
	custom_priv_key.text = priv_key
	
func _on_iterative_calls_pressed() -> void:
	var int_test = [
#		"validate_fp_40",
		#"validate_i8",
		#"validate_i16",
		#"validate_i32",
#		"validate_i64",
		"validate_i128",
		#"validate_u8",
		#"validate_u16",
		#"validate_u32",
#		"validate_u64",
		"validate_u128",
		"validate_u256",

	]
	var felt_test = [
		#"validate_bool",
		#"validate_felt252,
		#"validate_class_hash,
		#"validate_contract_address,
		#"validate_eth_address,
		]
	for _entrypoint in int_test:
		var _call = {
				"contract_address": Constants.CONTRACT,
				"entrypoint": _entrypoint,
				"calldata": []
		}
		var _test_number = [
			2,
			-2,
			1.9,
			-1.9,
		]
		
		var result
		for n in _test_number:
			_call["calldata"] = [n]
			if _entrypoint == "validate_i128":
				var _i128:I128 = I128.from_int(n) if n is int else I128.from_float(n)           
				printt("creating from int", n, _entrypoint, _i128.to_float(), _i128.to_string())
				_call["calldata"] = [_i128]			
			if _entrypoint == "validate_u128":
				var _u128:U128 = U128.from_int(n) if n is int else U128.from_float(n) 
				printt("creating from int", n, _entrypoint, _u128.to_float(), _u128.to_string())
				_call["calldata"] = [_u128]
			if _entrypoint == "validate_u256":
				var _u256:U256 = U256.from_int(n) if n is int else U256.from_float(n) 
				printt("creating from int", n, _entrypoint, _u256.to_float(), _u256.to_string())
				_call["calldata"] = [_u256]				
						
			result = dojo_session_account.execute_from_outside([_call])
			add_entry_to_output("EXECUTE %s %s" % [_call["entrypoint"], n], result)


func _on_call_test_btn_pressed() -> void:
	var result = dojo_session_account.execute_test(Constants.CONTRACT, "validate_i64", 2)
	add_entry_to_output("EXECUTE", result)
