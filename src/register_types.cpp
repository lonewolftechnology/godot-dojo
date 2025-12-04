//
// Created by hazel on 12/05/25.
//
#include "register_types.h"

#include <godot_cpp/classes/editor_plugin.hpp>

#include "gdextension_interface.h"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/godot.hpp"

#include "ref_counted/query_builder.hpp"
#include "ref_counted/dojo_utilities/queries/pagination.hpp"
#include "ref_counted/dojo_utilities/queries/order_by.hpp"

#include "ref_counted/queries/achievement.hpp"
#include "ref_counted/queries/activity.hpp"
#include "ref_counted/queries/aggregation.hpp"
#include "ref_counted/queries/contract.hpp"
#include "ref_counted/queries/controller.hpp"
#include "ref_counted/queries/entity.hpp"
#include "ref_counted/queries/player_achievement.hpp"
#include "ref_counted/queries/search.hpp"
#include "ref_counted/queries/token.hpp"
#include "ref_counted/queries/token_balance.hpp"
#include "ref_counted/queries/token_transfer.hpp"
#include "ref_counted/queries/transaction.hpp"

#ifdef WEB_ENABLED
#include <tools/dojo_bridge.h>
#endif

#ifdef TOOLS_ENABLED
#include "editor/dojo_editor_plugin.h"
#endif

#include "tools/dojo_helper.h"
#include "tools/controller_helper.h"

#include "classes/torii_client.h"
#include "classes/account.h"
#include "classes/dojo_owner.h"
#include "classes/dojo_controller.h"
#include "classes/dojo_session_account.h"

#include "variant/ty/dojo_array.h"
#include "variant/ty/enum.h"
#include "variant/ty/struct.h"
#include "variant/ty/ty.h"
#include "variant/ty/primitive.h"
#include "variant/field_element.h"
#include "types/big_int.h"

#include "resources/dojo_query_base.h"
#include "resources/dojo_subscription.h"

#include "resources/queries/dojo_query.h"
#include "resources/queries/dojo_token_query.h"
#include "resources/queries/dojo_controller_query.h"
#include "resources/queries/dojo_transaction_query.h"
#include "resources/queries/dojo_token_balance_query.h"
#include "resources/queries/dojo_token_transfer_query.h"
#include "resources/queries/dojo_transaction_filter.h"
#include "resources/queries/dojo_contract_query.h"

#include "resources/subscriptions/entity.h"
#include "resources/subscriptions/event.h"
#include "resources/subscriptions/message.h"
#include "resources/subscriptions/starknet.h"
#include "resources/subscriptions/token.h"
#include "resources/subscriptions/token_balance.h"
#include "resources/subscriptions/token_transfer.h"
#include "resources/subscriptions/transaction.h"
#include "resources/subscriptions/contract.h"

#include "ref_counted/dojo_option.h"
#include "ref_counted/options/option_u32.h"
#include "ref_counted/options/option_u64.h"
#include "ref_counted/options/option_f64.h"
#include "ref_counted/options/option_char.h"
#include "ref_counted/options/option_u256.h"
#include "ref_counted/options/option_clause.h"
#include "ref_counted/options/option_field_element.h"
#include "ref_counted/options/option_transaction_filter.h"
#include "ref_counted/options/option_array_field_element.h"

using namespace godot;

void initialize_godotdojo_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        // Tools
#ifdef WEB_ENABLED
        GDREGISTER_CLASS(DojoBridge);
