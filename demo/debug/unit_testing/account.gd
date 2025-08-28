extends Node

signal validation_result(result:bool, selector:String, calldata:Variant)
signal next_test

const local_rpc_url = "http://localhost:5050"
const local_torii_url = "http://localhost:8080"

const slot_rpc_url = "https://api.cartridge.gg/x/godot-demo-rookie/katana"
const slot_torii_url = "https://api.cartridge.gg/x/godot-demo-rookie/torii"

const contract_namespace = "dojo_starter"
const contract_address = "0x0406949a238323d32875589b1881dbc549017a75f72eaa731fc1237a9e69c10b"
const local_contract_address = "0x06b71e9a3aa5198aa78f7d8c407024d863e1d1263e735081c824deea349d3852" #Plain Starter
const world_address = "0x073c2b317136214562b608523812f369a05efe67257842a17c4985ce6d390be7"

# First account of contract
const account_address = "0x6677fe62ee39c7b07401f754138502bab7fac99d2d3c5d37df7d1c6fab10819"
const private_key = "0x3e3979c1ed728490308054fe357a9f49cf67f80f9721f44cc57235129e090f4"
const public_key = "0x1e8965b7d0b20b91a62fe515dd991dc9fcb748acddf6b2cf18cec3bdd0f9f9a"

# The key is the selector and the array contains the data to validate to
#const to_validate = [20,1.5,2.6,9.5,1.2,1.6,-1.5]
#const to_validate = [-5,-1,-2,-4,-3,-8,-6,-200,-100,-60]
const to_validate = [255,256,260]
const tests = {
    #"validate_vec3": [Vector3(7.71271926302993,3,7)],
    "validate_vec3": [[7.71271926302993, 3, 7]],
    #"validate_i8": to_validate,
    #"validate_i16": to_validate,
    "validate_i32": to_validate,
    "validate_i64": to_validate,
    "validate_i128": to_validate,
    #"validate_u8": to_validate,
    #"validate_u16": to_validate,
    #"validate_u32": to_validate,
    #"validate_u64": to_validate,
    "validate_u128": to_validate,
    "validate_u256": to_validate,
    #"validate_bool": [true, false],
    #"validate_felt252": [],
    #"validate_class_hash": [],
    #"validate_contract_address": [],
    #"validate_eth_address": [],
}

const events = [
    "dojo_starter-ValidatedI8",
    "dojo_starter-ValidatedI16",
    "dojo_starter-ValidatedI32",
    "dojo_starter-ValidatedI64",
    "dojo_starter-ValidatedI128",
    "dojo_starter-ValidatedVec3",
    "dojo_starter-ValidatedU8",
    "dojo_starter-ValidatedU16",
    "dojo_starter-ValidatedU32",
    "dojo_starter-ValidatedU64",
    "dojo_starter-ValidatedU128",
    "dojo_starter-ValidatedU256",
    "dojo_starter-ValidatedBool",
]

var current_contract:String:
    get:
        if use_custom_starter:
            return contract_address
        else:
            return local_contract_address

var rpc_url:String:
    get:
        if use_slot:
            return slot_rpc_url
        else:
            return local_rpc_url

var torii_url:String:
    get:
        if use_slot:
            return slot_torii_url
        else:
            return local_torii_url

@onready var account: Account = $Account
@onready var torii_client: ToriiClient = $ToriiClient

@onready var output_text: RichTextLabel = %OutputText
@onready var sub_output: RichTextLabel = %SubOutput

@onready var output_container: ScrollContainer = %OutputContainer
@onready var response_container: ScrollContainer = %ResponseContainer

@export var event_message_sub:MessageSubscription
@export var txn_sub:TransactionSubscription

@export var use_slot:bool = true
@export var use_custom_starter:bool = true

