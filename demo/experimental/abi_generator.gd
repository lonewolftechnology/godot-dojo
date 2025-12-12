@tool
class_name AbiGenerator
extends Node

@export_global_file("*.json") var manifest:String
@export_tool_button("Generate Resources") var _generate_resources = func(): _generate_abi()

var _script:GDScript

func _generate_abi():
	var _file = FileAccess.open(manifest,FileAccess.READ)
	var manifest_data = JSON.parse_string(_file.get_as_text())
	
	var world: Dictionary = manifest_data["world"]
	var contracts: Array = manifest_data["contracts"]
	var models: Array = manifest_data["models"]
	var events: Array = manifest_data["events"]
	var abis: Array = manifest_data["abis"]
	
	var world_name: String = world["name"]
	var world_address: String = world["address"]
	var world_seed: String = world["seed"]
	
	_script = GDScript.new()
	
	_set_class_name(world_name.to_pascal_case(), "RefCounted")
	_add_const("WORLD_ADRRESS", world_address)
	_add_const("WORLD_NAME",world_name)
	_add_const("WORLD_SEED",world_seed)
	
	_add_const("CONTRACTS", _parse_contracts(contracts))
	_add_const("MODELS", _parse_models(models))
	_add_const("EVENTS", _parse_models(events))
	_add_const("ABIS", _parse_abis(abis,world_seed))
	
	
	var script_path:String = "res://%s_bindings.gd" % [world_name.to_snake_case()]
	var err: int = ResourceSaver.save(_script, script_path)

	if Engine.has_singleton("EditorInterface") and err == OK:
		var editor_interface = Engine.get_singleton("EditorInterface")
		editor_interface.get_editor_toaster().push_toast("%s created" % script_path.get_file())
		editor_interface.get_resource_filesystem().update_file(script_path)
		editor_interface.get_file_system_dock().navigate_to_path(script_path)
		editor_interface.edit_script(load(script_path))
		
# Helper Methods
func _parse_contracts(list:Array) -> Array:
	var data := []
	for dict : Dictionary in list:
		var tag = dict["tag"].split("-")
		var systems : Array = dict["systems"]
		var parsed : Dictionary = {
			"address" = dict["address"],
			"tag" = dict["tag"],
			"namespace" = tag[0],
			"actions" = tag[1],
			"systems" = systems.filter(
				func(e): return e != "upgrade"
			)
		}
		data.append(parsed)
	return data

func _parse_models(list:Array) -> Array:
	var data := []
	for dict : Dictionary in list:
		var tag = dict["tag"].split("-")
		var members : Array = _parse_model_members(dict["members"])
		var parsed : Dictionary = {
			"selector" = dict["selector"],
			"tag" = dict["tag"],
			"namespace" = tag[0],
			"model_name" = tag[1],
			"members" = members
		}
		data.append(parsed)
	return data

func _parse_model_members(list:Array) -> Array:
	var data := []
	for dict : Dictionary in list:
		var parsed : Dictionary = {
			"name": dict["name"],
			"type": _parse_name(dict["type"]),
			"key": dict["key"]
		}
		data.append(parsed)
	return data


func _parse_abis(list:Array,seed:String,contract_specific_only:bool=true) -> Array:
	var data := []
	for dict : Dictionary in list:
		var parsed : Dictionary = _parse_abi(dict,seed)
		if parsed.contract_specific:
			data.append(parsed)
	return data

func _parse_abi(dict:Dictionary,seed:String) -> Dictionary:
	var data : Dictionary
	var type : String = dict["type"]
	var abi_name : Array = _parse_name(dict["name"])
	var has_seed_in_name : bool = abi_name.any( func(e:String): return e == seed)
	
	data = {
		"type":type,
		"name":abi_name,
		"contract_specific":has_seed_in_name
	}
	if has_seed_in_name:
		if dict.has("members"):
			data["members"] = dict["members"]
		if dict.has("variants"):
			data["variants"] = dict["variants"]
		if dict.has("kind"):
			data["kind"] = dict["kind"]
	
	return data
	

func _parse_name(type:String) -> Array:
	# TODO
	var list : Array = type.replace("::","$$").replace(",","$$").split("$$")
	list = list.map(
		func(e:String):
			return e.strip_edges().replace("<","").replace(">","").replace("(","").replace(")","")
	)
	return list
		

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
	var _const: String = "const {name} = {value}".format({"name": _name, "value": _indent_value(_value)})
	
	_append_code(_const,_new_line)
	if _new_line:
		_add_line()
		
func _indent_value(_value) -> String:
	var value:String = ""
	if _value is Dictionary or _value is Array:
		value = JSON.stringify(_value, "\t")
	elif _value is String:
		_value = "\"{value}\"".format({"value": _value})
		value = _value
	else:
		value = str(_value)
	return value