#endif
        // Tools
        GDREGISTER_CLASS(DojoHelpers);
        GDREGISTER_CLASS(ControllerHelper);
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
        // Classes //
        GDREGISTER_CLASS(ToriiClient);
        GDREGISTER_CLASS(Account);
        GDREGISTER_CLASS(DojoSessionAccount);
        GDREGISTER_CLASS(DojoOwner);
        GDREGISTER_CLASS(DojoController);
        // RefCounted //
        GDREGISTER_CLASS(DojoOption);
        GDREGISTER_CLASS(DojoOptionU32);
        GDREGISTER_CLASS(DojoOptionU64);
        GDREGISTER_CLASS(DojoOptionf64)
        GDREGISTER_CLASS(DojoOptionChar);
        GDREGISTER_CLASS(DojoOptionU256);
        GDREGISTER_CLASS(DojoOptionFieldElement);
        GDREGISTER_CLASS(DojoOptionClause);
        GDREGISTER_CLASS(DojoOptionTransactionFilter);
        GDREGISTER_CLASS(DojoOptionArrayFieldElement);

        // -Experimental- //
        // Builders
        GDREGISTER_CLASS(QueryBuilder)
        GDREGISTER_CLASS(DojoPagination)
        GDREGISTER_CLASS(DojoOrderBy)

        // Queries
        GDREGISTER_CLASS(AchievementQuery)
        GDREGISTER_CLASS(ActivityQuery)
        GDREGISTER_CLASS(AggregationQuery)
        GDREGISTER_CLASS(ContractQuery)
        GDREGISTER_CLASS(ControllerQuery)
        GDREGISTER_CLASS(EntityQuery)
        GDREGISTER_CLASS(PlayerAchievementQuery)
        GDREGISTER_CLASS(SearchQuery)
        GDREGISTER_CLASS(TokenQuery)
        GDREGISTER_CLASS(TokenBalanceQuery)
        GDREGISTER_CLASS(TokenTransferQuery)
        GDREGISTER_CLASS(TransactionQuery)
        // -Experimental- //

        // Resources //
        GDREGISTER_CLASS(DojoTransactionFilter);

        // Queries (DEPRECATED)
        GDREGISTER_CLASS(DojoQueryBase);
        GDREGISTER_CLASS(DojoAchievementQuery)
        GDREGISTER_CLASS(DojoActivityQuery)
        GDREGISTER_CLASS(DojoAggregationQuery)
        GDREGISTER_CLASS(DojoContractQuery)
        GDREGISTER_CLASS(DojoControllerQuery);
        GDREGISTER_CLASS(DojoQuery);
        GDREGISTER_CLASS(DojoPlayerAchievementQuery);
        GDREGISTER_CLASS(DojoSearchQuery);
        GDREGISTER_CLASS(DojoTokenQuery);
        GDREGISTER_CLASS(DojoTokenBalanceQuery);
        GDREGISTER_CLASS(DojoTokenTransferQuery);
        GDREGISTER_CLASS(DojoTransactionQuery);

        // Subscriptions
        GDREGISTER_CLASS(DojoSubscription);
        GDREGISTER_CLASS(EventSubscription);
        GDREGISTER_CLASS(MessageSubscription);
        GDREGISTER_CLASS(TokenSubscription);
        GDREGISTER_CLASS(TokenBalanceSubscription);
        GDREGISTER_CLASS(TokenTransferSubscription);
        GDREGISTER_CLASS(ContractSubscription);
        GDREGISTER_CLASS(EntitySubscription);
        GDREGISTER_CLASS(TransactionSubscription);
        GDREGISTER_CLASS(StarknetSubscription);
        GDREGISTER_CLASS(ActivitySubscription);
        GDREGISTER_CLASS(AggregationSubscription);
        GDREGISTER_CLASS(AchievementProgressionSubscription);
    }

    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
#ifdef TOOLS_ENABLED
        GDREGISTER_CLASS(DojoEditorPlugin);
        EditorPlugins::add_by_type<DojoEditorPlugin>();
#endif
    }
}

void uninitialize_godotdojo_module(ModuleInitializationLevel p_level) {
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT godotdojo_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                              GDExtensionClassLibraryPtr p_library,
                                              GDExtensionInitialization *r_initialization) {
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_godotdojo_module);
    init_obj.register_terminator(uninitialize_godotdojo_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_EDITOR);

    return init_obj.init();
}
}
