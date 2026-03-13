#include "editor/export_plugin.hpp"

#include <godot_cpp/classes/editor_export_platform_web.hpp>

#include "tools/logger.hpp"

bool GodotDojoExportPlugin::_supports_platform(const Ref<EditorExportPlatform> &p_platform) const {
    if (p_platform->get_os_name() == "Web"){
        return true;
    }
    return false;
}

void GodotDojoExportPlugin::_export_begin(const PackedStringArray &p_features, bool p_is_debug, const String &p_path,
    uint32_t p_flags) {
    Logger::debug_extra("Web BEGIN", p_features, p_is_debug, p_path, p_flags);
}

void GodotDojoExportPlugin::_export_end() {
    Logger::debug_extra("Web END", "GINISHED");
}

PackedStringArray GodotDojoExportPlugin::_get_export_features(const Ref<EditorExportPlatform> &p_platform,
    bool p_debug) const {
    Logger::debug_extra("Web FEATURES", p_platform, p_debug);
    if (p_platform->get_os_name() == "Web") {
        Ref<EditorExportPlatformWeb>
    }
    Logger::debug_extra("Web Extension", p_platform->get_option("variant/extension_support"));
    return {
        Array::make(
            "variant/extensions_support"
            )
    };
}

Dictionary GodotDojoExportPlugin::_get_export_options_overrides(const Ref<EditorExportPlatform> &p_platform) const {
    if (p_platform->get_os_name() == "Web")
    {Logger::debug_extra("Web OPTIONS OVERRIDES", p_platform);}
    return {};
}

TypedArray<Dictionary> GodotDojoExportPlugin::_get_export_options(const Ref<EditorExportPlatform> &p_platform) const {
    // if (p_platform->get_os_name() == "Web")
    // {Logger::debug_extra("Web OPTIONS", p_platform);}
    return {};
}

bool GodotDojoExportPlugin::_should_update_export_options(const Ref<EditorExportPlatform> &p_platform) const {
    // if (p_platform->get_os_name() == "Web")
    // {Logger::debug_extra("Web OPTIONS SHOULD", p_platform);}
    return false;
}

void GodotDojoExportPlugin::_bind_methods() {
}
