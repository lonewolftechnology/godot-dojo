extends VBoxContainer
signal entered

const PLUGIN_NAME = "GodotDojoAndroidPlugin"
var _android_plugin

@onready var deep_link: Label = $DeepLink

func _ready():
	if OS.get_name() != "Android":
		deep_link.text = "Plugin only works on Android."
		queue_free()
		return

	if not Engine.has_singleton(PLUGIN_NAME):
		printerr("Couldn't find plugin: " + PLUGIN_NAME)
		deep_link.text = "Plugin not found: " + PLUGIN_NAME
		return
	
	_android_plugin = Engine.get_singleton(PLUGIN_NAME)

	_android_plugin.connect("on_auth_payload_received", _on_deep_link_received)


func _on_deep_link_received(uri_string: String):
	print_rich("[color=red]Deep Link received in Godot[/color]: ", uri_string)
	deep_link.text = "Deep Link Received:\n" + uri_string


func _on_toast_pressed() -> void:
	if _android_plugin:
		# TODO: Update to match your plugin's API
		#_android_plugin.helloWorld()
		var con:DojoConnection = get_tree().root.get_node("DemoGame/Connection")
		var result = con.controller_account.controller_generate_url_and_public_key("example://godot")
		_android_plugin.openAuthWebView(result["url"])

func _on_deep_link_btn_pressed() -> void:
	if _android_plugin:
		print("Simulating deep link from GDScript...")
		#_android_plugin.simulateDeepLink("example://godot/from_button")
		push_warning(_android_plugin.getData())
		
