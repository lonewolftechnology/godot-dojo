//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_U256_H
#define OPTION_U256_H

#include "../dojo_option.h"
#include "dojo_types.h"
#include "tools/dojo_helper.h"
#include "types/big_int.h"

using namespace godot;

class DojoOptionU256 : public DojoOption {
    GDCLASS(DojoOptionU256, DojoOption);

protected:
    static void _bind_methods() {
    }

public:
    DojoOptionU256() : DojoOption() {
    }

    DojoOptionU256(const String &value) : DojoOption(value) {
    }

    DojoOptionU256(const DOJO::COptionU256 &native_option) : DojoOption() {
        if (native_option.tag == DOJO::SomeU256) {
            Ref<U256> _u256 = memnew(U256(native_option.some));
            set_value(_u256);
        } else {
            set_value(Variant());
        }
    }

    DOJO::COptionU256 get_native_option() const {
        DOJO::COptionU256 option = {};
        if (is_some()) {
            option.tag = DOJO::SomeU256;
            option.some = DojoHelpers::string_to_u256(get_value());
        } else {
            option.tag = DOJO::NoneU256;
        }
        return option;
    }
};

#endif //OPTION_U256_H
