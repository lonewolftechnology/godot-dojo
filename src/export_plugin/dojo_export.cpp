#include "export_plugin/dojo_export.h"

#include <godot_cpp/classes/editor_export_platform_web.hpp>
#include <godot_cpp/classes/editor_file_system.hpp>
#include <godot_cpp/classes/editor_interface.hpp>

#include "godot_cpp/classes/project_settings.hpp"
#include "tools/logger.h"

DojoExportPlugin::DojoExportPlugin() {
    // Constructor
    Logger::info("DojoExport: Initializing...");
    dojo_files_added = false;
}

DojoExportPlugin::~DojoExportPlugin() {
    // Destructor
    Logger::info("DojoExport: Destroying...");
}

void DojoExportPlugin::_export_begin(const PackedStringArray &features, bool is_debug, const String &path, uint32_t flags) {
    if (!features.has("web")) {
        Logger::debug_extra("DojoExport", "Not a web export, skipping.");
        return;
    }

    String build_mode = is_debug ? "debug" : "release";
    Logger::debug_extra("DojoExport", "Web export detected. Build mode: " + build_mode);

    Logger::debug_extra("DojoExport", "adding Dojo files...");

    String js_file = "dojo_c_" + build_mode + ".js";
    String wasm_file = "dojo_c_" + build_mode + ".wasm";

    _copy_file_to_export(js_file, path);
    _copy_file_to_export(wasm_file, path);

}

void DojoExportPlugin::_export_file(const String &path, const String &type, const PackedStringArray &features) {
}

void DojoExportPlugin::_copy_file_to_export(const String &p_file_name, const String &p_export_path) {
    String addon_dir = "res://addons/godot-dojo/";
    String file_path = addon_dir + p_file_name;

    if (FileAccess::file_exists(file_path)) {
        // For web exports, p_export_path is the .zip file. We need the directory.
        String export_dir = p_export_path.get_base_dir();
        String dest_path = export_dir.path_join(p_file_name);

        Error err = DirAccess::copy_absolute(file_path, dest_path);
        if (err == OK) {
            Logger::debug_extra("DojoExport", "Successfully copied " + file_path + " to " + dest_path);
        } else {
            Logger::error("DojoExport: Failed to copy " + file_path + " to " + dest_path + ". Error code: " + itos(err));
        }
    } else {
        Logger::error("DojoExport: File not found with FileAccess::file_exists: " + file_path);
    }
}

bool DojoExportPlugin::_supports_platform(const Ref<EditorExportPlatform> &p_platform) const {
    return p_platform.is_valid() && p_platform->get_os_name() == "Web";
}
