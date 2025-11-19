class_name HttpTools
extends Node

var port:int = 27000
var server:TCPServer
var clients:Array[StreamPeerTCP]
var redirect_query_name:String = "startapp"

@export_file_path("*.html") var html_file:String

func _ready() -> void:
	start_server()
	
func _process(delta: float) -> void:
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
				
				var response:Dictionary = JSON.parse_string(parsed_payload)
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
	
func send_http_response(client: StreamPeerTCP, response:Dictionary):
	var username:String = response["username"]
	var address:String = response["address"]
	var owner_guid:String = response["ownerGuid"]
	var transaction_hash:String = response["transactionHash"]
	var expires_at:Dictionary = Time.get_datetime_dict_from_unix_time(int(response["expiresAt"]))
	

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

func setup(_port:int) -> void:
	port = _port
	start_server()
	
func start_server() -> bool:
	if server != null and server.is_listening():
		return true

	server = TCPServer.new()
	
	var err:int = server.listen(port)
	if not err == OK:
		push_error("Error initializing http server")
		set_process(false)
		return false
	
	set_process(true)
	print_rich("[color=green] Server listening on http://localhost:%d" %  port)
	return true

func get_template() -> String:
	var _file: FileAccess = FileAccess.open(html_file, FileAccess.READ)
	if not _file.get_error() == OK:
		push_error("failed opening html template")
	return _file.get_as_text()