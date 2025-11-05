#include "tools/controller_helper.h"

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/variant/vector2.hpp"
#include "godot_cpp/variant/vector2i.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/vector3i.hpp"
#include <iomanip>
#include <random>
#include <deque>

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

std::vector<std::string> ControllerHelper::prepare_calldata(const Array &args) {
    std::vector<std::string> calldata_felts;
    std::deque<Variant> worklist;

    for (int i = 0; i < args.size(); ++i) {
        worklist.push_back(args[i]);
    }

    while (!worklist.empty()) {
        Variant arg = worklist.front();
        worklist.pop_front();

        switch (arg.get_type()) {
            case Variant::Type::ARRAY: {
                Array inner_array = arg;
                for (int i = inner_array.size() - 1; i >= 0; --i) {
                    worklist.push_front(inner_array[i]);
                }
                continue;
            }
            case Variant::Type::FLOAT: {
                // Assuming DojoHelpers is available for this conversion
                // If not, this logic needs to be implemented here as well.
                // worklist.push_front(DojoHelpers::double_to_variant_fp(arg, DojoHelpers::get_setting("dojo/config/fixed_point/default")));
                // For now, let's just convert to int, which is a common case for non-fp numbers
                calldata_felts.push_back(String::num_int64(static_cast<int64_t>(static_cast<double>(arg))).utf8().get_data());
                break;
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
                calldata_felts.push_back(String::num_int64(static_cast<bool>(arg) ? 1 : 0).utf8().get_data());
                break;
            case Variant::Type::INT: {
                calldata_felts.push_back(String::num_int64(arg).utf8().get_data());
                break;
            }
            case Variant::Type::PACKED_BYTE_ARRAY: {
                PackedByteArray arr = arg;
                String hex_str = "0x";
                for (int i = 0; i < arr.size(); ++i) {
                    hex_str += String::num_uint64(arr[i], 16).lpad(2, "0");
                }
                calldata_felts.push_back(hex_str.utf8().get_data());
                break;
            }
            case Variant::Type::STRING:
                calldata_felts.push_back(static_cast<String>(arg).utf8().get_data());
                break;
            case Variant::Type::OBJECT: {
                Ref<RefCounted> obj = arg;
                if (obj.is_valid()) {
                    // This part is simplified. For U256, etc., you'd need to include
                    // the relevant headers and logic from dojo_helper.
                    // For now, we'll just log a warning.
                    // Example for a hypothetical "as_calldata_array" method:
                    // if (obj->has_method("as_calldata_array")) {
                    //     Array obj_calldata = obj->call("as_calldata_array");
                    //     for (int i = obj_calldata.size() - 1; i >= 0; --i) {
                    //         worklist.push_front(obj_calldata[i]);
                    //     }
                    //     continue;
                    // }
                }
                // Logger::warning("prepare_calldata: Unsupported object type in calldata: " + obj->get_class());
                break;
            }
            default:
                // Logger::warning("prepare_calldata: Unsupported variant type in calldata: " + Variant::get_type_name(arg.get_type()));
                break;
        }
    }

    return calldata_felts;
}
