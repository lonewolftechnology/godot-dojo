//
// Created by hazel on 24/06/25.
//

#ifndef DOJO_ARRAY_H
#define DOJO_ARRAY_H
#include "godot_cpp/classes/ref_counted.hpp"

#include "dojo_types.h"
#include "variant/field_element.h"
#include "tools/logger.h"
#include "ref_counted/options/option_field_element.h"
#include "tools/dojo_helper.h"

using namespace godot;

namespace DojoArrayHelpers {
// TODO: refactor, remove, improve
    inline DOJO::CArrayFieldElement string_array_to_native_carray_felt(const TypedArray<String>& arr) {
        auto* native_arr = static_cast<DOJO::FieldElement*>(malloc(sizeof(DOJO::FieldElement) * arr.size()));
        for (int i = 0; i < arr.size(); ++i) {
            // Construct the object directly in the allocated memory
            native_arr[i] = FieldElement::from_string(arr[i]); // Now assign the value
        }
        return {native_arr, static_cast<uintptr_t>(arr.size())};
    }

    inline DOJO::CArrayU256 string_array_to_native_carray_u256(const TypedArray<String>& arr) {
        auto* native_arr = new DOJO::U256[arr.size()];
        for (int i = 0; i < arr.size(); ++i) {
            String str = arr[i];
            native_arr[i] = DojoHelpers::string_to_u256(str);
        }
        return {native_arr, static_cast<uintptr_t>(arr.size())};
    }

    inline DOJO::CArrayc_char string_array_to_native_carray_str(const TypedArray<String>& arr) {
        const char** native_arr = new const char*[arr.size()];
        for (int i = 0; i < arr.size(); ++i) {
            String str = arr[i];
            char *c_str = static_cast<char*>(malloc(str.utf8().length() + 1));
            strcpy(c_str, str.utf8().get_data());
            native_arr[i] = c_str;
        }
        return {native_arr, static_cast<uintptr_t>(arr.size())};
    }

    inline DOJO::CArrayCOptionFieldElement option_field_element_array_to_native_carray(const Array& arr) {
        auto* native_arr = static_cast<DOJO::COptionFieldElement*>(malloc(sizeof(DOJO::COptionFieldElement) * arr.size()));
        for (int i = 0; i < arr.size(); ++i) {
            Ref<DojoOptionFieldElement> key_ref = arr[i];
            if (key_ref.is_valid())
            {
                native_arr[i] = key_ref->get_native_option();
            }
            else
            {
                native_arr[i].tag = DOJO::NoneFieldElement;
            }
        }
        return {native_arr, static_cast<uintptr_t>(arr.size())};
    }

    inline DOJO::CArrayOrderBy dictionary_array_to_native_carray_orderby(const TypedArray<Dictionary>& arr) {
        auto* native_arr = new DOJO::OrderBy[arr.size()];
        for (int i = 0; i < arr.size(); ++i) {
            Dictionary dict = arr[i];
            String field_str = dict["field"];

            char *field_cstr = new char[field_str.utf8().length() + 1];
            strcpy(field_cstr, field_str.utf8().get_data());

            native_arr[i] = {field_cstr, static_cast<DOJO::OrderDirection>(dict["direction"].operator int64_t())};
        }
        return {native_arr, static_cast<uintptr_t>(arr.size())};
    }

    inline void free_native_carray_felt(DOJO::CArrayFieldElement& arr) {
        std::free(arr.data);
        arr.data = nullptr;
        arr.data_len = 0;
    }

    inline void free_native_carray_str(DOJO::CArrayc_char& arr) {
        if (arr.data) {
            // The Rust side will free the individual strings. We only free the array of pointers.
            delete[] arr.data;
        }
        arr.data = nullptr;
        arr.data_len = 0;
    }

    inline void free_native_carray_option_field_element(DOJO::CArrayCOptionFieldElement& arr) {
        if (arr.data) {
            // COptionFieldElement has no internal memory that needs to be freed,
            // so we only need to free the main array.
            std::free(arr.data);
        }
        arr.data = nullptr;
        arr.data_len = 0;
    }

}


class DojoArray : public RefCounted
{
    GDCLASS(DojoArray, RefCounted);

    String name = "";
    Variant value = Variant::NIL;

public:
    DojoArray()
    {
    };

    ~DojoArray()
    {
    };
    // For now almost every type of CArray is implemented
    // There are some that aren't needed and/or need to be converted (Variant -> CArray)
    DojoArray(DOJO::CArrayTy array);
    DojoArray(DOJO::CArrayToken array);
    DojoArray(DOJO::CArrayEntity array);
    DojoArray(DOJO::CArrayc_char array);
    DojoArray(DOJO::CArrayClause array);
    DojoArray(DOJO::CArrayWorld array);
    DojoArray(DOJO::CArrayStruct array);
    DojoArray(DOJO::CArrayMember array);
    DojoArray(DOJO::CArrayOrderBy array);
    DojoArray(DOJO::CArrayEnumOption array);
    DojoArray(DOJO::PageController array);
    DojoArray(DOJO::CArrayMemberValue array);
    DojoArray(DOJO::CArrayFieldElement array);
    DojoArray(DOJO::CArrayTokenContract array);
    DojoArray(DOJO::CArrayCOptionFieldElement array);
    DojoArray(DOJO::CArrayModel array);
    DojoArray(DOJO::CArrayActionCount array);
    DojoArray(DOJO::CArrayActivity array);
    DojoArray(DOJO::CArrayAggregationEntry array);
    DojoArray(DOJO::CArrayAchievement array);
    DojoArray(DOJO::CArrayTokenTransfer array);
    DojoArray(DOJO::CArrayTokenBalance array);
    DojoArray(DOJO::CArrayAchievementTask array);
    DojoArray(DOJO::CArrayPlayerAchievementEntry array);
    DojoArray(DOJO::CArrayPlayerAchievementProgress array);
    DojoArray(DOJO::CArrayTaskProgress array);
    DojoArray(DOJO::CArrayTableSearchResults array);
    DojoArray(DOJO::CArraySearchMatch array);
    DojoArray(DOJO::CArraySearchMatchField array);

