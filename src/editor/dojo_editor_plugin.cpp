//
// Created by hazel on 26/9/25.
//

#include "editor/dojo_editor_plugin.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "tools/logger.hpp"

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
    if (!Engine::get_singleton()->is_editor_hint())
    {
        return;
    }
    init_config();
    add_tool_menu_item("Reset Dojo ProjectSettings to default", callable_mp(this, &DojoEditorPlugin::reset_project_settings));
    add_tool_menu_item("Print godot-dojo Version", callable_mp(this, &DojoEditorPlugin::print_version));
    Logger::success_extra("GodotDojo", _get_plugin_version());
}

void DojoEditorPlugin::_exit_tree()
{
    Logger::success_extra("DojoEditorPlugin", "Exiting Tree");
    remove_tool_menu_item("Reset Dojo ProjectSettings to default");
    remove_tool_menu_item("Print godot-dojo Version");
    Logger::success_extra("DojoEditorPlugin", "Tree exited");
}

String DojoEditorPlugin::_get_plugin_version() const
{
#ifdef VERSION_STR
    return VERSION_STR;
#else
    return "0.0.0";
#endif
}

void DojoEditorPlugin::reset_project_settings()
{
    Logger::debug_extra("Dojo", "Resetting Dojo ProjectSettings");
    init_config(true);
}

void DojoEditorPlugin::print_version()
{
    Logger::_print_typed("green","GodotDojo", _get_plugin_version());
}

void DojoEditorPlugin::init_config(bool reset)
{
    if (Engine::get_singleton()->is_editor_hint())
    {
        set_setting("dojo/config/katana_url", "http://localhost:5050", reset, false);
        set_setting("dojo/config/contract_address", "0x0", reset, false);
        // Torii Config
        // TODO: multiple contracts
        set_setting("dojo/config/torii/torii_url", "http://localhost:8080", reset, false);
        // set_setting("dojo/config/torii/contract_addresses", TypedArray<String>(), reset);
        set_setting("dojo/config/torii/worlds", TypedArray<String>(), reset, false);
        // Account
        set_setting("dojo/config/account/address", "0x0",
                    reset, false);
        set_setting("dojo/config/account/public_key",
                    "0x0", reset, false);
        set_setting("dojo/config/account/private_key",
                    "0x0", reset, false);

        // Fixed Point
        set_setting("dojo/config/fixed_point/default", 128, reset, false);
        set_setting("dojo/config/fixed_point/64", 40, reset, false);
        set_setting("dojo/config/fixed_point/128", 60, reset, false);
        set_setting("dojo/config/fixed_point/256", 123, reset, false);

        // Debug
        set_setting("dojo/config/debug/error", true, reset, false);
        set_setting("dojo/config/debug/warning", true, reset, false);
        set_setting("dojo/config/debug/info", false, reset, false);
        set_setting("dojo/config/debug/debug", false, reset, false);
        set_setting("dojo/config/debug/success", true, reset, false);

        ProjectSettings::get_singleton()->save();
    }
}

void DojoEditorPlugin::set_setting(const String& setting, const Variant& value, const bool& force, bool p_save)
{
    ProjectSettings* settings = ProjectSettings::get_singleton();
    bool changed = false;
    if (force)
    {
        Logger::info("Forcing init of config", setting);
        settings->set_setting(setting, value);
        changed = true;
    }
    else if (!settings->has_setting(setting))
    {
        settings->set_setting(setting, value);
        settings->set_as_basic(setting, true);
        // settings->set_initial_value(setting, value);
        Logger::info("Inited config", setting, "with value", value);
        changed = true;
    }
    
    if (changed && p_save)
    {
        settings->save();
    }
}
