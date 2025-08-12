//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_U256_H
#define OPTION_U256_H

#include "../dojo_option.h"
#include "dojo_types.h"
#include "tools/dojo_helper.h"

using namespace godot;

class OptionU256 : public DojoOption {
    GDCLASS(OptionU256, DojoOption);

protected:
    static void _bind_methods() {}

public:
    OptionU256() : DojoOption() {}
    OptionU256(const String& value) : DojoOption(value) {}

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
