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
        Array output_message;


        static DojoC *get_singleton();

        void create_client(const String& world_addr, String torii_url, String relay_url);
        void controller_new(const String& controller_addr, const String& rpc_url);
        // void controller_connect(const String &controller_addr);
        void testing();
        void create_entity_subscription(Callable callback);
        void spawn(bool _debug);
        void move(Ref<FieldElement> ref_felt, bool _debug);
        // Array values = {};
        // Array get_values() const {return values;}
        // void set_values(const Array &p_values) {values = p_values;}

        Array get_output_message() const {return output_message;}
        void set_output_message(const Array p_output_message) {output_message = p_output_message;}

        DojoC();
        ~DojoC();

        //void _process(double delta) override;
    };

}

#endif //DOJO_C_GDEXTENSION_H
