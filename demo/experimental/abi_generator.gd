@tool
class_name AbiGenerator
extends Node

@export_global_file("*.json") var manifest:String
@export_tool_button("Generate Resources") var _generate_resources = func(): _generate_abi()

var _script:GDScript

func _generate_abi():
	var _file = FileAccess.open(manifest,FileAccess.READ)
	var manifest_data = JSON.parse_string(_file.get_as_text())
	var world:Dictionary = manifest_data["world"]
	
	var world_name:String = world["name"]
	
	var contracts = manifest_data["contracts"]
	
	var models:Array = manifest_data["models"]
	var events:Array = manifest_data["events"]
	
	var contract_abis = manifest_data["abis"]
	
	var data:Dictionary = {
		"world": world["address"],
		"contracts": [],
		"models": models,
		"events": events,
		"abis": []
	}
	
	
	for contract:Dictionary in contracts:
		var tag = contract["tag"].split("-")
		var _contract:Dictionary = {
			"address" = contract["address"],
			"tag" = contract["tag"],
			"namespace" = tag[0],
			"actions" = tag[1],
			"systems" = contract["systems"]
		}
		data["contracts"].append(_contract)
	
	printt("JSON",data)

	_script = GDScript.new()
	_set_class_name(world_name.to_pascal_case(), "RefCounted")

	_add_const("WORLD_ADRRESS", world["address"])
	_add_const("MODELS", models)
	_add_const("CONTRACTS", data["contracts"])
	
	var script_path:String = "res://%s_bindings.gd" % [world_name.to_snake_case()]
	var err = ResourceSaver.save(_script, script_path)

	if Engine.has_singleton("EditorInterface") and err == OK:
		Engine.get_singleton("EditorInterface").get_editor_toaster().push_toast("%s created" % script_path.get_file())

# Helper Methods
func _add_line():
	_script.source_code += "\n"

func _set_class_name(_class:String, _extends:String = ""):
	_script.source_code = "class_name %s" % _class
	if not _extends.is_empty():
		_add_line()
		_script.source_code += "extends %s" % _extends
	_add_line()
	

func _append_code(_code:String, _new_line:bool = true):
	_add_line()
	_script.source_code += _code
	if _new_line: _add_line()

func _add_const(_name:String, _value, _new_line:bool = false):
	if _value is String:
		_value = "\"{value}\"".format({"value": _value})
		
	var _const = "const {name} = {value}".format({"name": _name, "value": _value})
	_append_code(_const,_new_line)
	if _new_line:
		_add_line()