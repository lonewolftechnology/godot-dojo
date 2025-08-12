//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_U64_H
#define OPTION_U64_H

#include "../dojo_option.h"
#include "dojo_types.h"

using namespace godot;

class OptionU64 : public DojoOption {
    GDCLASS(OptionU64, DojoOption);

protected:
    static void _bind_methods() {}

public:
    OptionU64() : DojoOption() {}
    OptionU64(uint64_t value) : DojoOption(value) {}

    DOJO::COptionu64 get_native_option() const {
        DOJO::COptionu64 option = {};
        if (is_some()) {
            option.tag = DOJO::Someu64;
            option.some = static_cast<uint64_t>((int64_t)get_value());
        } else {
            option.tag = DOJO::Noneu64;
        }
        return option;
    }
};

#endif //OPTION_U64_H
