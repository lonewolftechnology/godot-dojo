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
	set_process(false)

func _process(delta: float) -> void:
	if shutting_down:
		return

	if server.is_connection_available():
		var client:StreamPeerTCP = server.take_connection()
		if client:
			clients.push_back(client)

	var i = 0
	while i < clients.size():
		var client: StreamPeerTCP = clients[i]
		if not client.is_connected_to_host():
			clients.remove_at(i)
			continue
		
		if client.get_available_bytes() > 0:
			var client_data:Array = client.get_data(client.get_available_bytes())
			var data:PackedByteArray = client_data[1]
			var request_string = data.get_string_from_utf8()
			var payload:String = _get_padded_base64(filter_response(redirect_query_name + "=", request_string))
			
			if payload.is_empty():
				client.disconnect_from_host()
				i += 1
				continue
			
			var parsed_payload:String = Marshalls.base64_to_utf8(payload)
			var response:Dictionary = JSON.parse_string(parsed_payload)
			send_http_response(client, response)
			# The shutdown is initiated in send_http_response, stop processing.
			return
		
		i += 1
				
func _get_padded_base64(base64_string: String) -> String:
	var b64 := base64_string.replace("-", "+").replace("_", "/")
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
	
	return response.substr(start_pos, end_pos - start_pos)
	
func send_http_response(client: StreamPeerTCP, response:Dictionary) -> void:
	var format_args:Dictionary = {
		"username" : response["username"],
		"address" : response["address"],
		"owner_guid" : response["ownerGuid"],
		"transaction_hash" : response["transactionHash"],
		"expires_at" : Time.get_datetime_dict_from_unix_time(int(response["expiresAt"]))
	}

	var html_template:String = get_template()
	var html_content:String = html_template.format(format_args)
	
	var http_response:String = "HTTP/1.1 200 OK\r\n"
	http_response += "Content-Type: text/html; charset=UTF-8\r\n"
	http_response += "Content-Length: " + str(html_content.to_utf8_buffer().size()) + "\r\n"
	http_response += "Connection: close\r\n"
	http_response += "\r\n"
	http_response += html_content
	
	if client.is_connected_to_host():
		client.put_data(http_response.to_utf8_buffer())
	
	_shutdown()

func _shutdown() -> void:
	if shutting_down:
		return
	shutting_down = true
	
	set_process(false)

	if server and server.is_listening():
		server.stop()
	
	for client in clients:
		if client and client.is_connected_to_host():
			client.disconnect_from_host()
	clients.clear()

	get_tree().create_timer(5.0).timeout.connect(queue_free)

func start_server() -> bool:
	if server and server.is_listening():
		return true

	server = TCPServer.new()
	port = _generate_port()
	var err:Error = server.listen(port)
	if err == OK:
		print_rich("[color=green] Server listening on http://localhost:%d" %  port)
		set_process(true)
		return true
	
	push_error("Error initializing http server")
	set_process(false)
	return false

func get_template() -> String:
	var file := FileAccess.open(html_file, FileAccess.READ)
	if not file:
		var error = FileAccess.get_open_error()
		push_error("Failed to open template file '%s'. Error: %s" % [html_file, error_string(error)])
		return ""
	
	var content := file.get_as_text()
	return content

func _generate_port() -> int:
	return randi_range(1024, 49151)

func get_port() -> int:
	return port
