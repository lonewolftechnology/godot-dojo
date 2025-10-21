//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_FIELD_ELEMENT_H
#define OPTION_FIELD_ELEMENT_H

#include "../dojo_option.h"
#include "dojo_types.h"
#include "tools/dojo_helper.h"

using namespace godot;

class DojoOptionFieldElement : public DojoOption {
    GDCLASS(DojoOptionFieldElement, DojoOption);

protected:
    static void _bind_methods() {}

public:
    DojoOptionFieldElement() : DojoOption() {}
    DojoOptionFieldElement(const String& value) : DojoOption(value) {}

    DOJO::COptionFieldElement get_native_option() const {
        DOJO::COptionFieldElement option = {};
        if (is_some()) {
            option.tag = DOJO::SomeFieldElement;
            DOJO::U256 u256_val = DojoHelpers::string_to_u256(get_value());
            memcpy(option.some.data, u256_val.data, 32);
        } else {
            option.tag = DOJO::NoneFieldElement;
        }
        return option;
    }
};

#endif //OPTION_FIELD_ELEMENT_H
