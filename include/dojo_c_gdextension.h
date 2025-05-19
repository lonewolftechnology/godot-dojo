//
// Created by hazel on 12/05/25.
//

#ifndef DOJO_C_GDEXTENSION_H
#define DOJO_C_GDEXTENSION_H

#include <godot_cpp/classes/object.hpp>

#include "dojo.h"

namespace godot {

    class DojoC : public Object {
        GDCLASS(DojoC, Object);

    private:
        dojo_bindings::ToriiClient* client;

    protected:
        static void _bind_methods();
		static DojoC *singleton;
		bool enabled;

    public:
        void set_enabled(bool p_enabled){enabled=p_enabled;};
        bool get_enabled(){return enabled;}


        static DojoC *get_singleton();

        String create_client(const String& world_addr);
        void controller_connect(const String &controller_addr);
        void testing();

        DojoC();
        ~DojoC();

        //void _process(double delta) override;
    };

}

#endif //DOJO_C_GDEXTENSION_H
