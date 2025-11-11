
#ifndef CONTROLLER_TYPES_H
#define CONTROLLER_TYPES_H

namespace Controller {
    extern "C" {
        // Core Diplomat runtime
        #include "diplomat_runtime.h"

        // Diplomat-generated C bindings for controller.c
        #include "CONTROLLERS.h"
        #include "Controller.h"
        #include "ControllerError.h"
        #include "ContractPolicy.h"
        #include "DiplomatCall.h"
        #include "DiplomatCallList.h"
        #include "DiplomatFelt.h"
        #include "DiplomatOwner.h"
        #include "DiplomatSessionPolicies.h"
        #include "DiplomatSigner.h"
        #include "Method.h"
        #include "SessionAccount.h"
        #include "SignMessagePolicy.h"
        #include "SignerType.h"
        #include "Utils.h"
    }

    // Helper macros for handling Diplomat results
    #define RESULT_IS_OK(result) ((result).is_ok)
    #define RESULT_GET_OK(result) ((result).ok)
    #define RESULT_GET_ERR(result) ((result).err)
}

#endif // CONTROLLER_TYPES_H