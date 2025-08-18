//
// Created by hazel on 12/05/25.
//
#include "dojo_c_gdextension.h"

#include "godot_cpp/classes/project_settings.hpp"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "tools/logger.h"
#include "dojo_types.h"

DojoC* DojoC::singleton = nullptr;

void DojoC::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoC::set_enabled);
    ClassDB::bind_method(D_METHOD("get_enabled"), &DojoC::get_enabled);

    ClassDB::bind_static_method("DojoC", D_METHOD("init_config", "reset"), &DojoC::init_config);
    ClassDB::bind_static_method("DojoC", D_METHOD("init_setting", "setting", "value", "force"), &DojoC::set_setting);
    // ClassDB::bind_method(D_METHOD("controller_connect"), &DojoC::controller_connect);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");
}

DojoC::DojoC()
{
    singleton = this;
    enabled = true;
    init_config();
}

DojoC::~DojoC()
{
    singleton = nullptr;
}

void DojoC::init_config(bool reset)
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        Logger::debug_extra("DojoC", "is running in editor mode");
        set_setting("dojo/config/katana/rpc_url", "http://localhost:5050", reset);
        set_setting("dojo/config/fixed_point/default", 24, reset);
        set_setting("dojo/config/fixed_point/64", 40, reset);
        set_setting("dojo/config/fixed_point/128", 60, reset);
        set_setting("dojo/config/fixed_point/256", 123, reset);
    }
}

bool DojoC::set_setting(const String& setting, const Variant& value, const bool& force)
{
    ProjectSettings* settings = ProjectSettings::get_singleton();
    if (force)
    {
        Logger::info("Forcing init of config", setting);
    }
    if (settings->has_setting(setting) == false || force)
    {
        settings->set_setting(setting, value);
        settings->set_as_basic(setting, true);
        settings->set_initial_value(setting, value);
        Logger::info("Inited config", setting, "with value", value);
        settings->save();
        return true;
    }
    return false;
}
