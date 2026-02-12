#include "tools/controller_helper.h"
#include "tools/godot_helper.hpp"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/os.hpp"
#include <iomanip>
#include <sstream>
#include <random>
#include <deque>
#include "tools/logger.hpp"
#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::cpp_int;

bool ControllerHelper::has_storage(const String& app_id)
{
    return controller::controller_has_storage(app_id.utf8().get_data());
}

String ControllerHelper::get_controller_class_hash(int version)
{
    return {controller::get_controller_class_hash(static_cast<controller::Version>(version)).c_str()};
}

String ControllerHelper::get_public_key(const String& private_key)
{
    return {controller::get_public_key(private_key.utf8().get_data()).c_str()};
}

String ControllerHelper::signer_to_guid(const String& private_key)
{
    return {controller::signer_to_guid(private_key.utf8().get_data()).c_str()};
}

bool ControllerHelper::validate_felt(const String& felt)
{
    return controller::validate_felt(felt.utf8().get_data());
}

String ControllerHelper::generate_private_key()
{
    while (true) {
        // Generate 31 bytes of random data to ensure it fits in felt (252 bits)
        // 31 bytes = 248 bits, which is safe.
        PackedByteArray bytes = OS::get_singleton()->get_entropy(31);

        // Convert to hex string
        String hex = "0x";
        for (int i = 0; i < bytes.size(); i++) {
            hex += String::num_uint64(bytes[i], 16).lpad(2, "0");
        }

        // Validate the key to ensure it doesn't cause a panic in the controller library
        try {
            controller::get_public_key(hex.utf8().get_data());
            return hex;
        } catch (const std::exception &e) {
            Logger::debug_extra("Private Key", "Invalid key: ", e.what());
            continue;
        }
    }
}

std::vector<std::shared_ptr<controller::Call>> ControllerHelper::prepare_calls(const TypedArray<Dictionary>& calls)
{
    std::vector<std::shared_ptr<controller::Call>> prepared_calls;

    for (int i = 0; i < calls.size(); ++i)
    {
        Dictionary call_dict = calls[i];
        if (!call_dict.has("contract_address") || !call_dict.has("entrypoint"))
        {
            Logger::info("Skipping a call due to missing 'contract_address' or 'entrypoint'.");
            continue;
        }

        auto current_call = std::make_shared<controller::Call>();
        current_call->contract_address = static_cast<String>(call_dict["contract_address"]).utf8().get_data();
        current_call->entrypoint = static_cast<String>(call_dict["entrypoint"]).utf8().get_data();

        if (!call_dict.has("calldata") || call_dict["calldata"].get_type() != Variant::ARRAY)
        {
            prepared_calls.push_back(current_call);
            continue;
        }
        Array args = call_dict["calldata"];
        std::deque<Variant> worklist;
        for (int j = 0; j < args.size(); ++j)
        {
            worklist.push_back(args[j]);
        }

        while (!worklist.empty())
        {
            Variant arg = worklist.front();
            worklist.pop_front();

            switch (arg.get_type())
            {
            case Variant::Type::ARRAY:
                {
                    Array inner_array = arg;
                    for (int64_t k = inner_array.size() - 1; k >= 0; --k)
                    {
                        worklist.emplace_front(inner_array[k]);
                    }
                    continue;
                }
            case Variant::Type::FLOAT:
                {
                    worklist.emplace_front(
                        GodotDojoHelper::double_to_variant_fp(
                            arg, GodotDojoHelper::get_dojo_setting("fixed_point/default")));
                    continue;
                }
            case Variant::Type::VECTOR2:
                {
                    Vector2 vec = arg;
                    worklist.emplace_front(vec.y);
                    worklist.emplace_front(vec.x);
                    continue;
                }
            case Variant::Type::VECTOR2I:
                {
                    Vector2i vec = arg;
                    worklist.emplace_front(static_cast<int64_t>(vec.y));
                    worklist.emplace_front(static_cast<int64_t>(vec.x));
                    continue;
                }
            case Variant::Type::VECTOR3:
                {
                    Vector3 vec = arg;
                    worklist.emplace_front(vec.z);
                    worklist.emplace_front(vec.y);
                    worklist.emplace_front(vec.x);
                    continue;
                }
            case Variant::Type::VECTOR3I:
                {
                    Vector3i vec = arg;
                    worklist.emplace_front(static_cast<int64_t>(vec.z));
                    worklist.emplace_front(static_cast<int64_t>(vec.y));
                    worklist.emplace_front(static_cast<int64_t>(vec.x));
                    continue;
                }
            case Variant::Type::VECTOR4:
                {
                    Vector4 vec = arg;
                    worklist.emplace_front(vec.w);
                    worklist.emplace_front(vec.z);
                    worklist.emplace_front(vec.y);
                    worklist.emplace_front(vec.x);
                    continue;
                }
            case Variant::Type::VECTOR4I:
                {
                    Vector4i vec = arg;
                    worklist.emplace_front(static_cast<int64_t>(vec.w));
                    worklist.emplace_front(static_cast<int64_t>(vec.z));
                    worklist.emplace_front(static_cast<int64_t>(vec.y));
                    worklist.emplace_front(static_cast<int64_t>(vec.x));
                    continue;
                }
            case Variant::Type::BOOL:
                current_call->calldata.emplace_back(static_cast<bool>(arg) ? "0x1" : "0x0");
                break;
            case Variant::Type::INT:
                {
                    int64_t int_val = arg;
                    cpp_int val = int_val;
                    if (int_val < 0)
                    {
                        val = GodotDojoHelper::to_starknet_negative_felt(val);
                    }
                    std::stringstream ss;
                    ss << "0x" << std::hex << val;
                    current_call->calldata.emplace_back(ss.str());
                    break;
                }
            case Variant::Type::PACKED_BYTE_ARRAY:
                {
                    PackedByteArray arr = arg;
                    String hex_str = "0x";
                    for (int k = 0; k < arr.size(); ++k)
                    {
                        hex_str += String::num_uint64(arr[k], 16).lpad(2, "0");
                    }
                    current_call->calldata.emplace_back(hex_str.utf8().get_data());
                    break;
                }
            case Variant::Type::STRING:
                current_call->calldata.emplace_back(static_cast<String>(arg).utf8().get_data());
                break;
            case Variant::Type::OBJECT:
                {
                    Ref<RefCounted> obj = arg;
                    if (obj.is_valid() && obj->has_method("to_calldata"))
                    {
                        Variant ret = obj->call("to_calldata");
                        if (ret.get_type() == Variant::PACKED_STRING_ARRAY) {
                            PackedStringArray arr = ret;
                            for (const auto & k : arr) {
                                current_call->calldata.emplace_back(k.utf8().get_data());
                            }
                        } else {
                            current_call->calldata.emplace_back(String(ret).utf8().get_data());
                        }
                        continue;
                    }
                    if (obj.is_valid() && obj->has_method("to_string"))
                    {
                        String val = obj->call("to_string");
                        current_call->calldata.emplace_back(val.utf8().get_data());
                        continue;
                    }

                    Logger::warning("prepare_calldata: Unsupported object type in calldata: " + obj->get_class());
                    break;
                }
            default:
                Logger::warning(
                    "prepare_calldata: Unsupported variant type in calldata: " + Variant::get_type_name(
                        arg.get_type()));
                break;
            }
        }
        prepared_calls.push_back(current_call);
    }

    return prepared_calls;
}

