//
// Created by hazel on 12/05/25.
//

#ifndef DOJO_C_GDEXTENSION_H
#define DOJO_C_GDEXTENSION_H

#include "godot_cpp/classes/object.hpp"

using namespace godot;

class DojoC : public Object
{
    GDCLASS(DojoC, Object);

protected:
    static void _bind_methods();
    static DojoC* singleton;
    bool enabled;

public:
    void set_enabled(bool p_enabled) { enabled = p_enabled; };
    bool get_enabled() const { return enabled; }

    static DojoC* get_singleton() { return singleton; }


    DojoC();
    ~DojoC();
    static void init_config(bool reset = false);
    static bool set_setting(const String& setting, const Variant& value, const bool& force = false);

    //void _process(double delta) override;
};

#endif //DOJO_C_GDEXTENSION_H
