//
// Created by hazel on 2/08/25.
//

#ifndef OPTION_TRANSACTION_FILTER_H
#define OPTION_TRANSACTION_FILTER_H

#include "../dojo_option.h"
#include "resources/queries/dojo_transaction_filter.h"
#include "dojo_types.h"

using namespace godot;

class OptionTransactionFilter : public DojoOption {
    GDCLASS(OptionTransactionFilter, DojoOption);

protected:
    static void _bind_methods() {}

public:
    OptionTransactionFilter() : DojoOption() {}
    OptionTransactionFilter(const Ref<DojoTransactionFilter>& value) : DojoOption(value) {}

    DOJO::COptionTransactionFilter get_native_option() const {
        DOJO::COptionTransactionFilter option = {};
        if (is_some()) {
            option.tag = DOJO::SomeTransactionFilter;
            Ref<DojoTransactionFilter> filter = get_value();
            if (filter.is_valid()) {
                option.some = filter->get_native();
            }
        } else {
            option.tag = DOJO::NoneTransactionFilter;
        }
        return option;
    }
};

#endif //OPTION_TRANSACTION_FILTER_H
