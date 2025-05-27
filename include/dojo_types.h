//
// Created by hazel on 26/05/25.
//

#ifndef DOJO_TYPES_H
#define DOJO_TYPES_H
#include "dojo.h"
// Se listan las mas usadas

namespace dojo_bindings {

// Enums
enum Ty_Tag;
enum Primitive_Tag;
enum PaginationDirection;

// Estructuras básicas
struct FieldElement;

struct Primitive;
struct Struct;
struct Enum;
struct Array;
struct Tuple;
struct Ty;

// Estructuras de Cliente y Proveedor
struct ToriiClient;
struct Provider;
struct Event;
struct Subscription;

// Estructuras de Resultados
struct ResultToriiClient;
struct ResultProvider;
struct ResultSubscription;
struct ResultPageEntity;
struct ResultControllerAccount;

// Estructuras de Paginación y Consulta
struct Pagination;
struct Query;
struct KeysClause;
struct CArrayOrderBy;
struct COptionc_char;

// Estructuras de Control y Política
struct Policy;

struct Member;

// Estructuras de Array y Opciones
struct CArrayMember;

struct CArrayEntity;

struct COptionClause;
struct COptionFieldElement;
struct CArrayCOptionFieldElement;
struct CArrayc_char;

// Estructuras de Error
struct Error;

} // namespace dojo_bindings

#endif // DOJO_TYPES_H