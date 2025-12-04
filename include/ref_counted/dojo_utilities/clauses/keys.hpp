#pragma once

#include "dojo_types.h"
#include "godot_cpp/classes/ref_counted.hpp"

using namespace godot;

class KeysClause : public RefCounted {
    GDCLASS(KeysClause, RefCounted)

    PackedStringArray p_keys; // Option Felt
    DOJO::PatternMatching p_patter_matching;
    PackedStringArray p_models;

public:
    KeysClause();
    ~KeysClause();

protected:
    static void _bind_methods() {

    }
};
