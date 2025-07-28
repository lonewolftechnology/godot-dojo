//
// Created by hazel on 24/06/25.
//

#ifndef DOJO_ARRAY_H
#define DOJO_ARRAY_H
#include "godot_cpp/classes/ref_counted.hpp"

#include "dojo_types.h"
using namespace godot;

class ArrayDojo : public RefCounted
{
    GDCLASS(ArrayDojo, RefCounted);

    String name = "";
    Variant value = Variant::NIL;

public:
    ArrayDojo()
    {
    };

    ~ArrayDojo()
    {
    };
    // For now almost every type of CArray is implemented
    // There are some that aren't needed and/or need to be converted (Variant -> CArray)
    ArrayDojo(DOJO::CArrayTy array);
    ArrayDojo(DOJO::CArrayToken array);
    ArrayDojo(DOJO::CArrayEntity array);
    ArrayDojo(DOJO::CArrayc_char array);
    ArrayDojo(DOJO::CArrayClause array);
    ArrayDojo(DOJO::CArrayStruct array);
    ArrayDojo(DOJO::CArrayMember array);
    ArrayDojo(DOJO::CArrayOrderBy array);
    ArrayDojo(DOJO::CArrayEnumOption array);
    ArrayDojo(DOJO::PageController array);
    ArrayDojo(DOJO::CArrayMemberValue array);
    ArrayDojo(DOJO::CArrayFieldElement array);
    ArrayDojo(DOJO::CArrayTokenCollection array);
    ArrayDojo(DOJO::CArrayCOptionFieldElement array);
    ArrayDojo(DOJO::CArrayModel array);

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

protected:
    static void _bind_methods()
    {
    };
};

#endif //DOJO_ARRAY_H
