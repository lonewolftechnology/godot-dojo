extends Node2D

const PLUGIN_NAME = "GodotDojoAndroidPlugin"
var _android_plugin

@onready var result_label = $Label

func _ready():
	if OS.get_name() != "Android":
		result_label.text = "Plugin only works on Android."
		return

	if not Engine.has_singleton(PLUGIN_NAME):
		printerr("Couldn't find plugin: " + PLUGIN_NAME)
		result_label.text = "Plugin not found: " + PLUGIN_NAME
		return
		
	_android_plugin = Engine.get_singleton(PLUGIN_NAME)
	

	get_tree().get_root().connect("new_intent", _android_plugin.onNewGodotIntent)

	_android_plugin.connect("on_deep_link_received", _on_deep_link_received)

	var launch_intent = Engine.get_main_loop().get_startup_intent()
	if launch_intent and launch_intent.has("intent"):
		_android_plugin.onNewGodotIntent(launch_intent)


func _on_deep_link_received(uri_string: String):
	print("Deep Link received in Godot: ", uri_string)
	result_label.text = "Deep Link Received:\n" + uri_string

func _on_Button_pressed():
	if _android_plugin:
		# TODO: Update to match your plugin's API
		_android_plugin.helloWorld()

func _on_TestDeepLinkButton_pressed():
	if _android_plugin:
		print("Simulating deep link from GDScript...")
		_android_plugin.simulateDeepLink("example://godot/from_button")
