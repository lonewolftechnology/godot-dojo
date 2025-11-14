//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_F64_H
#define OPTION_F64_H

#include "../dojo_option.h"
#include "dojo_types.h"

using namespace godot;

class DojoOptionf64 : public DojoOption {
    GDCLASS(DojoOptionf64, DojoOption);

protected:
    static void _bind_methods() {}

public:
    DojoOptionf64() : DojoOption() {}
    DojoOptionf64(double value) : DojoOption(value) {}

    DOJO::COptionf64 get_native_option() const {
        DOJO::COptionf64 option = {};
        if (is_some()) {
            option.tag = DOJO::Somef64;
            option.some = static_cast<double>(get_value());
        } else {
            option.tag = DOJO::Nonef64;
        }
        return option;
    }

    static Ref<DojoOptionf64> from_native(const DOJO::COptionf64& native_option) {
        if (native_option.tag == DOJO::Somef64) {
            return memnew(DojoOptionf64(native_option.some));
        }
        return memnew(DojoOptionf64());
    }
};

#endif //OPTION_F64_H
