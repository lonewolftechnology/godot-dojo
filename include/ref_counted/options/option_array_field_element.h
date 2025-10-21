//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_ARRAY_FIELD_ELEMENT_H
#define OPTION_ARRAY_FIELD_ELEMENT_H

#include "../dojo_option.h"
#include "dojo_types.h"
#include "tools/dojo_helper.h"

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

        TypedArray<String> arr = get_value();
        auto* data = new DOJO::FieldElement[arr.size()];
        for (int i = 0; i < arr.size(); ++i) {
            String str = arr[i];
            DOJO::U256 u256_val = DojoHelpers::string_to_u256(str);
            memcpy(data[i].data, u256_val.data, 32);
        }
        return {data, (uintptr_t)arr.size()};
    }
};

#endif //OPTION_ARRAY_FIELD_ELEMENT_H
