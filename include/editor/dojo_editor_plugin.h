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

    }
    ~DojoEditorPlugin()
    {
        Logger::success_extra("DojoEditorPlugin", "Destroyed");

    }

    void _enter_tree() override
    {
        Logger::success_extra("DojoEditorPlugin", "Entering tree");

        add_tool_menu_item("Reset Dojo ProjectSettings to default", callable_mp_static(&DojoEditorPlugin::reset_project_settings));
        Logger::success_extra("DojoEditorPlugin", "Tree entered");

    }

    void _exit_tree() override
    {
        Logger::success_extra("DojoEditorPlugin", "Exiting Tree");

        remove_tool_menu_item("Reset Dojo ProjectSettings to default");
        Logger::success_extra("DojoEditorPlugin", "Tree exited");

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
