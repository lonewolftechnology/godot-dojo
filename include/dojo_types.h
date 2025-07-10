//
// Created by hazel on 26/05/25.
//
#ifndef DOJO_TYPES_H
#define DOJO_TYPES_H
#include "dojo.h"

#define DOJO dojo_bindings
#define GET_DOJO_ERROR(result) ((result).err.message)
#define GET_DOJO_OK(result) ((result).ok)
#define GET_DOJO_OK_PTR(result) ((&result).ok)

/**
 * @file dojo_types.h
 * Declaraciones forward para los tipos de datos utilizados en Dojo
 *
 * Este archivo contiene las declaraciones forward de todos los tipos de datos
 * principales utilizados en el ecosistema Dojo, incluyendo estructuras para
 * clientes, proveedores, consultas, eventos y manejo de errores.
 *
 * Las declaraciones forward permiten reducir las dependencias de compilación
 * y mejorar los tiempos de compilación al evitar incluir dojo.h completo
 * cuando solo se necesitan referencias a los tipos.
 */

namespace dojo_bindings {

// ============================================================================
// Enumeraciones
// ============================================================================

enum BlockTag;
enum ComparisonOperator;
enum LogicalOperator;
enum OrderDirection;
enum PaginationDirection;
enum PatternMatching;
enum Primitive_Tag;
enum ResultToriiClient_Tag;
enum ResultControllerAccount_Tag;
enum Resultbool_Tag;
enum ResultFieldElement_Tag;
enum ResultCArrayFieldElement_Tag;
enum ResultPageController_Tag;
enum ResultPageEntity_Tag;
enum ResultPageToken_Tag;
enum ResultPageTokenBalance_Tag;
enum ResultPageTokenCollection_Tag;
enum ResultSignature_Tag;
enum ResultProvider_Tag;
enum ResultAccount_Tag;
enum BlockId_Tag;
enum COptionFieldElement_Tag;
enum COptionc_char_Tag;
enum COptionu32_Tag;
enum MemberValue_Tag;
enum Clause_Tag;
enum COptionClause_Tag;
enum ResultWorld_Tag;
enum ResultSubscription_Tag;
enum Resultc_char_Tag;
enum Ty_Tag;

// ============================================================================
// Clases Principales
// ============================================================================

class ToriiClient;
class Policy;
class ControllerAccount;
class Call;
class Controller;
class Entity;
class OrderBy;
class COptionFieldElement;
class Model;
class Subscription;
class Struct;
class Token;
class TokenBalance;
class TokenCollection;
class Provider;
class Account;
class Ty;
class Member;
class EnumOption;

// ============================================================================
// Estructuras Fundamentales
// ============================================================================

struct FieldElement;
struct Message;
struct CArrayFieldElement;
struct CArrayController;
struct COptionc_char;
struct PageController;
struct COptionu32;
struct CArrayEntity;
struct PageEntity;
struct CArrayOrderBy;
struct Pagination;
struct CArrayCOptionFieldElement;
struct CArrayc_char;
struct KeysClause;
struct U256;
struct Primitive;
struct CArrayMemberValue;
struct MemberValue;
struct MemberClause;
struct CArrayClause;
struct CompositeClause;
struct Clause;
struct COptionClause;
struct Query;
struct CArrayModel;
struct World;
struct CArrayStruct;
struct Event;
struct CArrayToken;
struct PageToken;
struct CArrayTokenBalance;
struct PageTokenBalance;
struct CArrayTokenCollection;
struct PageTokenCollection;
struct IndexerUpdate;
struct Signature;
struct BlockId;
struct CArrayMember;
struct CArrayEnumOption;
struct Error;

// ============================================================================
// Tipos de Resultado
// ============================================================================

struct ResultToriiClient;
struct ResultControllerAccount;
struct Resultbool;
struct ResultFieldElement;
struct ResultCArrayFieldElement;
struct ResultPageController;
struct ResultPageEntity;
struct ResultWorld;
struct ResultSubscription;
struct ResultPageToken;
struct ResultPageTokenBalance;
struct ResultPageTokenCollection;
struct Resultc_char;
struct ResultSignature;
struct ResultProvider;
struct ResultAccount;

} // namespace dojo_bindings

#endif // DOJO_TYPES_H