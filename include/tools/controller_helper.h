#pragma once

#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "controller/controller.hpp"
#include <vector>

using namespace godot;

class ControllerHelper : public Object {
    GDCLASS(ControllerHelper, Object)

public:
    ControllerHelper() = default;
    ~ControllerHelper() override = default;

    static bool has_storage(const String &app_id);
    static String get_controller_class_hash(int version);
    static String get_public_key(const String &private_key);
    static String signer_to_guid(const String &private_key);
    static bool validate_felt(const String &felt);
	static String generate_private_key();
	static std::vector<std::shared_ptr<controller::Call>>  prepare_calls(const TypedArray<Dictionary> &calls);
    static controller::SessionPolicies to_c_policies(const Dictionary &policies);
    static String create_session_registration_url(const String &private_key, const Dictionary &policies, const String &rpc_url, const String &preset = "");
    static String create_session_registration_url_with_urls(const String &private_key, const Dictionary &policies, const String &rpc_url, const String &keychain_url, const String &cartridge_api_url, const String &preset = "");



protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("has_storage", "app_id"), &ControllerHelper::has_storage);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("get_controller_class_hash", "version"), &ControllerHelper::get_controller_class_hash);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("get_public_key", "private_key"), &ControllerHelper::get_public_key);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("signer_to_guid", "private_key"), &ControllerHelper::signer_to_guid);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("validate_felt", "felt"), &ControllerHelper::validate_felt);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("generate_private_key"), &ControllerHelper::generate_private_key);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("create_session_registration_url", "private_key", "policies", "rpc_url", "preset"), &ControllerHelper::create_session_registration_url, DEFVAL(""));
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("create_session_registration_url_with_urls", "private_key", "policies", "rpc_url", "keychain_url", "cartridge_api_url", "preset"), &ControllerHelper::create_session_registration_url_with_urls, DEFVAL(""));

        ClassDB::bind_integer_constant(get_class_static(), "SignerType", "Webauthn", static_cast<int>(controller::SignerType::kWebauthn));
        ClassDB::bind_integer_constant(get_class_static(), "SignerType", "Starknet", static_cast<int>(controller::SignerType::kStarknet));

        // For Controller class_hash
        ClassDB::bind_integer_constant(get_class_static(), "Version", "V104", static_cast<int>(controller::Version::kV104));
        ClassDB::bind_integer_constant(get_class_static(), "Version", "V105", static_cast<int>(controller::Version::kV105));
        ClassDB::bind_integer_constant(get_class_static(), "Version", "V106", static_cast<int>(controller::Version::kV106));
        ClassDB::bind_integer_constant(get_class_static(), "Version", "V107", static_cast<int>(controller::Version::kV107));
        ClassDB::bind_integer_constant(get_class_static(), "Version", "V108", static_cast<int>(controller::Version::kV108));
        ClassDB::bind_integer_constant(get_class_static(), "Version", "V109", static_cast<int>(controller::Version::kV109));
        ClassDB::bind_integer_constant(get_class_static(), "Version", "Latest", static_cast<int>(controller::Version::kLatest));

    }

};
