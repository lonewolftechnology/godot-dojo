//
// Created by hazel on 24/06/25.
//

#ifndef DOJO_ARRAY_H
#define DOJO_ARRAY_H
#include "godot_cpp/classes/ref_counted.hpp"

#include "dojo_types.h"
#include "variant/field_element.h"
#include "tools/logger.h"
using namespace godot;

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
    DojoArray(DOJO::CArrayStruct array);
    DojoArray(DOJO::CArrayMember array);
    DojoArray(DOJO::CArrayOrderBy array);
    DojoArray(DOJO::CArrayEnumOption array);
    DojoArray(DOJO::PageController array);
    DojoArray(DOJO::CArrayMemberValue array);
    DojoArray(DOJO::CArrayFieldElement array);
    DojoArray(DOJO::CArrayTokenCollection array);
    DojoArray(DOJO::CArrayCOptionFieldElement array);
    DojoArray(DOJO::CArrayModel array);

    // "static" methods.
    static Variant CArrayTyToVariant(DOJO::CArrayTy array);
    static Variant CArrayTokenToVariant(DOJO::CArrayToken array);
    static Variant CArrayEntityToVariant(DOJO::CArrayEntity array);
    static Variant CArrayc_charToVariant(DOJO::CArrayc_char array);
    static Variant CArrayClauseToVariant(DOJO::CArrayClause array);
    static Variant CArrayStructToVariant(DOJO::CArrayStruct array);
    static Variant CArrayMemberToVariant(DOJO::CArrayMember array);
    static Variant CArrayOrderByToVariant(DOJO::CArrayOrderBy array);
    static Variant CArrayEnumOptionToVariant(DOJO::CArrayEnumOption array);
    static Variant PageControllerToVariant(const DOJO::PageController& array);
    static Variant CArrayMemberValueToVariant(DOJO::CArrayMemberValue array);
    static Variant CArrayFieldElementToVariant(DOJO::CArrayFieldElement array);
    static Variant CArrayTokenCollectionToVariant(DOJO::CArrayTokenCollection array);
    static Variant CArrayCOptionFieldElementToVariant(DOJO::CArrayCOptionFieldElement array);
    static Variant CArrayModelToVariant(DOJO::CArrayModel array);

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

    public:
        DOJO::CArrayFieldElement c_array;

        CFieldElementArrayHelper(const TypedArray<String>& hex_string_array) {
            fe_storage.reserve(hex_string_array.size());

            for (int i = 0; i < hex_string_array.size(); ++i) {
                String address = hex_string_array[i];
                fe_storage.push_back(FieldElement::from_string(address));
            }

            c_array.data = fe_storage.data();
            c_array.data_len = fe_storage.size();

            Logger::debug_extra("CXexFelt", fe_storage.size());
        }
    };
protected:
    static void _bind_methods()
    {
    };
};

#endif //DOJO_ARRAY_H
