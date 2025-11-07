extends Node
@onready var dojo_session_account: DojoSessionAccount = $DojoSessionAccount
@onready var torii_client:ToriiClient = $ToriiClient

@export var entity_sub: EntitySubscription
@export var message_sub: MessageSubscription

const CONTRACT = "0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da"
const RPC_URL = "https://api.cartridge.gg/x/godot-demo-rookie/katana"
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
@onready var priv_key = DojoHelpers.generate_private_key()

func callback(data):
	print_rich("[color=RED]SUB Callback[/color] %s" % [data])

func _ready() -> void:
	torii_client.create_client(TORII_URL)
	torii_client.on_entity_state_update(
		callback,
		entity_sub
	)
	torii_client.on_event_message_update(
		callback,
		message_sub
	)

func _on_session_btn_pressed() -> void:
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
	
			
func _on_login_btn_pressed() -> void:
	get_session_url()
	
func get_session_url():
	var base_url = "https://x.cartridge.gg/session"
	var public_key = ControllerHelper.get_public_key(priv_key)
	var rpc_url = RPC_URL
	var redirect_uri = "about:blank"
	var redirect_query_name = "startapp"
	
	var policies = {
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
		policies, 
		rpc_url, 
#		redirect_uri, # Optional
#		redirect_query_name # Optional
		)
	OS.shell_open(session_url)



func _on_info_btn_pressed() -> void:
	push_warning(dojo_session_account.get_info())

func _on_call_btn_pressed() -> void:
	for _call in calldata:
		prints("CURRENT CALL",_call)
		print_rich("[color=green]EXECUTE[/color] %s" % [dojo_session_account.execute([_call])])
		#print_rich("[color=green]EXECUTE OUTSIDE[/color] %s" % [dojo_session_account.execute_from_outside([_call])])
