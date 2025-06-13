//
// Created by hazel on 12/05/25.
//

#ifndef DOJO_C_GDEXTENSION_H
#define DOJO_C_GDEXTENSION_H

#include <godot_cpp/classes/object.hpp>
#include "dojo_types.h"
#include "variant/field_element.h"
#include "classes/event_subscription.h"

namespace godot {

    class DojoC : public Object {
        GDCLASS(DojoC, Object);

    protected:
        static void _bind_methods();
		static DojoC *singleton;
        bool enabled;

    public:
        void set_enabled(bool p_enabled){enabled=p_enabled;};
        bool get_enabled(){return enabled;}

        static DojoC *get_singleton();


        DojoC();
        ~DojoC();

        //void _process(double delta) override;
    };

}

#endif //DOJO_C_GDEXTENSION_H
