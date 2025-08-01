//
// Created by hazel on 12/05/25.
//
#include "register_types.h"

#include "dojo_c_gdextension.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "classes/torii_client.h"
#include "classes/event_subscription.h"
#include "classes/controller_account.h"
#include "tools/dojo_helper.h"

#include "variant/ty/dojo_array.h"
#include "variant/ty/enum.h"
#include "variant/ty/struct.h"
#include "variant/ty/ty.h"
#include "variant/ty/primitive.h"
#include "variant/field_element.h"

#include "resources/dojo_policies.h"
#include "resources/dojo_policy.h"
#include "resources/dojo_call.h"
#include "resources/dojo_query.h"

using namespace godot;

void initialize_dojoc_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
    GDREGISTER_CLASS(DojoHelpers);
    // DojoTypes
    GDREGISTER_CLASS(DojoTy);
    GDREGISTER_CLASS(DojoPrimitive);
    GDREGISTER_CLASS(DojoStruct);
    GDREGISTER_CLASS(DojoEnum);
    GDREGISTER_CLASS(DojoArray);
    GDREGISTER_CLASS(FieldElement);
    // Dojo Classes
    GDREGISTER_CLASS(DojoC);
    GDREGISTER_CLASS(ToriiClient);
    GDREGISTER_CLASS(ControllerAccount);
    GDREGISTER_CLASS(EventSubscription);
    // Dojo Resources
    GDREGISTER_CLASS(DojoCall);
    GDREGISTER_CLASS(DojoPolicy);
    GDREGISTER_CLASS(DojoPolicies);
    GDREGISTER_CLASS(DojoQuery);

}

void uninitialize_dojoc_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT dojoc_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                              const GDExtensionClassLibraryPtr p_library,
                                              GDExtensionInitialization* r_initialization)
{
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_dojoc_module);
    init_obj.register_terminator(uninitialize_dojoc_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}
