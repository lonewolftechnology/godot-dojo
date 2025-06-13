//
// Created by hazel on 12/05/25.
//
#include "register_types.h"

#include <dojo_c_gdextension.h>

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include <variant/primitive.h>
#include <classes/torii_client.h>

#include <variant/field_element.h>

#include <classes/event_subscription.h>

#include "classes/controller_account.h"


using namespace godot;

void initialize_dojoc_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    GDREGISTER_CLASS(FieldElement);
    GDREGISTER_CLASS(DojoPrimitive);
    GDREGISTER_CLASS(ToriiClient);
    GDREGISTER_CLASS(EventSubscription);
    GDREGISTER_CLASS(ControllerAccount);
    GDREGISTER_CLASS(DojoC);
}

void uninitialize_dojoc_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
    // Initialization.
    GDExtensionBool GDE_EXPORT dojoc_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_dojoc_module);
        init_obj.register_terminator(uninitialize_dojoc_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}