controller::SessionPolicies ControllerHelper::to_c_policies(const Dictionary& policies)
{
    controller::SessionPolicies c_policies;
    c_policies.max_fee = "0x0";

    if (policies.is_empty())
    {
        Logger::warning("Policies dictionary is empty.");
        return c_policies;
    }

    if (policies.has("max_fee") && policies["max_fee"].get_type() == Variant::STRING)
    {
        c_policies.max_fee = policies["max_fee"].operator String().utf8().get_data();
        Logger::debug_extra("DojoSessionAccount", "max_fee", c_policies.max_fee.c_str());
    }
    else
    {
        Logger::warning("`max_fee` not found or not a String in policies. Using default.");
    }

    if (!policies.has("policies") || policies["policies"].get_type() != Variant::ARRAY)
    {
        Logger::error("`policies` key not found or is not an Array.");
        return c_policies;
    }

    Array policy_array = policies["policies"];

    for (int i = 0; i < policy_array.size(); ++i)
    {
        Dictionary policy_group = policy_array[i];

        if (policy_group.has("target") && policy_group.has("method"))
        {
            auto p = std::make_shared<controller::SessionPolicy>();
            p->contract_address = String(policy_group["target"]).utf8().get_data();
            p->entrypoint = String(policy_group["method"]).utf8().get_data();
            Logger::debug_extra("Policy", "Target", p->contract_address.c_str());
            Logger::debug_extra("Policy", "Method", p->entrypoint.c_str());
            c_policies.policies.push_back(p);
            continue;
        }

        if (!policy_group.has("contract_address")) continue;

        String contract_address = policy_group["contract_address"];
        Array methods;

        if (policy_group.has("methods")) {
            methods = policy_group["methods"];
        } else if (policy_group.has("entrypoints")) {
            methods = policy_group["entrypoints"];
        } else {
            continue;
        }

        Logger::debug_extra("Policy", "Contract", contract_address);
        for (int j = 0; j < methods.size(); ++j)
        {
            auto p = std::make_shared<controller::SessionPolicy>();
            p->contract_address = contract_address.utf8().get_data();
            p->entrypoint = String(methods[j]).utf8().get_data();
            Logger::debug_extra("Policy", "Entrypoint", p->entrypoint.c_str());
            c_policies.policies.push_back(p);
        }
    }
    return c_policies;
}

String ControllerHelper::create_session_registration_url(const String &private_key, const Dictionary &policies, const String &rpc_url, const String &preset)
{
    controller::SessionPolicies c_policies = to_c_policies(policies);
    std::optional<std::string> c_preset;
    if (!preset.is_empty()) {
        c_preset = preset.utf8().get_data();
    }

    std::string result = controller::create_session_registration_url(
        private_key.utf8().get_data(),
        c_policies,
        rpc_url.utf8().get_data(),
        c_preset
    );
    return {result.c_str()};
}

String ControllerHelper::create_session_registration_url_with_urls(const String &private_key, const Dictionary &policies, const String &rpc_url, const String &keychain_url, const String &cartridge_api_url, const String &preset)
{
    controller::SessionPolicies c_policies = to_c_policies(policies);
    std::optional<std::string> c_preset;
    if (!preset.is_empty()) {
        c_preset = preset.utf8().get_data();
    }

    std::string result = controller::create_session_registration_url_with_urls(
        private_key.utf8().get_data(),
        c_policies,
        rpc_url.utf8().get_data(),
        keychain_url.utf8().get_data(),
        cartridge_api_url.utf8().get_data(),
        c_preset
    );
    return {result.c_str()};
}
