//
// Created by hazel on 26/9/25.
//
#ifndef DOJO_EDITOR_PLUGIN_H
#define DOJO_EDITOR_PLUGIN_H

#include "godot_cpp/classes/editor_plugin.hpp"
#include "android_export_plugin.h"
#include "tools/logger.h"
using namespace godot;

class DojoEditorPlugin : public EditorPlugin {
    GDCLASS(DojoEditorPlugin, EditorPlugin)


public:
    Ref<AndroidExportPlugin> android_export_plugin;

    DojoEditorPlugin() {
    }

    ~DojoEditorPlugin() override {
    }

    void _enter_tree() override {
        add_tool_menu_item("Reset Dojo ProjectSettings to default",
                           callable_mp_static(&DojoEditorPlugin::reset_project_settings));

        android_export_plugin.instantiate();
        add_export_plugin(android_export_plugin);
    }

    void _exit_tree() override {
        remove_tool_menu_item("Reset Dojo ProjectSettings to default");

        remove_export_plugin(android_export_plugin);
        android_export_plugin.unref();
    }

    String _get_plugin_name() const override {
        return "DojoC Tools";
    }

    static void reset_project_settings() {
        Logger::debug_extra("Dojo", "Resetting Dojo ProjectSettings");
        DojoC::init_config(true);
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("DojoEditorPlugin", D_METHOD("reset_project_settings"),
                                    &DojoEditorPlugin::reset_project_settings);
    }
};

#endif // DOJO_EDITOR_PLUGIN_H
