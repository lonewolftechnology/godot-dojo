#include "tools/controller_helper.h"

#include "godot_cpp/classes/ref_counted.hpp"
#include "tools/dojo_helper.h"
#include <iomanip>
#include <sstream>
#include <types/big_int.h>
#include <random>
#include <deque>

#include <boost/multiprecision/cpp_int.hpp>

#include "tools/logger.h"


bool ControllerHelper::has_storage(const String &app_id) {
    return controller::controller_has_storage(app_id.utf8().get_data());
}

String ControllerHelper::get_controller_class_hash(int version) {
    return String(controller::get_controller_class_hash(static_cast<controller::Version>(version)).c_str());
}

String ControllerHelper::get_public_key(const String &private_key) {
    return String(controller::get_public_key(private_key.utf8().get_data()).c_str());
}

String ControllerHelper::signer_to_guid(const String &private_key) {
    return String(controller::signer_to_guid(private_key.utf8().get_data()).c_str());
}

bool ControllerHelper::validate_felt(const String &felt) {
    return controller::validate_felt(felt.utf8().get_data());
}

String ControllerHelper::generate_private_key() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<unsigned int> dis(0, 255);

    std::stringstream ss;
    ss << "0x";
    for (int i = 0; i < 32; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int) dis(gen);
    }
    return ss.str().data();
}

std::vector<std::shared_ptr<controller::Call>> ControllerHelper::prepare_calls(const TypedArray<Dictionary> &calls) {
    std::vector<std::shared_ptr<controller::Call>> prepared_calls;

    for (int i = 0; i < calls.size(); ++i) {
        Dictionary call_dict = calls[i];
        if (!call_dict.has("contract_address") || !call_dict.has("entrypoint")) {
            Logger::info("Skipping a call due to missing 'contract_address' or 'entrypoint'.");
            continue;
        }

        auto current_call = std::make_shared<controller::Call>();
        current_call->contract_address = static_cast<String>(call_dict["contract_address"]).utf8().get_data();
        current_call->entrypoint = static_cast<String>(call_dict["entrypoint"]).utf8().get_data();
        
        if (!call_dict.has("calldata") || call_dict["calldata"].get_type() != Variant::ARRAY) {
            prepared_calls.push_back(current_call);
            continue;
        }
        Array args = call_dict["calldata"];
        std::deque<Variant> worklist;
        for (int j = 0; j < args.size(); ++j) {
            worklist.push_back(args[j]);
        }

        while (!worklist.empty()) {
            Variant arg = worklist.front();
            worklist.pop_front();

            switch (arg.get_type()) {
                case Variant::Type::ARRAY: {
                    Array inner_array = arg;
                    for (int k = inner_array.size() - 1; k >= 0; --k) {
                        worklist.push_front(inner_array[k]);
                    }
                    continue;
                }
                case Variant::Type::FLOAT: {
                    worklist.push_front(DojoHelpers::double_to_variant_fp(arg, DojoHelpers::get_dojo_setting("fixed_point/default")));
                    continue;
                }
                case Variant::Type::VECTOR2: {
                    Vector2 vec = arg;
                    worklist.push_front(vec.y);
                    worklist.push_front(vec.x);
                    continue;
                }
                case Variant::Type::VECTOR2I: {
                    Vector2i vec = arg;
                    worklist.push_front(static_cast<int64_t>(vec.y));
                    worklist.push_front(static_cast<int64_t>(vec.x));
                    continue;
                }
                case Variant::Type::VECTOR3: {
                    Vector3 vec = arg;
                    worklist.push_front(vec.z);
                    worklist.push_front(vec.y);
                    worklist.push_front(vec.x);
                    continue;
                }
                case Variant::Type::VECTOR3I: {
                    Vector3i vec = arg;
                    worklist.push_front(static_cast<int64_t>(vec.z));
                    worklist.push_front(static_cast<int64_t>(vec.y));
                    worklist.push_front(static_cast<int64_t>(vec.x));
                    continue;
                }
                case Variant::Type::BOOL:
                    current_call->calldata.push_back(static_cast<bool>(arg) ? "0x1" : "0x0");
                    break;
                case Variant::Type::INT: {                    
                    int64_t int_val = arg;
                    boost::multiprecision::cpp_int felt_val;
                    if (int_val < 0) {
                        felt_val = DojoHelpers::to_starknet_negative_felt(int_val);
                    } else {
                        felt_val = int_val;
                    }
                    std::stringstream ss;
                    ss << "0x" << std::hex << felt_val;
                    current_call->calldata.push_back(ss.str());
                    break;
                }
                case Variant::Type::PACKED_BYTE_ARRAY: {
                    PackedByteArray arr = arg;
                    String hex_str = "0x";
                    for (int k = 0; k < arr.size(); ++k) {
                        hex_str += String::num_uint64(arr[k], 16).lpad(2, "0");
                    }
                    current_call->calldata.push_back(hex_str.utf8().get_data());
                    break;
                }
                case Variant::Type::STRING:
                    current_call->calldata.push_back(static_cast<String>(arg).utf8().get_data());
                    break;
                case Variant::Type::OBJECT: {
                    Ref<RefCounted> obj = arg;
                    if (obj.is_valid()) {
                        if (obj->is_class("U256")) {
                            Ref<U256> u256 = obj;
                            current_call->calldata.push_back(u256->to_string().utf8().get_data());
                            continue;
                        }
                        if (obj->is_class("U128")) {
                            Ref<U128> u128 = obj;
                            current_call->calldata.push_back(u128->to_string().utf8().get_data());
                            continue;
                        }
                        if (obj->is_class("I128")) {
                            Ref<I128> i128 = obj;
                            current_call->calldata.push_back(i128->to_string().utf8().get_data());
                            continue;
                        }
                        if (obj->is_class("FieldElement")) {
                            Ref<FieldElement> felt = obj;
                            current_call->calldata.push_back(felt->to_string().utf8().get_data());
                            continue;
                        }
                    }
                    Logger::warning("prepare_calldata: Unsupported object type in calldata: " + obj->get_class());
                    break;
                }
                default:
                    Logger::warning("prepare_calldata: Unsupported variant type in calldata: " + Variant::get_type_name(arg.get_type()));
                    break;
            }
        }
        prepared_calls.push_back(current_call);
    }

    return prepared_calls;
}
