#pragma once

#include "godot_cpp/classes/editor_plugin.hpp"
#include "godot_cpp/classes/http_request.hpp"
#include "tools/logger.hpp"

using namespace godot;

class DojoEditorPlugin : public EditorPlugin
{
    GDCLASS(DojoEditorPlugin, EditorPlugin)

    bool enabled;

public:
    DojoEditorPlugin();
    ~DojoEditorPlugin();

    void _enter_tree() override;
    void _exit_tree() override;

    String _get_plugin_name() const override
    {
        return "GodotDojo Tools";
    }
    String _get_plugin_version() const;

    void set_enabled(bool p_enabled) { enabled = p_enabled; };
    bool get_enabled() const { return enabled; }

    void reset_project_settings();
    void print_version();
    void init_config(bool reset = false);
    void set_setting(const String& setting, const Variant& value, const bool& force = false, bool p_save = true);

    void _on_menu_item_pressed(int id);

    void check_for_updates();
    void _on_release_request_completed(int result, int response_code, const PackedStringArray& headers, const PackedByteArray& body, HTTPRequest* http);

protected:
    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoEditorPlugin::set_enabled);
        ClassDB::bind_method(D_METHOD("get_enabled"), &DojoEditorPlugin::get_enabled);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");

        ClassDB::bind_method(D_METHOD("reset_project_settings"), &DojoEditorPlugin::reset_project_settings);
        ClassDB::bind_method(D_METHOD("print_version"), &DojoEditorPlugin::print_version);
        ClassDB::bind_method(D_METHOD("_get_plugin_version"), &DojoEditorPlugin::_get_plugin_version);
        ClassDB::bind_method(D_METHOD("check_for_updates"), &DojoEditorPlugin::check_for_updates);
        ClassDB::bind_method(D_METHOD("init_config", "reset"), &DojoEditorPlugin::init_config);
        ClassDB::bind_method(D_METHOD("init_setting", "setting", "value", "force", "save"),
                                    &DojoEditorPlugin::set_setting, DEFVAL(true), DEFVAL(false));
    }
};