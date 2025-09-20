//
// Created by hazel on 21/06/25.
//

#ifndef POLICY_H
#define POLICY_H
#include "godot_cpp/classes/resource.hpp"

using namespace godot;

class DojoPolicy : public Resource
{
    GDCLASS(DojoPolicy, Resource)

public:
    DojoPolicy(){};
    ~DojoPolicy(){};
    
    void set_method(const String& p_method) {
        method = p_method;
        method_buffer = CharString();
        emit_changed();
    };
    
    String get_method() const { return method; };
    
    void set_description(const String& p_description) {
        description = p_description;
        description_buffer = CharString();
        emit_changed();
    };
    
    String get_description() const { return description; };

    const char* get_method_ctr() const {
        if (method_buffer.size() == 0) {
            method_buffer = method.utf8();
        }
        return method_buffer.get_data();
    }
    
    const char* get_description_ctr() const {
        if (description_buffer.size() == 0) {
            description_buffer = description.utf8();
        }
        return description_buffer.get_data();
    }

    Dictionary to_dictionary() const {
        Dictionary dict;
        dict["method"] = method;
        dict["description"] = description;
        return dict;
    }

    static Ref<DojoPolicy> from_dictionary(const Dictionary& dict) {
        Ref<DojoPolicy> policy;
        policy.instantiate();
        policy->_from_dictionary(dict);
        return policy;
    }


protected:
    void _from_dictionary(const Dictionary& dict) {
        if (dict.has("method")) {
            set_method(dict["method"]);
        }
        if (dict.has("description")) {
            set_description(dict["description"]);
        }
    }

    String target;
    String method;
    String description;
    
    mutable CharString method_buffer;
    mutable CharString description_buffer;

    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_method", "method"), &DojoPolicy::set_method);
        ClassDB::bind_method(D_METHOD("get_method"), &DojoPolicy::get_method);
        ClassDB::bind_method(D_METHOD("set_description", "description"), &DojoPolicy::set_description);
        ClassDB::bind_method(D_METHOD("get_description"), &DojoPolicy::get_description);
        ClassDB::bind_method(D_METHOD("to_dictionary"), &DojoPolicy::to_dictionary);
        ClassDB::bind_static_method(get_class_static(), D_METHOD("from_dictionary", "dictionary"),
                                    &DojoPolicy::from_dictionary);

        ADD_PROPERTY(PropertyInfo(Variant::STRING, "method"), "set_method", "get_method");
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "description"), "set_description", "get_description");
    };
};

#endif //POLICY_H