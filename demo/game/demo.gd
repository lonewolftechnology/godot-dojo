# CHAIN ID WP_GODOT_DEMO_ROOKIE
# KATANA https://api.cartridge.gg/x/godot-demo-rookie/katana
# TORII https://api.cartridge.gg/x/godot-demo-rookie/torii
# WORLD 0x073c2b317136214562b608523812f369a05efe67257842a17c4985ce6d390be7
# Actions 0x05d64ed5dff420d1d1f1c36958b6c4b0983868e5d0072f3fc928859b95d72a24

# hazel 0x3dc821653fa9ed84324821b26afd5775b128921b9436327211949947a888
# dtodice 0x75c10973fc5fdfa2f7ff5875f10441e546d98f37f40b97a663f89bdaf81a0

class_name DemoGame
extends Node


enum Directions {
    LEFT = 0,
    RIGHT = 1,
    UP = 2,
    DOWN = 3,
}

@export var spawn_call : DojoCall
@export var move_call : DojoCall

@export var connection : DojoConnection 

@export var controller_query:DojoControllerQuery

@onready var label_username: Label = %LabelUsername
@onready var label_address: Label = %LabelAddress
@onready var label_moves: Label = %LabelMoves

@onready var controllers_manager: ControllersManager = %ControllersManager

var _player_address : String = ""

func _ready() -> void:
    OS.set_environment("RUST_BACKTRACE", "full")
    OS.set_environment("RUST_LOG", "debug")
    connection.controller_account.current_user_info.connect(_on_controller_current_user_info)

func _on_controller_current_user_info(data:Dictionary) -> void:
    var address : String = data['address']
    var username : String = data['username']
    _player_address = address
    label_username.text = "Username: %s"%username
    var player_entity := controllers_manager.spawn_entity(address,true)
    player_entity.set_username(username)


func _on_events(args:Dictionary) -> void:
    push_warning("CALLBACK EVENTS", args)
    _handle_callback(args)

func _on_entities(args:Dictionary) -> void:
    push_warning("CALLBACK ENTITIES", args)
    _handle_callback(args)

func _handle_callback(args:Dictionary) -> void:
    print(args)
    return
    var data = args["data"]
    # This is contract specific and data structure may change
    var result_data : Dictionary
    for entry : Dictionary in data:
        result_data.merge(entry)

    await get_tree().process_frame
    if result_data.has("Vec2"):
        var pos := Vector2(result_data['Vec2']['x'], result_data['Vec2']['y'] )
        var id : String = result_data['player']
        controllers_manager.move_controller(id, pos)
    if result_data.has("remaining"):
        var id : String = result_data['player']
        if id == _player_address:
            label_moves.text = "Moves: %s" % result_data['remaining']


func spawn() -> void:
    connection.controller_account.execute_from_outside(spawn_call)


func _on_start_screen_entered() -> void:
    start()

func start() -> void:
    await get_tree().create_timer(0.1).timeout
    connection.create_subscriptions(_on_events,_on_entities)
    
    await get_tree().create_timer(0.1).timeout
    spawn()
    
    await get_tree().create_timer(0.1).timeout
    
    var parsed : Dictionary = get_entities()
    
    for id in parsed.keys():
        controllers_manager.spawn_entity(id)
    
    
    #await get_tree().create_timer(0.1).timeout
    _update_entities(parsed)
    
    
    get_controllers(parsed.keys())


func get_controllers(addrs:Array = []) -> void:
    print("&&&&&&& Addresses: %s\n"%str(addrs))
    var data = connection.client.get_controllers(controller_query)
    print("######\nControllers: %s\n######\n"%str(data))
    for controller in data:
        var id : String = controller["address"]
        var username : String = controller["username"]
        var entity : GenericEntity = controllers_manager.get_entity(id)
        if entity == null:
            entity = controllers_manager.spawn_entity(id)
        
        entity.set_username(username)


func get_entities() -> Dictionary:
    var data = connection.client.get_entities(DojoQuery.new())
    var parsed_entities : Dictionary = {}
    for entity in data:
        for model:Dictionary in entity.models:
            var id : String = ""
            var position : Vector2 = Vector2.ZERO
            var can_move : bool = true
            var remaining : int = 100
            for key in model:
                var entry:Dictionary = model[key]
                #print("###### KEY: %s\n###### ENTRY: %s\n"%[str(key),str(entry)])
                id = entry["player"]
                match key:
                    "dojo_starter-Position":#,"dojo_starter-PositionSigned","dojo_starter-PositionI32":
                        var v : Dictionary = {}
                        
                        if entry.has("vec"):
                            v = entry["vec"]
                            
                        elif entry.has("Vector2Signed"):
                            v = entry["Vector2Signed"]
                        var x := v["x"] as float
                        var y := v["y"] as float
                        position = Vector2(x,y)
                        
                        
                        if not parsed_entities.has(id):
                            parsed_entities[id] = {}
                        
                        parsed_entities[id] = {}
                        parsed_entities[id].merge({"position": position})
                    "dojo_starter-Moves":
                        remaining = entry["remaining"] as int
                        can_move = entry["can_move"] as bool
                        
                        if not parsed_entities.has(id):
                            parsed_entities[id] = {}
                            
                        parsed_entities[id]["remaining"] = remaining
                        parsed_entities[id]["can_move"] = can_move
                    "dojo_starter-U128Value":
                        pass
                    "dojo_starter-U256Value":
                        pass
    return parsed_entities


func _update_entities(parsed_entities:Dictionary) -> void:
    for key in parsed_entities.keys():
        var id : String = key
        if parsed_entities[key].has("position"):
            var position : Vector2 = parsed_entities[key]["position"]
            controllers_manager.move_controller(id,position)



func _move(dir:Directions) -> void:
    move_call.calldata[0] = dir
    move_call.calldata[1] = int(%StepsAmount.value)
    connection.controller_account.execute_from_outside(move_call)

func _on_arrow_left_pressed() -> void:
    _move(Directions.LEFT)

func _on_arrow_up_pressed() -> void:
    _move(Directions.UP)

func _on_arrow_down_pressed() -> void:
    _move(Directions.DOWN)

func _on_arrow_right_pressed() -> void:
    _move(Directions.RIGHT)


#func _on_move_to_pressed() -> void:
    #var x:int = %Vx.value
    #var y:int = %Vy.value
    #move_to.calldata[0] = Vector2i(x,y)
    #connection.controller_account.execute_from_outside(move_to)


func _on_disconnect_pressed() -> void:
    connection.controller_account.disconnect_controller()
    controllers_manager.clear()
    await get_tree().process_frame	
    get_tree().reload_current_scene()


func _on_move_to_pressed() -> void:
    var testing:DojoControllerQuery = DojoControllerQuery.new()
    testing.usernames = ["hazel", "danntheoperator", "dtodice"]
    var data = connection.client.get_controllers(testing)
    print("######\nControllers: %s\n######\n"%str(data))
