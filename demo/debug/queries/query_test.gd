extends Control
@export_category("ToriiTest")
@onready var address_input:LineEdit = %AddressInput
@onready var model_input:LineEdit = %ModelInput
@onready var member_input:LineEdit = %MemberInput
@onready var output_box:RichTextLabel = %Output

@export var torii_client:ToriiClient

func _ready() -> void:
#	printt("query" ,result.get_type(), result.get_type_as_string(), type_string(result.get_type()))
	if get_parent() == get_tree().root:
		torii_client = ToriiClient.new()
		add_child(torii_client)
		torii_client.create_client(Constants.TORII_URL)

func _on_query_pressed() -> void:
	var query:DojoQuery = DojoQuery.new()
	var clause = KeysClause.new()
	clause.add_key(address_input.text)
	clause.add_model(model_input.text)
#	clause.tag = DojoOptionClause.ClauseTag.Member
#	clause.comparison_operator = DojoOptionClause.ComparisonOperator.Eq
#	clause.member_tag = DojoOptionClause.MemberValueTag.PrimitiveValue
#	clause.primitive_tag = DojoOptionClause.PrimitiveTag.ContractAddress
#	clause.model = model_input.text
#	clause.member = member_input.text
#	clause.value = address_input.text
#	query.models = [model_input.text]
	query.with_clause(clause)
	_send_query(query)


func _on_clear_pressed() -> void:
	output_box.text = ""


func _on_empty_query_pressed() -> void:
	var query:DojoQuery = DojoQuery.new()
	_send_query(query)


func _send_query(query:DojoQuery):
	var data:Array = torii_client.get_entities(query)
	if data.is_empty():
		output_box.append_text("No entity obtained\n")
		return
	if data[0].has("error"):
		var error = data[0]["error"]
		output_box.append_text("[color=red]%s[/color]\n" % error.split('"')[1])
		return
	print(data)
	data.any(_parse_query)

	output_box.append_text("------------------------\n")

func _parse_query(response:Dictionary):
	var models:Array = response["models"]
	models.any(_parse_model)
	output_box.append_text("\n")

func _parse_model(model:Dictionary):
	var _name = model.keys()[0]
	var model_data:Dictionary = model[_name]
	output_box.append_text("%s %s\n" % [_name, model_data["player"]])
	print_rich("[color=green]FOUND[/color] ", _name, " [color=cyan]ADDR[/color] ", model_data["player"])
