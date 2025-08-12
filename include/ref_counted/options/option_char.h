//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_CHAR_H
#define OPTION_CHAR_H

#include "../dojo_option.h"
#include "dojo_types.h"

using namespace godot;

class OptionChar : public DojoOption {
    GDCLASS(OptionChar, DojoOption);

protected:
    static void _bind_methods() {}

public:
    OptionChar() : DojoOption() {}
    OptionChar(const String& value) : DojoOption(value) {}

    DOJO::COptionc_char get_native_option() const {
        DOJO::COptionc_char option = {};
        if (is_some()) {
            option.tag = DOJO::Somec_char;
            String str = get_value();
            // The memory for this string must be managed by the caller
            // or the underlying C library.
            char* c_str = new char[str.utf8().length() + 1];
            strcpy(c_str, str.utf8().get_data());
            option.some = c_str;
        } else {
            option.tag = DOJO::Nonec_char;
        }
        return option;
    }
};

#endif //OPTION_CHAR_H
