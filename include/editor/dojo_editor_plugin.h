//
// Created by hazel on 26/9/25.
//
#ifndef DOJO_EDITOR_PLUGIN_H
#define DOJO_EDITOR_PLUGIN_H

#include "dojo_c_gdextension.h"
#include "godot_cpp/classes/editor_plugin.hpp"
#include "tools/logger.h"
using namespace godot;

class DojoEditorPlugin : public EditorPlugin
{
    GDCLASS(DojoEditorPlugin, EditorPlugin)

public:
    DojoEditorPlugin()
    {
        Logger::success_extra("DojoEditorPlugin", "Constructed");
        add_tool_menu_item("Reset Dojo ProjectSettings to default", callable_mp_static(&DojoEditorPlugin::reset_project_settings));

    }
    ~DojoEditorPlugin()
    {
        Logger::success_extra("DojoEditorPlugin", "Destroyed");
        // Maybe not needed as you can't actually toogle the gdextension on/off, you need to remove the files to disable it
        // remove_tool_menu_item("Reset Dojo ProjectSettings to default");

    }

    String _get_plugin_name() const override
    {
        return "DojoC Tools";
    }

    static void reset_project_settings()
    {
        Logger::debug_extra("Dojo", "Resetting Dojo ProjectSettings");
        DojoC::init_config(true);
    }

protected:
    static void _bind_methods()
    {
    }
};

#endif // DOJO_EDITOR_PLUGIN_H
