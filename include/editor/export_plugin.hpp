#pragma once

#include "godot_cpp/classes/editor_export_plugin.hpp"
#include "godot_cpp/classes/editor_export_platform.hpp"

using namespace godot;

class GodotDojoExportPlugin : public EditorExportPlugin {
    GDCLASS(GodotDojoExportPlugin, EditorExportPlugin)

public:
    GodotDojoExportPlugin(){};
    ~GodotDojoExportPlugin(){};

    String _plugin_name = "GodotDojo";

    bool _supports_platform(const Ref<EditorExportPlatform> &p_platform) const override;
    void _export_begin(const PackedStringArray &p_features, bool p_is_debug, const String &p_path, uint32_t p_flags) override;
    void _export_end() override;

    Dictionary _get_export_options_overrides(const Ref<EditorExportPlatform> &p_platform) const override;
    TypedArray<Dictionary> _get_export_options(const Ref<EditorExportPlatform> &p_platform) const override;
    bool _should_update_export_options(const Ref<EditorExportPlatform> &p_platform) const override;

    String _get_name() const override { return _plugin_name; }

    static String _get_install_path();

protected:
    static void _bind_methods();
};