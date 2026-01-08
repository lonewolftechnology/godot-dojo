class_name HttpPinger
extends HTTPRequest

func _ready() -> void:
	request_completed.connect(_on_request_completed)
	
	var urls = [
			"https://api.cartridge.gg/x/godot-dojo-starter/katana",
			"https://api.cartridge.gg/x/godot-dojo-starter/torii"
		]
	
	var headers = ["Accept: application/json"]
	
	for url in urls:
		var error = request(url, headers)
	
		if error != OK:
			push_error("Url unreachable")
		
		print("Pinging %s" % url)
		await request_completed
	
func _on_request_completed(result: int, response_code: int, headers: PackedStringArray, body: PackedByteArray) -> void:
	if response_code == 200:
		var json_string: String = body.get_string_from_utf8()
		var json_data = JSON.parse_string(json_string)
		
		if json_data != null:
			print("Url reached")
			push_warning(json_data)
			
		else:
			push_error("Endpoint doesn't return a JSON as response")
	else:
		push_error("Petition failed with error:", response_code)
