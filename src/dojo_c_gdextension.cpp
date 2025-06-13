//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/time.hpp>

#include <debug_macros.h>
#include <variant/primitive.h>

#include "classes/controller_account.h"


using namespace godot;
DojoC* DojoC::singleton = nullptr;
dojo_bindings::FieldElement* actions;

void DojoC::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoC::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &DojoC::get_enabled);

    // ClassDB::bind_method(D_METHOD("controller_connect"), &DojoC::controller_connect);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");

}

DojoC::DojoC()
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        UtilityFunctions::push_warning("DojoC is running in editor mode");
        return;
    }
    // Initialize any variables here.
    singleton = this;
    enabled = true;
}

DojoC* DojoC::get_singleton()
{
    return singleton;
}

DojoC::~DojoC()
{
    // Add your cleanup here.
    singleton = nullptr;

}