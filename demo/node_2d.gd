extends Node
@onready var dojo_session_account: DojoSessionAccount = $DojoSessionAccount

func _ready() -> void:
	var policies:Dictionary = {
		"max_fee": "0x100000",
		"policies": [{
			"contract_address": "0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da",
			"entrypoints": [
				"spawn",
				"move"
			]
		}]
	}
	
	var calldata:Array = [
		{
		"contract_address": "0x01d18853e41a1607c1bd80c2e80c34db3a59999a038b54a2424fae4ac71278da",
		"entrypoint": "spawn",
		}
	]
	
	var priv_key
	priv_key = DojoHelpers.generate_private_key()
#	priv_key = dojo_session_account.generate_private_key()
	push_warning(priv_key)
#	dojo_session_account.create(
#		"https://api.cartridge.gg/x/godot-demo-rookie/katana", 
#		priv_key, 
#		"0x075C10973fC5Fdfa2F7fF5875F10441E546D098f370f40b97A663F89BdAF81A0",
#		"0x57505f474f444f545f44454d4f5f524f4f4b4945", 
#		policies,
#		20
#	)
	
	dojo_session_account.create_from_subscribe(
		priv_key,
		policies,
		"https://api.cartridge.gg/x/godot-demo-rookie/katana",
		"https://api.cartridge.gg/x/godot-demo-rookie/torii/graphql"
	)
	
	dojo_session_account.execute(calldata)
	dojo_session_account.execute_from_outside(calldata)
