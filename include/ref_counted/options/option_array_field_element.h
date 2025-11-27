//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_ARRAY_FIELD_ELEMENT_H
#define OPTION_ARRAY_FIELD_ELEMENT_H

#include "../dojo_option.h"
#include "dojo_types.h"
#include "tools/dojo_helper.h"
#include "variant/ty/dojo_array.h"

using namespace godot;

class DojoOptionArrayFieldElement : public DojoOption {
    GDCLASS(DojoOptionArrayFieldElement, DojoOption);

protected:
    static void _bind_methods() {}

public:
    DojoOptionArrayFieldElement() : DojoOption() {}
    DojoOptionArrayFieldElement(const TypedArray<String>& value) : DojoOption(value) {}

    DOJO::CArrayFieldElement get_native_array() const {
        if (!is_some()) {
            return {nullptr, 0};
        }

        return DojoArrayHelpers::string_array_to_native_carray_felt(get_value());
    }
};

#endif //OPTION_ARRAY_FIELD_ELEMENT_H
