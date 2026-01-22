//
// Created by hazel on 12/05/25.
//
#include "register_types.h"

#include <godot_cpp/classes/editor_plugin.hpp>

#include "gdextension_interface.h"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/godot.hpp"

// New implementation
#include "node/torii_client.hpp"
#include "node/dojo_controller.hpp"
#include "node/dojo_session_account.hpp"
#include "ref_counted/controller_utilities/dojo_owner.hpp"

#include "ref_counted/query_builder.hpp"
#include "ref_counted/dojo_utilities/callback.hpp"
#include "ref_counted/dojo_utilities/clause.hpp"
#include "ref_counted/dojo_utilities/clauses/keys.hpp"
#include "ref_counted/dojo_utilities/clauses/member.hpp"
#include "ref_counted/dojo_utilities/clauses/composite.hpp"
#include "ref_counted/dojo_utilities/callback_utils.hpp"
#include "tools/godot_helper.hpp"
#include "tools/controller_helper.h"
#include "tools/http_pinger.hpp"

#include "ref_counted/queries/achievement.hpp"
#include "ref_counted/queries/activity.hpp"
#include "ref_counted/queries/aggregation.hpp"
#include "ref_counted/queries/contract.hpp"
#include "ref_counted/queries/controller.hpp"
#include "ref_counted/queries/entity.hpp"
#include "ref_counted/queries/event.hpp"
#include "ref_counted/queries/player_achievement.hpp"
#include "ref_counted/queries/search.hpp"
#include "ref_counted/queries/token.hpp"
#include "ref_counted/queries/token_balance.hpp"
#include "ref_counted/queries/token_contract.hpp"
#include "ref_counted/queries/token_transfer.hpp"
#include "ref_counted/queries/transaction.hpp"

using namespace godot;

void initialize_godotdojo_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        // Main Client
        GDREGISTER_CLASS(ToriiClient);

        // Controller
        GDREGISTER_CLASS(DojoController);
        GDREGISTER_CLASS(DojoSessionAccount);
        GDREGISTER_CLASS(DojoOwner);

        // Utilities
        GDREGISTER_CLASS(DojoCallback);
        GDREGISTER_CLASS(CallbackUtils);
        GDREGISTER_CLASS(GodotHelper);
        GDREGISTER_CLASS(ControllerHelper);
        GDREGISTER_CLASS(HttpPinger);

        // Clauses
        GDREGISTER_CLASS(DojoClause);
        GDREGISTER_CLASS(KeysClause);
        GDREGISTER_CLASS(MemberClause);
        GDREGISTER_CLASS(CompositeClause);

        // Builders
        GDREGISTER_CLASS(QueryBuilder);

        // Queries
        GDREGISTER_CLASS(AchievementQuery);
        GDREGISTER_CLASS(ActivityQuery);
        GDREGISTER_CLASS(AggregationQuery);
        GDREGISTER_CLASS(ContractQuery);
        GDREGISTER_CLASS(ControllerQuery);
        GDREGISTER_CLASS(DojoQuery); // Formerly EntityQuery
        GDREGISTER_CLASS(EventQuery);
        GDREGISTER_CLASS(PlayerAchievementQuery);
        GDREGISTER_CLASS(SearchQuery);
        GDREGISTER_CLASS(TokenQuery);
        GDREGISTER_CLASS(TokenBalanceQuery);
        GDREGISTER_CLASS(TokenContractQuery);
        GDREGISTER_CLASS(TokenTransferQuery);
        GDREGISTER_CLASS(TransactionQuery);
    }

    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        // Editor plugins if any
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