func _ready() -> void:
    OS.set_environment("RUST_BACKTRACE", "full")
    OS.set_environment("RUST_LOG", "full")
    var test = I128.new()
    test._init_from_string("100000000000000000")
    push_warning(test.to_bytes())
    var call_result_scroll = output_text.get_v_scroll_bar()
    var call_response_scroll = sub_output.get_v_scroll_bar()
    call_response_scroll.scrolling.connect(_on_scrolling.bind(call_response_scroll,call_result_scroll))
    call_result_scroll.scrolling.connect(_on_scrolling.bind(call_result_scroll,call_response_scroll))
    
    torii_client.world_address = world_address
    var env = "Slot" if use_slot else "Local"
    print_rich("[color=yellow]Using %s[/color]" % env)
    
    torii_client.torii_url = torii_url

    torii_client.create_client()
    #print(torii_client.get_world_metadata())

func _torii_logger(_msg:String):
    prints("[TORII LOGGER]", _msg)

func _on_events_message(args:Dictionary) -> void:
    push_warning("CALLBACK EVENTS MESSAGE", args)
    var model_data:Dictionary = args["models"][0]
    var model:String = model_data.keys()[0]

    if model in events:
        var value = model_data[model]["val"]
        if value is Dictionary:
            var new_dict:Dictionary = {}
            for key in value.keys():
                new_dict[key] = bigint_to_string(value[key]) 
            value = new_dict
        value = bigint_to_string(value)
        sub_output.call_deferred("append_text","[color=yellow]%s:[/color] %s\n" % [model, value])

func bigint_to_string(value):
    if value is I128: return value.to_string()
    if value is U128: return value.to_string()
    if value is U256: return value.to_string()
    return value

func _on_transaction(args:Dictionary) -> void:
    push_warning("CALLBACK TXN MESSAGE", args)

func _on_torii_client_client_connected(success: bool) -> void:
    torii_client.set_logger_callback(_torii_logger)
    await get_tree().create_timer(0.1).timeout
    torii_client.on_event_message_update(_on_events_message, event_message_sub)
    await get_tree().process_frame
    torii_client.on_transaction(_on_transaction, txn_sub)

    account.create(rpc_url, account_address, private_key)

    account.set_block_id()

    if account.is_account_valid():
        for selector in tests.keys():
            var data:Array = tests[selector]
            if selector == "validate_u128":
                var result = []
                for i in data:
                    var u256 = U256.new()
                    u256._init_from_int(i)
                    result.append(u256)
                    push_warning(u256.to_string())
                data = result
                    
            if data.is_empty():
                account.execute_raw(current_contract, selector)
                continue
                
            for calldata in data:
                account.execute_raw(current_contract, selector, [calldata])
                await get_tree().process_frame
                # TODO: Sequencer
                await get_tree().create_timer(0.6).timeout # Workaround for missing events
                

func _on_account_transaction_failed(error_message: Dictionary) -> void:
    var selector_data = "[color=cyan]Selector: [/color]%s[color=yellow] Calldata: [/color] %s [color=Red]Error[/color]\n" % [error_message["selector"], error_message["calldata"]]
    var error:String = error_message["error"]
    var regex = RegEx.new()
    regex.compile("\\(([^\\)]+)\\)")
    var regex_result = regex.search_all(error)
    var error_msg = "[color=red]%s[/color]\n" % [regex_result[-1].get_string()]
    #push_warning(stripped_error[-1].get_string())
    output_text.append_text(selector_data)
    sub_output.append_text(error_msg)


func _on_account_transaction_executed(success_message: Dictionary) -> void:
    var output = "[color=cyan]Selector: [/color]%s[color=yellow] Calldata: [/color] %s \n" % [success_message["selector"], success_message["calldata"]]
    var value = success_message["calldata"][0]
    output_text.append_text(output)
    var selector:String = success_message["selector"]
    selector = selector.replace("validate", "validated")
    var event = "%s-%s" % [contract_namespace, selector.to_pascal_case()]
    #sub_output.append_text("[color=yellow]%s[/color]\n" % event)
    
func _on_scrolling(active:VScrollBar, mirror:VScrollBar):
    mirror.value = active.value
    
