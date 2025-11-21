class_name HttpTools
extends Node

var port:int
var server:TCPServer
var clients:Array[StreamPeerTCP]
var redirect_query_name:String = "startapp"
var shutting_down: bool = false

@export_file_path("*.html") var html_file:String

func _ready() -> void:
	randomize()
	
func _process(delta: float) -> void:
	if server == null or not server.is_listening():
		return

	if server.is_connection_available():
		var client:StreamPeerTCP = server.take_connection()
		
		if client:
			clients.push_back(client)
	
	for client:StreamPeerTCP in clients:
		if client.get_status() == StreamPeerTCP.STATUS_CONNECTED:
			var available_bytes = client.get_available_bytes()
			if available_bytes > 0:
				var client_data:Array = client.get_data(available_bytes)
				var data:PackedByteArray = client_data[1]
				var request_string = data.get_string_from_utf8()
				var payload:String = _get_padded_base64(filter_response(redirect_query_name + "=", request_string))
				
				if payload.is_empty():
					client.disconnect_from_host()
					continue
				
				var parsed_payload:String = Marshalls.base64_to_utf8(payload)
				
				var response:Variant = JSON.parse_string(parsed_payload)
				if response == null:
					push_error("HTTP Tools: Failed to parse JSON payload or payload was empty.")
					client.disconnect_from_host()
					continue

				send_http_response(client, response)
				client.disconnect_from_host()
				
				DisplayServer.window_request_attention()
		else:
			clients.erase(client)
				
			

func _get_padded_base64(base64_string: String) -> String:
	# Replace URL-safe characters with standard Base64 characters
	var b64 := base64_string.replace("-", "+").replace("_", "/")
	# Add padding
	while b64.length() % 4 != 0:
		b64 += "="
	return b64
	
func filter_response(marker:String, response:String) -> String:
	var end_marker = " HTTP/1.1"
	var start_pos:int = response.find(marker)
	if start_pos == -1: return ""
	start_pos += marker.length()
	
	var end_pos:int = response.find(end_marker, start_pos)
	if end_pos == -1: return ""
	
	var result:String = response.substr(start_pos, end_pos - start_pos)
	
	return result
	
func send_http_response(client: StreamPeerTCP, response:Dictionary) -> void:
	var username:String = response["username"]
	var address:String = response["address"]
	var owner_guid:String = response["ownerGuid"]
	var transaction_hash:String = response["transactionHash"]
	var time_dict:Dictionary = Time.get_datetime_dict_from_unix_time(int(response["expiresAt"]))
	var expires_at:String = Time.get_datetime_string_from_datetime_dict(time_dict, true)
	

	var format_args:Dictionary = {
		"username" : username,
		"address" : address,
		"owner_guid" : owner_guid,
		"transaction_hash" : transaction_hash,
		"expires_at" : expires_at
	}

	var html_template:String = get_template()
	var html_content:String = html_template.format(format_args)
	
	var http_response:String = "HTTP/1.1 200 OK\r\n";
	http_response += "Content-Type: text/html; charset=UTF-8\r\n";
	http_response += "Content-Length: " + str(html_content.to_utf8_buffer().size()) + "\r\n";
	http_response += "Connection: close\r\n";
	http_response += "\r\n";
	http_response += html_content;
	
	client.put_data(http_response.to_utf8_buffer())
	
	if not shutting_down:
		shutting_down = true
		get_tree().create_timer(5.0).timeout.connect(_stop_and_free)


func _stop_and_free() -> void:
	if server != null and server.is_listening():
		server.stop()
	
	if is_instance_valid(self):
		queue_free()


func start_server() -> bool:
	if server != null and server.is_listening():
		return true

	server = TCPServer.new()
	
	var err:int
	port = _generate_port()
	err = server.listen(port)
	if err == OK:
		print_rich("[color=green] Server listening on http://localhost:%d" %  port)
		set_process(true)
		return true
	
	push_error("Error initializing http server")
	set_process(false)
	return false

func get_template() -> String:
	var file := FileAccess.open(html_file, FileAccess.READ)
	if file == null:
		var error_code = FileAccess.get_open_error()
		push_error("Failed to open template file '%s'. Error code: %s" % [html_file, error_code])
		return ""
	
	var content := file.get_as_text()
	return content

func _generate_port() -> int:
	return randi_range(1024, 49151)

func get_port() -> int:
	return port
