//
// Created by hazel on 26/9/25.
//

#include "editor/dojo_editor_plugin.h"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "tools/logger.h"

DojoEditorPlugin::DojoEditorPlugin()
{
    enabled = true;
}

DojoEditorPlugin::~DojoEditorPlugin()
{
    enabled = false;
}

void DojoEditorPlugin::_enter_tree()
{
    init_config();
    add_tool_menu_item("Reset Dojo ProjectSettings to default", callable_mp(this, &DojoEditorPlugin::reset_project_settings));
}

void DojoEditorPlugin::_exit_tree()
{
    Logger::success_extra("DojoEditorPlugin", "Exiting Tree");
    remove_tool_menu_item("Reset Dojo ProjectSettings to default");
    Logger::success_extra("DojoEditorPlugin", "Tree exited");
}

void DojoEditorPlugin::reset_project_settings()
{
    Logger::debug_extra("Dojo", "Resetting Dojo ProjectSettings");
    init_config(true);
}

void DojoEditorPlugin::init_config(bool reset)
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        set_setting("dojo/config/katana_url", "http://localhost:5050", reset);
        set_setting("dojo/config/contract_address", "0x0", reset);
        // Torii Config
        // TODO: multiple contracts
        set_setting("dojo/config/torii/torii_url", "http://localhost:8080", reset);
        // set_setting("dojo/config/torii/contract_addresses", TypedArray<String>(), reset);
        set_setting("dojo/config/torii/worlds", TypedArray<String>(), reset);
        // Account
        set_setting("dojo/config/account/address", "0x0",
                    reset);
        set_setting("dojo/config/account/public_key",
                    "0x0", reset);
        set_setting("dojo/config/account/private_key",
                    "0x0", reset);

        // Fixed Point
        set_setting("dojo/config/fixed_point/default", 128, reset);
        set_setting("dojo/config/fixed_point/64", 40, reset);
        set_setting("dojo/config/fixed_point/128", 60, reset);
        set_setting("dojo/config/fixed_point/256", 123, reset);

        // Debug
        set_setting("dojo/config/debug/error", true, reset);
        set_setting("dojo/config/debug/warning", true, reset);
        set_setting("dojo/config/debug/info", false, reset);
        set_setting("dojo/config/debug/debug", false, reset);
        set_setting("dojo/config/debug/success", true, reset);


    }
}

void DojoEditorPlugin::set_setting(const String& setting, const Variant& value, const bool& force)
{
    ProjectSettings* settings = ProjectSettings::get_singleton();
    if (force)
    {
        Logger::info("Forcing init of config", setting);
        settings->set_setting(setting, value);

    }
    if (settings->has_setting(setting) == false)
    {
        settings->set_setting(setting, value);
        settings->set_as_basic(setting, true);
        // settings->set_initial_value(setting, value);
        Logger::info("Inited config", setting, "with value", value);

    }
    settings->save();
}
