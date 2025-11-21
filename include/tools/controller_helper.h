#ifndef CONTROLLER_HELPER_H
#define CONTROLLER_HELPER_H

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
    enum SignerType {
        Webauthn = 1,
        Starknet = 2
    };


    enum Version {
        V104 = 1,
        V105 = 2,
        V106 = 3,
        V107 = 4,
        V108 = 5,
        V109 = 6,
        Latest = 7
    };

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



protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("has_storage", "app_id"), &ControllerHelper::has_storage);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("get_controller_class_hash", "version"), &ControllerHelper::get_controller_class_hash);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("get_public_key", "private_key"), &ControllerHelper::get_public_key);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("signer_to_guid", "private_key"), &ControllerHelper::signer_to_guid);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("validate_felt", "felt"), &ControllerHelper::validate_felt);
        ClassDB::bind_static_method("ControllerHelper", D_METHOD("generate_private_key"), &ControllerHelper::generate_private_key);

        // SignerType
        BIND_ENUM_CONSTANT(SignerType::Webauthn);
        BIND_ENUM_CONSTANT(SignerType::Starknet);

        // Version
        BIND_ENUM_CONSTANT(Version::V104);
        BIND_ENUM_CONSTANT(Version::V105);
        BIND_ENUM_CONSTANT(Version::V106);
        BIND_ENUM_CONSTANT(Version::V107);
        BIND_ENUM_CONSTANT(Version::V108);
        BIND_ENUM_CONSTANT(Version::V109);
        BIND_ENUM_CONSTANT(Version::Latest);

    }

};
VARIANT_ENUM_CAST(ControllerHelper::SignerType);
VARIANT_ENUM_CAST(ControllerHelper::Version);


#endif // CONTROLLER_HELPER_H
