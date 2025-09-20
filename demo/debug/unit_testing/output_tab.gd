@tool
class_name OutputTab
extends VBoxContainer

@onready var _title: Label = $Title
@onready var _scroll_container: ScrollContainer = $ScrollContainer
@onready var _output_text: RichTextLabel = $ScrollContainer/OutputText

@export var title:String = "":
	set(value):
		if _title:
			_title.text = value
		title = value
