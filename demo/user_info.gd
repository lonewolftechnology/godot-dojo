class_name UserInfo
extends VBoxContainer

@onready var username_label:Label = $Username
@onready var address_label:Label = $Address
@onready var chain_id_label:Label = $ChainID
@onready var app_id_label:Label = $AppID
@onready var expires_at_label:Label = $ExpiresAt
@onready var is_expired_check:CheckButton = $IsExpired
@onready var is_revoked_check:CheckButton = $IsRevoked
@onready var owner_guid_label:Label = $OwnerGuid
@onready var owner_session_id_label:Label = $OwnerSessionId

func update_info(_data:Dictionary) -> void:
	if not visible:
		show()
		
	print_rich("[color=orange]%s[/color]" % _data)
	update_label(username_label, _data["username"])
	update_label(address_label, _data["address"])
	update_label(chain_id_label, _data["chain_id"])
	update_label(app_id_label, _data["app_id"])
	var expires_at: Dictionary = Time.get_datetime_dict_from_unix_time(_data["expires_at"])
	update_label(expires_at_label, Time.get_datetime_string_from_datetime_dict(expires_at, true))
	is_expired_check.set_pressed_no_signal(_data["is_expired"])
	is_revoked_check.set_pressed_no_signal(_data["is_revoked"])
	update_label(owner_guid_label, _data["owner_guid"])
	update_label(owner_session_id_label, _data["session_id"])

func update_label(_label:Label, _text:String) -> void:
	if _text.is_empty():
		_label.hide()
	_label.text = "%s: %s" % [_label.name, _text]
