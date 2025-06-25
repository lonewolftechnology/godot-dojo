//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include "godot_cpp/classes/project_settings.hpp"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "tools/logger.h"

using namespace godot;
DojoC* DojoC::singleton = nullptr;

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
        Logger::debug_extra("DojoC", "is running in editor mode");
        ProjectSettings* settings = ProjectSettings::get_singleton();
        if (settings->has_setting("dojo/rpc_url") == false)
        {
            settings->set_setting("dojo/rpc_url", "");
            settings->set_initial_value("dojo/rpc_url", "http://localhost:5050");
            settings->save();
        }
        return;
    }
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