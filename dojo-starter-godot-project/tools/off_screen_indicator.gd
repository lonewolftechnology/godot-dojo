class_name OffScreenIndicator extends Node2D

var indicator:Sprite2D
# TODO This works but it has the flaw that is too dependant of getting the SafeArea node 
#@onready var safe_area:Control = get_tree().get_nodes_in_group("IndicatorSafeArea")[0]

func _ready() -> void:
	indicator = Sprite2D.new()
	add_child(indicator)
	# TODO We should expose this texture to make it easy to change
	indicator.texture = preload("res://assets/arrow_right_p.png")
	indicator.offset.x = -indicator.texture.get_size().x

func _physics_process(delta):
	var canvas = get_canvas_transform()
	var top_left = -canvas.origin / canvas.get_scale() 
	var rect = get_viewport_rect()
	var size = get_viewport_rect().size / canvas.get_scale()
	set_marker_position(Rect2(top_left,size))
	set_marker_rotation()
	
func set_marker_position(bounds:Rect2):
	indicator.global_position.x = clamp(global_position.x, bounds.position.x, bounds.end.x)
	indicator.global_position.y = clamp(global_position.y, bounds.position.y, bounds.end.y)
	
	if bounds.has_point(global_position):
		hide()
	else:
		show()

func set_marker_rotation():
	var angle = (global_position - indicator.global_position).angle()
	indicator.global_rotation = angle
