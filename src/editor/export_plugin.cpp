#include "editor/export_plugin.hpp"

#include <godot_cpp/classes/editor_export_platform_web.hpp>
#include <godot_cpp/classes/gd_extension_manager.hpp>

#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "tools/logger.hpp"

bool GodotDojoExportPlugin::_supports_platform(const Ref<EditorExportPlatform>& p_platform) const
{
    if (p_platform->get_os_name() == "Web")
    {
        return true;
    }
    return false;
}

void copy_dir_recursive(const String& from_path, const String& to_path)
{
    Ref<DirAccess> dir = DirAccess::open(from_path);
    if (dir.is_valid())
    {
        DirAccess::make_dir_recursive_absolute(to_path);
        dir->list_dir_begin();
        String file_name = dir->get_next();
        while (file_name != "")
        {
            if (!file_name.begins_with("."))
            {
                String src = vformat("%s/%s", from_path, file_name);
                String dest = vformat("%s/%s", to_path, file_name);

                if (dir->current_is_dir())
                {
                    copy_dir_recursive(src, dest);
                }
                else
                {
                    String global_src = ProjectSettings::get_singleton()->globalize_path(src);
                    DirAccess::copy_absolute(global_src, dest);
                }
            }
            file_name = dir->get_next();
        }
    }
    else
    {
        Logger::warning("Origin not found ", from_path);
    }
}

void GodotDojoExportPlugin::_export_begin(const PackedStringArray& p_features, bool p_is_debug, const String& p_path,
                                          uint32_t p_flags)
{
    if (p_features.has("web"))
    {
        Logger::debug_extra("Web BEGIN", p_features, p_is_debug, p_path, p_flags, _get_install_path());
        String dojo_path = p_path.get_base_dir();
        copy_dir_recursive(_get_install_path().path_join("web/dojo"), dojo_path);
    }
}


void GodotDojoExportPlugin::_export_end()
{
    Logger::debug_extra("Export", "END-FINISHED");
}

Dictionary GodotDojoExportPlugin::_get_export_options_overrides(const Ref<EditorExportPlatform>& p_platform) const
{
    if (p_platform->get_os_name() == "Web")
    {
        Logger::debug_extra("Web OPTIONS OVERRIDES", p_platform);
        Logger::debug_extra("Web Extension", p_platform->get("variant/extensions_support"));
        Dictionary _override = {};
        _override["variant/extensions_support"] = true;
        _override["html/head_include"] = R"(
            <script type="module">
                import init, * as dojo from './godot_dojo_core.js';
            	console.log("Initializing WASM...");
            	await init();
            	window.dojo = dojo;

            </script>
        )";
        return _override;
    }
    return {};
}

TypedArray<Dictionary> GodotDojoExportPlugin::_get_export_options(const Ref<EditorExportPlatform>& p_platform) const
{
    if (p_platform->get_os_name() == "Web")
    {
        // Logger::debug_extra("Web OPTIONS", p_platform);
        // Logger::debug_extra("Web Extension", p_platform->get("variant/extension_support"));
    }
    return {};
}

bool GodotDojoExportPlugin::_should_update_export_options(const Ref<EditorExportPlatform>& p_platform) const
{
    if (p_platform->get_os_name() == "Web")
    {
        // Logger::debug_extra("Web OPTIONS SHOULD", p_platform);
        // Logger::debug_extra("Web Extension", p_platform->get("variant/extension_support"));
    }
    return false;
}

String GodotDojoExportPlugin::_get_install_path()
{
    String _source_path;
    PackedStringArray loaded_exts = GDExtensionManager::get_singleton()->get_loaded_extensions();
    for (const String& ext : loaded_exts)
    {
        Logger::debug_extra("Extension", ext);
        if (ext.ends_with("godot-dojo.gdextension"))
        {
            return ext.get_base_dir();
        }
    }
    return _source_path;
}

void GodotDojoExportPlugin::_bind_methods()
{
    ClassDB::bind_static_method(get_class_static(),D_METHOD("_get_install_path"), &GodotDojoExportPlugin::_get_install_path);
}
