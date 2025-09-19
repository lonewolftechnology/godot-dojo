#ifndef DOJO_EXPORT_HPP
#define DOJO_EXPORT_HPP

#include <godot_cpp/classes/editor_export_platform.hpp>

#include "godot_cpp/classes/editor_export_plugin.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/variant/string.hpp"

using namespace godot;

class DojoExportPlugin : public EditorExportPlugin {
    GDCLASS(DojoExportPlugin, EditorExportPlugin)

public:
    DojoExportPlugin();
    ~DojoExportPlugin();

    // Métodos principales del plugin
    String _get_name() const override {
        return "DojoExport";
    }
    void _export_begin(const PackedStringArray &features, bool is_debug, const String &path, uint32_t flags) override;
    void _export_file(const String &path, const String &type, const PackedStringArray &features) override;
    bool _supports_platform(const Ref<EditorExportPlatform> &p_platform) const override;

protected:
    static void _bind_methods() {}

private:
    void _copy_file_to_export(const String& p_file_name, const String& p_export_path);
    String build_mode;
    bool dojo_files_added;
};

#endif // DOJO_EXPORT_HPP