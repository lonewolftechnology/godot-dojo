extends Node
@onready var dojo_session_account: DojoSessionAccount = $DojoSessionAccount
@onready var torii_client:ToriiClient = $ToriiClient

@export var entity_sub: EntitySubscription

var policies:Dictionary = {
		"max_fee": "0x100000",
		"policies": [{
			"contract_address": "0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da",
			"entrypoints": [
				"spawn",
				"move",
				"validate_i64",
			]
		}]
	}
	
var calldata:Array = [
		{
		"contract_address": "0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da",
		"entrypoint": "validate_i64",
		"calldata": [4]
		},
		{
		"contract_address": "0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da",
		"entrypoint": "validate_i64",
		"calldata": [-4]
		}
	]
@onready var priv_key = DojoHelpers.generate_private_key()

func callback(data):
	push_warning(data)

func _ready() -> void:
	torii_client.create_client()
	torii_client.on_entity_state_update(
		callback,
		entity_sub
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
		"https://api.cartridge.gg/x/godot-demo-rookie/katana",
		"https://api.cartridge.gg"
	)
	for _call in calldata:
		push_warning(_call)
		push_warning(dojo_session_account.execute([_call]))
	push_warning(dojo_session_account.execute_from_outside(calldata))

func _on_login_btn_pressed() -> void:
	get_session_url()
	
func get_session_url():
	var base_url = "https://x.cartridge.gg/session"
	var public_key = ControllerHelper.get_public_key(priv_key)
	var rpc_url = "https://api.cartridge.gg/x/godot-demo-rookie/katana"
	var redirect_uri = "about:blank"
	var redirect_query_name = "startapp"
	
	var policies = {
		"contracts": {
			"0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da": {
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
