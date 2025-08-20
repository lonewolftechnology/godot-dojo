@tool
class_name AbiGenerator
extends Node

@export_global_file("*.json") var manifest:String
@export var torii_url:String
@export var katan_url:String
@export var chain_id:String

@export var torii_client:ToriiClient
@export var controller_account:ControllerAccount

@export_tool_button("Generate Resources") var _generate_resources = func(): _generate_abi()

func _generate_abi():
	var _file = FileAccess.open(manifest,FileAccess.READ)
	var manifest_data = JSON.parse_string(_file.get_as_text())
	var world = manifest_data["world"]
	# For now, we have only one
	var contract = manifest_data["contracts"][0]
	var contract_abi = contract["abi"]

	var models = manifest_data["models"]
	var events = manifest_data["events"]
	
	var seed = world["seed"]
	# Setup Torii
	torii_client.torii_url = torii_url
	torii_client.world_address = world["address"]
	
	var parsed_events:Array = []
	for event in events:
		parsed_events.append({
			"members": event["members"],
			"class_hash": event["class_hash"],
			"tag": event["tag"],
			"selector": event["selector"]
		})
		
	var parsed_models:Array = []
	for model in models:
		parsed_models.append({
			"members": model["members"],
			"class_hash": model["class_hash"],
			"tag": model["tag"],
			"selector": model["selector"]
		})
	
	torii_client.models = parsed_models
	torii_client.events = parsed_events
	
	var policies := DojoPolicies.new()
	var contract_namespace:String = contract["tag"].split("-")[0]
	var contract_name:String = contract["tag"].split("-")[1]
	
	policies.name = contract_name
	policies.contract = contract["address"]
	
	var result_policies: Array = []
	for system in contract["systems"]:
		if system == "upgrade": continue # Internal, ignore
		var policy:DojoPolicy = DojoPolicy.new()
		policy.method = system
		policy.description = system
		result_policies.append(policy)
		
	policies.policies = result_policies
	controller_account.policies = policies
	controller_account.chain_id = chain_id
	
	var _script:GDScript = GDScript.new()
	_script.source_code = "class_name %s\n" % contract_namespace.to_pascal_case()
	var script_path:String = "res://%s_%s_bindings.gd" % [contract_namespace, contract_name]
	var err = ResourceSaver.save(_script, script_path)
	
	if Engine.has_singleton("EditorInterface") and err == OK:
		Engine.get_singleton("EditorInterface").get_editor_toaster().push_toast("%s created" % script_path.get_file())
