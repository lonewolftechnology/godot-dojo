//
// Created by hazel on 12/05/25.
//

#ifndef DOJO_C_GDEXTENSION_H
#define DOJO_C_GDEXTENSION_H

#include <godot_cpp/classes/editor_plugin.hpp>

#include "godot_cpp/classes/object.hpp"

using namespace godot;

class DojoC : public Object
{
    GDCLASS(DojoC, Object);
    EditorPlugin* plugin;

public:
    void set_enabled(bool p_enabled) { enabled = p_enabled; };
    bool get_enabled() const { return enabled; }

    static DojoC* get_singleton() { return singleton; }


    DojoC();
    ~DojoC();
    static void init_config(bool reset = false);
    static void set_setting(const String& setting, const Variant& value, const bool& force = false);

    //void _process(double delta) override;
protected:
    static DojoC* singleton;
    bool enabled;

    static void _bind_methods()
    {
        ClassDB::bind_method(D_METHOD("set_enabled", "p_enabled"), &DojoC::set_enabled);
        ClassDB::bind_method(D_METHOD("get_enabled"), &DojoC::get_enabled);

        ClassDB::bind_static_method("DojoC", D_METHOD("init_config", "reset"), &DojoC::init_config);
        ClassDB::bind_static_method("DojoC", D_METHOD("init_setting", "setting", "value", "force"),
                                    &DojoC::set_setting);
        // ClassDB::bind_method(D_METHOD("controller_connect"), &DojoC::controller_connect);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_enabled", "get_enabled");
    }
};

#endif //DOJO_C_GDEXTENSION_H
