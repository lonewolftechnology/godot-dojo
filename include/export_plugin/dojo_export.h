#ifndef DOJO_EXPORT_HPP
#define DOJO_EXPORT_HPP

#include "godot_cpp/classes/editor_export_plugin.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/variant/string.hpp"

using namespace godot;

class DojoExportPlugin : public EditorExportPlugin {
    GDCLASS(DojoExportPlugin, EditorExportPlugin)

protected:
    static void _bind_methods();

public:
    DojoExportPlugin();
    ~DojoExportPlugin();

    // Métodos principales del plugin
    String _get_name() const override;
    void _export_begin(const PackedStringArray &features, bool is_debug, const String &path, int flags);
    void _export_file(const String &path, const String &type, const PackedStringArray &features) override;
    void _export_end() override;

private:
    String export_path;
    
    // Funciones auxiliares
    bool copy_file_to_export(const String &source_path, const String &dest_relative_path);
    bool ensure_directory_exists(const String &dir_path);
    void copy_files_from_folder(const String &source_folder, const String &dest_folder);
};

#endif // DOJO_EXPORT_HPP