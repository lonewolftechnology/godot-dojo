//
// Created by hazel on 26/9/25.
//

#include "editor/dojo_editor_plugin.hpp"

#include <godot_cpp/classes/editor_interface.hpp>

#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/popup_menu.hpp"
#include "godot_cpp/classes/reg_ex.hpp"
#include "tools/logger.hpp"

DojoEditorPlugin::DojoEditorPlugin()
{
    tool_menu = nullptr;
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

    tool_menu = memnew(PopupMenu);
    tool_menu->set_name("DojoToolsMenu");
    tool_menu->add_item("Reset Dojo ProjectSettings to default", 0);
    tool_menu->add_item("Print godot-dojo Version", 1);
    tool_menu->add_item("Check for Updates", 2);
    tool_menu->connect("id_pressed", callable_mp(this, &DojoEditorPlugin::_on_menu_item_pressed));

    add_tool_submenu_item("Godot Dojo Tools", tool_menu);
    Logger::success_extra("GodotDojo", _get_plugin_version());
}

void DojoEditorPlugin::_exit_tree()
{
    Logger::success_extra("DojoEditorPlugin", "Exiting Tree");
    remove_tool_menu_item("Godot Dojo Tools");
    if (tool_menu)
    {
        tool_menu->queue_free();
    }
    tool_menu = nullptr;
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

void DojoEditorPlugin::_on_menu_item_pressed(int id)
{
    switch (id)
    {
        case 0:
            reset_project_settings();
            break;
        case 1:
            print_version();
            break;
        case 2:
            check_for_updates();
            break;
    }
}

void DojoEditorPlugin::check_for_updates()
{
    HTTPRequest* http = memnew(HTTPRequest);
    add_child(http);
    http->connect("request_completed", callable_mp(this, &DojoEditorPlugin::_on_release_request_completed).bind(http));
    
    PackedStringArray headers;
    headers.push_back("User-Agent: GodotDojo");
    http->request("https://api.github.com/repos/lonewolftechnology/godot-dojo/releases", headers);
    Logger::info("Checking for updates...");
}

void DojoEditorPlugin::_on_release_request_completed(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body, HTTPRequest* http)
{
    if (result != HTTPRequest::RESULT_SUCCESS)
    {
        Logger::error("Failed to check for updates. Request failed.");
        http->queue_free();
        return;
    }

    if (response_code != 200)
    {
        Logger::error("Failed to check for updates. Response code:", response_code);
        http->queue_free();
        return;
    }

    Ref<JSON> json;
    json.instantiate();
    Error err = json->parse(body.get_string_from_utf8());
    if (err != OK)
    {
        Logger::error("Failed to parse update response.");
        http->queue_free();
        return;
    }

    Array releases = json->get_data();
    if (!releases.is_empty())
    {
        Dictionary latest_release = releases[0];
        String latest_version = latest_release.get("tag_name", "Unknown");
        String html_url = latest_release.get("html_url", "");
        String published_at = latest_release.get("published_at", "Unknown Date");
        String body_md = latest_release.get("body", "");
        bool is_prerelease = latest_release.get("prerelease", false);
        
        // Conversión simple de Markdown a BBCode
        Ref<RegEx> regex;
        regex.instantiate();

        // Bloques de código ``` ... ```
        regex->compile("(?s)```(.*?)```");
        body_md = regex->sub(body_md, "[code]$1[/code]", true);

        // Código en línea ` ... `
        regex->compile("`([^`]+)`");
        body_md = regex->sub(body_md, "[code]$1[/code]", true);

        // Negrita ** ... **
        regex->compile("\\*\\*(.*?)\\*\\*");
        body_md = regex->sub(body_md, "[b]$1[/b]", true);

        // Encabezados # ...
        regex->compile("(?m)^#+\\s+(.*)$");
        body_md = regex->sub(body_md, "[b][font_size=10]$1[/font_size][/b]", true);

        // Enlaces texto
        regex->compile("\\[(.*?)\\]\\((.*?)\\)");
        body_md = regex->sub(body_md, "[url=$2]$1[/url]", true);

        // Listas - ...
        regex->compile("(?m)^\\s*-\\s+(.*)$");
        body_md = regex->sub(body_md, " • $1", true);

        String output = "\n[b][color=green]GodotDojo Update Check[/color][/b]\n";
        output += "[b]Release Notes:[/b]\n" + body_md + "\n";
        output += "[b]Version:[/b] " + latest_version + (is_prerelease ? " [color=yellow](Pre-release)[/color]" : "") + "\n";
        output += "[b]Date:[/b] " + published_at + "\n";
        output += "[b]URL:[/b] [url]" + html_url + "[/url]\n";

        UtilityFunctions::print_rich(output);
        
        String current_version = _get_plugin_version();
        Logger::info("Current version:", current_version);
    }
    
    http->queue_free();
}