    // "static" methods.
    static Variant CArrayTyToVariant(DOJO::CArrayTy array);
    static Variant CArrayTokenToVariant(DOJO::CArrayToken array);
    static Variant CArrayEntityToVariant(DOJO::CArrayEntity array);
    static Variant CArrayc_charToVariant(DOJO::CArrayc_char array);
    static Variant CArrayClauseToVariant(DOJO::CArrayClause array);
    static Variant CArrayWorldToVariant(DOJO::CArrayWorld array);
    static Variant CArrayStructToVariant(DOJO::CArrayStruct array);
    static Variant CArrayMemberToVariant(DOJO::CArrayMember array);
    static Variant CArrayOrderByToVariant(DOJO::CArrayOrderBy array);
    static Variant CArrayEnumOptionToVariant(DOJO::CArrayEnumOption array);
    static Variant PageControllerToVariant(const DOJO::PageController& array);
    static Variant CArrayMemberValueToVariant(DOJO::CArrayMemberValue array);
    static Variant CArrayFieldElementToVariant(DOJO::CArrayFieldElement array);
    static Variant CArrayTokenContractToVariant(DOJO::CArrayTokenContract array);
    static Variant CArrayCOptionFieldElementToVariant(DOJO::CArrayCOptionFieldElement array);
    static Variant CArrayModelToVariant(DOJO::CArrayModel array);
    static Variant CArrayActionCountToVariant(DOJO::CArrayActionCount array);
    static Variant CArrayActivityToVariant(DOJO::CArrayActivity array);
    static Variant CArrayAggregationEntryToVariant(DOJO::CArrayAggregationEntry array);
    static Variant CArrayAchievementToVariant(DOJO::CArrayAchievement array);
    static Variant CArrayAchievementTaskToVariant(DOJO::CArrayAchievementTask array);
    static Variant CArrayTokenTransferToVariant(DOJO::CArrayTokenTransfer array);
    static Variant CArrayTokenBalanceToVariant(DOJO::CArrayTokenBalance array);
    static Variant CArrayPlayerAchievementEntryToVariant(DOJO::CArrayPlayerAchievementEntry array);
    static Variant CArrayPlayerAchievementProgressToVariant(DOJO::CArrayPlayerAchievementProgress array);
    static Variant CArrayTaskProgressToVariant(DOJO::CArrayTaskProgress array);
    static Variant CArrayTableSearchResultsToVariant(DOJO::CArrayTableSearchResults array);
    static Variant CArraySearchMatchToVariant(DOJO::CArraySearchMatch array);
    static Variant CArraySearchMatchFieldToVariant(DOJO::CArraySearchMatchField array);



    Variant get_value() const { return value; }
    void set_value(const Variant& p_value) { value = p_value; }
    String get_name() const { return name; }
    void set_name(const String& p_name) { name = p_name; }

    struct CStringArrayHelper {
        std::vector<CharString> utf8_storage;

        std::vector<const char*> ptr_storage;

        DOJO::CArrayc_char c_array;

        CStringArrayHelper(const Array& godot_array) {
            utf8_storage.reserve(godot_array.size());
            ptr_storage.reserve(godot_array.size());

            for (int i = 0; i < godot_array.size(); ++i) {
                utf8_storage.push_back(String(godot_array[i]).utf8());
            }

            for (const CharString& char_string : utf8_storage) {
                ptr_storage.push_back(char_string.get_data());
            }

            c_array.data = ptr_storage.data();
            c_array.data_len = ptr_storage.size();
        }
    };
    struct CFieldElementArrayHelper {
    private:
        std::vector<DOJO::FieldElement> fe_storage;
        template <typename T>
        void initialize_from_array(const T& hex_string_array) {
            fe_storage.reserve(hex_string_array.size());

            for (int i = 0; i < hex_string_array.size(); ++i) {
                String address = hex_string_array[i];
                fe_storage.push_back(FieldElement::from_string(address));
            }
            c_array.data = fe_storage.data();
            c_array.data_len = fe_storage.size();

            Logger::debug_extra("CXexFelt", fe_storage.size());
        }

    public:
        DOJO::CArrayFieldElement c_array;

        CFieldElementArrayHelper(const TypedArray<String>& hex_string_array) {
            initialize_from_array(hex_string_array);
        }

        CFieldElementArrayHelper(const PackedStringArray& hex_string_array) {
            initialize_from_array(hex_string_array);
        }
    };
protected:
    static void _bind_methods()
    {
    };
};

#endif //DOJO_ARRAY_H
