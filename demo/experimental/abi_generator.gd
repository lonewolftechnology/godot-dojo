@tool
class_name AbiGenerator
extends Node

@export_global_file("*.json") var manifest:String
@export_tool_button("Generate Resources") var _generate_resources = func(): _generate_abi()

var _script:GDScript

var symbols : Dictionary

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

	symbols["world"] = {
		"address":world_address,
		"name":world_name,
		"seed":world_seed
	}
	_set_class_name(world_name.to_pascal_case(), "RefCounted")
	_add_const("WORLD_ADRRESS", world_address)
	_add_const("WORLD_NAME",world_name)
	_add_const("WORLD_SEED",world_seed)

	symbols["contracts"] = _parse_contracts(contracts)
	symbols["models"] = _parse_models(models)
	symbols["events"] = _parse_models(events)
	symbols["abis"] = _parse_abis(abis,world_seed)
	
	_add_const("CONTRACTS", symbols["contracts"])
	_add_const("MODELS", symbols["models"])
	_add_const("EVENTS", symbols["events"])
	_add_const("ABIS", symbols["abis"])
	
	_create_enums()
	_create_funcs()
	
	_generate_classes()
	
	_save_script(world_name)

func _create_enums() -> void:
	var all_enums : Dictionary = {}
	for abi in symbols["abis"]:
		if abi["type"] == "enum":
			var names : Array = abi["name"]
			var enum_name : String = names.back()
			if not all_enums.has(enum_name):
				all_enums[enum_name] = []
			for variant in abi["variants"]:
				var value_type : String = variant["type"]
				var value_name : String = variant["name"]
				if value_type == "()":
					all_enums[enum_name].append(value_name.to_upper())
	
	for key in all_enums.keys():
		var enum_name : String = key
		var enum_values : Array = all_enums[enum_name]
		if enum_values[0] == "NONE":
			enum_values.pop_front()
			enum_values.append("NONE")
		_add_enum(enum_name,enum_values)
	

func _create_funcs() -> void:
	for abi in symbols["abis"]:
		if abi["type"] == "interface":
			var items : Array = abi["items"]
			var contract_name = abi["name"]
			for item in items:
				var _name : String = item["name"]
				var params : Array = []
				for input in item["inputs"]:
					params.append(input["name"])
				_add_execute(_name,params,contract_name)
				_add_execute_from_outside(_name,params,contract_name)


func _save_script(world_name:String) -> void:
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
		if dict.has("items"):
			data["items"] = dict["items"]
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

func _get_contract_address(tag:String) -> String:
	for contract in symbols["contracts"]:
		if contract["tag"] == tag:
			return contract["address"]
	return ""

func _add_execute(_name:String,params:Array,tag:Array):
	var contract_tag : String = "%s-%s"%[tag[0],tag[-2]]
	var contract_address : String = _get_contract_address(contract_tag)
	if contract_address.is_empty():
		return
	
	var line : String = """func {name}(account, {params}):
	var data := {}
	data["address"] = "{contract_address}"
	data["entrypoint"] = "{entrypoint}"
	data["calldata"] = [{calldata}]
	account.execute([data])""".format({
		"name":"execute_"+_name,
		"calldata": "" if params.is_empty() else ", ".join(params),
		"params": "" if params.is_empty() else ", ".join(params),
		"contract_address": contract_address,
		"entrypoint":_name,
	})
	_append_code(line)


func _add_execute_from_outside(_name:String,params:Array,tag:Array):
	var contract_tag : String = "%s-%s"%[tag[0],tag[-2]]
	var contract_address : String = _get_contract_address(contract_tag)
	if contract_address.is_empty():
		return
	
	var line : String = """func {name}(account, {params}):
	var data := {}
	data["address"] = "{contract_address}"
	data["entrypoint"] = "{entrypoint}"
	data["calldata"] = [{calldata}]
	account.execute_from_outside([data])""".format({
		"name":"execute_from_outside_"+_name,
		"calldata": "" if params.is_empty() else ", ".join(params),
		"params": "" if params.is_empty() else ", ".join(params),
		"contract_address": contract_address,
		"entrypoint":_name,
	})
	_append_code(line)

func _add_enum(_name:String, _values: Array):
	var line: String = "enum {name} {{values}}".format({
		"name": _name.capitalize(),
		"values": ", ".join(_values)
	})
	_append_code(line)

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

func _generate_classes() -> void:
	for model in symbols["models"]:
		var _name = model["model_name"]
		var _members = model["members"]
		_add_class(_name,_members)
		_add_line()

func _add_class(_name:String,members:Array) -> void:
	var line := "class %s:"%_name.to_pascal_case()
	_append_code(line,false)
	for member in members:
		var mname : String = member.name
		var mtype : String = _parse_core_type(member.type)
		line = "\tvar {name} {type}".format({
			"name":mname,
			"type": (": "+mtype) if not mtype.is_empty() else ""
		})
		_append_code(line,false)

func _parse_core_type(tags:Array) -> String:
	var returned_type : String = ""
	if tags[0] == "core":
		match tags[1]:
			"integer":
				if tags.back() in ["i128","u128","u256"]:
					returned_type = tags.back().to_upper()
				else:
					returned_type = "int"
			"bool":
				returned_type = "bool"
			"starknet":
				if tags.back() == "ContractAddress":
					returned_type = "String"
			"felt252":
				returned_type = "String"
			"byte_array":
				returned_type = "PackedByteArray"
			"option":
				returned_type = "Variant"
			"array":
				returned_type = "Array"
	return returned_type
