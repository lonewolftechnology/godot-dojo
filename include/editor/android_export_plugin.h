#ifndef ANDROID_EXPORT_PLUGIN_H
#define ANDROID_EXPORT_PLUGIN_H

#include "godot_cpp/classes/editor_export_platform.hpp"
#include "godot_cpp/classes/editor_export_plugin.hpp"
#include "tools/logger.h"

using namespace godot;

class AndroidExportPlugin : public EditorExportPlugin {
    GDCLASS(AndroidExportPlugin, EditorExportPlugin);

public:
    String _plugin_name = "GodotDojoAndroidPlugin";

    AndroidExportPlugin(){};

    ~AndroidExportPlugin() override{};

    bool _supports_platform(const Ref<EditorExportPlatform> &p_platform) const override {
        if (p_platform->get_class() == "EditorExportPlatformAndroid") {
            return true;
        }
        return false;
    };

    PackedStringArray _get_android_libraries(const Ref<EditorExportPlatform> &p_platform, bool p_debug) const override {
        PackedStringArray _libs = {};
        if (p_debug) {
            _libs.push_back(vformat("godot-dojo/bin/android/debug/%s-debug.aar", _plugin_name));
        } else {
            _libs.push_back(vformat("godot-dojo/bin/android/release/%s-release.aar", _plugin_name));
        }
        return _libs;
    };

    PackedStringArray
    _get_android_dependencies(const Ref<EditorExportPlatform> &p_platform, bool p_debug) const override {
        PackedStringArray _deps = {};
        _deps.push_back("androidx.browser:browser:1.8.0");
        _deps.push_back("androidx.webkit:webkit:1.10.0");
        return _deps;
    };

    String _get_android_manifest_activity_element_contents(const Ref<EditorExportPlatform> &p_platform,
                                                           bool p_debug) const override {
        return R"(
            <intent-filter>
                <action android:name="android.intent.action.VIEW" />
                <category android:name="android.intent.category.DEFAULT" />
                <category android:name="android.intent.category.BROWSABLE" />
                <data android:scheme="example" android:host="godot" />
            </intent-filter>)";
    };

    String _get_name() const override {
        return _plugin_name;
    };

protected:
    static void _bind_methods() {

    };
};
#endif //ANDROID_EXPORT_PLUGIN_H
