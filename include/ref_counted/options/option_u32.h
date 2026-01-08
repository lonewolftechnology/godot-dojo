//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_U32_H
#define OPTION_U32_H

#include "../dojo_option.h"
#include "dojo_types.h"

using namespace godot;

class DojoOptionU32 : public DojoOption {
    GDCLASS(DojoOptionU32, DojoOption);

protected:
    static void _bind_methods() {}

public:
    DojoOptionU32() : DojoOption() {}
    DojoOptionU32(uint32_t value) : DojoOption(value) {}

    DOJO::COptionu32 get_native_option() const {
        DOJO::COptionu32 option = {};
        if (is_some()) {
            option.tag = DOJO::Someu32;
            option.some = static_cast<uint32_t>((int64_t)get_value());
        } else {
            option.tag = DOJO::Noneu32;
        }
        return option;
    }
};

#endif //OPTION_U32_H
