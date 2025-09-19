#ifndef DOJO_EDITOR_PLUGIN_H
#define DOJO_EDITOR_PLUGIN_H

#include "godot_cpp/classes/editor_plugin.hpp"
#include "export_plugin/dojo_export.h"
#include "tools/logger.h"
using namespace godot;

class DojoEditorPlugin : public EditorPlugin {
    GDCLASS(DojoEditorPlugin, EditorPlugin)

    Ref<DojoExportPlugin> export_plugin;

public:
    DojoEditorPlugin()
    {
        Logger::debug_extra("EditorPlugin", "Creating...");
        export_plugin.instantiate();
        add_export_plugin(export_plugin);
    }
    ~DojoEditorPlugin()
    {
        Logger::debug_extra("EditorPlugin", "Destroying...");
        remove_export_plugin(export_plugin);
        export_plugin.unref();
    }
protected:
    static void _bind_methods() {}
};

#endif // DOJO_EDITOR_PLUGIN_H
