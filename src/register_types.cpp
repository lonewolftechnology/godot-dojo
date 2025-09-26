//
// Created by hazel on 12/05/25.
//
#include "register_types.h"

#include <godot_cpp/classes/editor_plugin.hpp>

#include "dojo_c_gdextension.h"

#include "gdextension_interface.h"
#include "editor/dojo_editor_plugin.h"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/godot.hpp"
#ifdef WEB_ENABLED
#include <tools/dojo_bridge.h>
#endif

#ifdef TOOLS_ENABLED
#include "editor/dojo_editor_plugin.h"
#endif


#include "classes/torii_client.h"
#include "classes/controller_account.h"
#include "classes/account.h"
#include "tools/dojo_helper.h"

#include "variant/ty/dojo_array.h"
#include "variant/ty/enum.h"
#include "variant/ty/struct.h"
#include "variant/ty/ty.h"
#include "variant/ty/primitive.h"
#include "variant/field_element.h"
#include "types/big_int.h"

#include "resources/dojo_policies.h"
#include "resources/dojo_policy.h"
#include "resources/dojo_query_base.h"
#include "resources/dojo_subscription.h"

#include "resources/queries/dojo_query.h"
#include "resources/queries/dojo_token_query.h"
#include "resources/queries/dojo_controller_query.h"
#include "resources/queries/dojo_transaction_query.h"
#include "resources/queries/dojo_token_balance_query.h"
#include "resources/queries/dojo_transaction_filter.h"
#include "resources/queries/dojo_contract_query.h"

#include "resources/subscriptions/entity.h"
#include "resources/subscriptions/event.h"
#include "resources/subscriptions/message.h"
#include "resources/subscriptions/starknet.h"
#include "resources/subscriptions/token.h"
#include "resources/subscriptions/token_balance.h"
#include "resources/subscriptions/transaction.h"
#include "resources/subscriptions/contract.h"

#include "ref_counted/dojo_option.h"
#include "ref_counted/options/option_u32.h"
#include "ref_counted/options/option_u64.h"
#include "ref_counted/options/option_char.h"
#include "ref_counted/options/option_u256.h"
#include "ref_counted/options/option_clause.h"
#include "ref_counted/options/option_field_element.h"
#include "ref_counted/options/option_transaction_filter.h"
#include "ref_counted/options/option_array_field_element.h"

using namespace godot;

void initialize_dojoc_module(ModuleInitializationLevel p_level)
{
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        // Tools
#ifdef WEB_ENABLED
        GDREGISTER_CLASS(DojoBridge);
#endif

        GDREGISTER_CLASS(DojoHelpers);
        // DojoTypes
        GDREGISTER_CLASS(DojoTy);
        GDREGISTER_CLASS(DojoPrimitive);
        GDREGISTER_CLASS(DojoStruct);
        GDREGISTER_CLASS(DojoEnum);
        GDREGISTER_CLASS(DojoArray);
        GDREGISTER_CLASS(FieldElement);
        GDREGISTER_CLASS(U128);
        GDREGISTER_CLASS(I128);
        GDREGISTER_CLASS(U256);
        // Classes
        GDREGISTER_CLASS(ToriiClient);
        GDREGISTER_CLASS(ControllerAccount);
        GDREGISTER_CLASS(Account);
        // RefCounted
        GDREGISTER_CLASS(DojoOption);
        GDREGISTER_CLASS(OptionU32);
        GDREGISTER_CLASS(OptionU64);
        GDREGISTER_CLASS(OptionChar);
        GDREGISTER_CLASS(OptionU256);
        GDREGISTER_CLASS(OptionFieldElement);
        GDREGISTER_CLASS(OptionClause);
        GDREGISTER_CLASS(OptionTransactionFilter);
        GDREGISTER_CLASS(OptionArrayFieldElement);
        // Resources
        GDREGISTER_CLASS(DojoPolicy);
        GDREGISTER_CLASS(DojoPolicies);
        GDREGISTER_CLASS(DojoQueryBase);
        GDREGISTER_CLASS(DojoQuery);
        GDREGISTER_CLASS(DojoTokenQuery);
        GDREGISTER_CLASS(DojoControllerQuery);
        GDREGISTER_CLASS(DojoTransactionQuery);
        GDREGISTER_CLASS(DojoTokenBalanceQuery);
        GDREGISTER_CLASS(DojoContractQuery)
        GDREGISTER_CLASS(DojoSubscription);
        GDREGISTER_CLASS(DojoTransactionFilter);
        GDREGISTER_CLASS(EventSubscription);
        GDREGISTER_CLASS(MessageSubscription);
        GDREGISTER_CLASS(TokenSubscription);
        GDREGISTER_CLASS(TokenBalanceSubscription);
        GDREGISTER_CLASS(ContractSubscription);
        GDREGISTER_CLASS(EntitySubscription);
        GDREGISTER_CLASS(TransactionSubscription);
        GDREGISTER_CLASS(StarknetSubscription);
        GDREGISTER_CLASS(DojoC);
    }

    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
    {
#ifdef TOOLS_ENABLED
        GDREGISTER_CLASS(DojoEditorPlugin);
#endif

    }
}

void uninitialize_dojoc_module(ModuleInitializationLevel p_level)
{
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT dojoc_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                              const GDExtensionClassLibraryPtr p_library,
                                              GDExtensionInitialization* r_initialization)
{
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_dojoc_module);
    init_obj.register_terminator(uninitialize_dojoc_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

    return init_obj.init();
}
}
