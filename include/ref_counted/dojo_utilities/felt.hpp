#pragma once
#include "godot_cpp/classes/ref_counted.hpp"
#include <boost/multiprecision/cpp_int.hpp>

using namespace godot;
using boost::multiprecision::cpp_int;

class Felt : public RefCounted {
    GDCLASS(Felt, RefCounted);
    cpp_int value;

public:
    Felt() : value(0) {}

    void _init_from_string(const String& p_value);
    
    String to_string() const;
    String to_short_string() const;

    static Ref<Felt> from_string(const String& p_value);

protected:
    static void _bind_methods();
};