extends Node

@onready var torii_client: ToriiClient = $ToriiClient
@onready var controller_account: ControllerAccount = $ControllerAccount

func _on_torii_client_client_connected(success: bool) -> void:
	pass # Replace with function body.

func _on_torii_client_client_disconnected() -> void:
	pass # Replace with function body.


func _on_controller_account_controller_connected(success: bool) -> void:
	pass # Replace with function body.


func _on_controller_account_controller_disconnected() -> void:
	pass # Replace with function body.


func _on_controller_account_provider_status_updated(success: bool) -> void:
	pass # Replace with function body.


func _on_controller_account_transaction_executed(transaction_hash: String) -> void:
	pass # Replace with function body.


func _on_controller_account_transaction_failed(error_message: String) -> void:
	pass # Replace with function body.
