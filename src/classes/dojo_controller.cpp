#include "classes/dojo_controller.h"

void DojoController::set_internal(std::shared_ptr<controller::Controller> p_internal) {
    internal = p_internal;
}

DojoController *DojoController::init(const String &app_id, const String &username, const String &class_hash,
                                     const String &rpc_url, const Ref<DojoOwner> &owner, const String &address,
                                     const String &chain_id) {
    DojoController *dojo_controller = memnew(DojoController);

    try {
        auto internal_controller = controller::Controller::init(
            app_id.utf8().get_data(),
            username.utf8().get_data(),
            class_hash.utf8().get_data(),
            rpc_url.utf8().get_data(),
            owner->get_internal(),
            address.utf8().get_data(),
            chain_id.utf8().get_data()
        );
        dojo_controller->set_internal(internal_controller);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.init failed:", e.what());
        return nullptr;
    }

    return dojo_controller;
}

DojoController *DojoController::from_storage(const String &app_id) {
    DojoController *dojo_controller = memnew(DojoController);

    try {
        auto internal_controller = controller::Controller::from_storage(app_id.utf8().get_data());
        dojo_controller->set_internal(internal_controller);
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.from_storage failed:", e.what());
        return nullptr;
    }

    return dojo_controller;
}

String DojoController::address() {
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }
    try {
        return String(internal->address().c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.address failed:", e.what());
        return "";
    }
}

String DojoController::execute(const TypedArray<Dictionary> &calls) {
    if (!internal) {
        Logger::error("DojoController is not initialized.");
        return "";
    }

    std::vector<std::shared_ptr<controller::Call>> c_calls;
    for (int i = 0; i < calls.size(); ++i) {
        Dictionary call_dict = calls[i];
        auto c = std::make_shared<controller::Call>();
        c->contract_address = call_dict["contract_address"].operator String().utf8().get_data();
        c->entrypoint = call_dict["entrypoint"].operator String().utf8().get_data();

        Array calldata_array = call_dict["calldata"];
        for (int j = 0; j < calldata_array.size(); ++j) {
            c->calldata.push_back(calldata_array[j].operator String().utf8().get_data());
        }
        c_calls.push_back(c);
    }

    try {
        return String(internal->execute(c_calls).c_str());
    } catch (const controller::ControllerError &e) {
        Logger::error("DojoController.execute failed:", e.what());
        return "";
    }
}
