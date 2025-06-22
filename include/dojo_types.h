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

enum Ty_Tag;
enum Primitive_Tag;
enum PaginationDirection;
enum BlockTag;
enum ComparisonOperator;
enum LogicalOperator;
enum OrderDirection;
enum PatternMatching;

// ============================================================================
// Tipos de Datos Fundamentales
// ============================================================================

struct FieldElement;
struct U256;
struct Primitive;
struct Struct;
struct Enum;
struct Ty;
struct Member;
struct EnumOption;
struct MemberValue;
struct Signature;

// ============================================================================
// Cliente y Conexión
// ============================================================================

struct ToriiClient;
struct Provider;
struct ControllerAccount;
struct Account;
struct Controller;
struct Event;
struct Subscription;

// ============================================================================
// Metadatos y Modelos
// ============================================================================

struct WorldMetadata;
struct ModelMetadata;
struct Entity;

// ============================================================================
// Tokens y Balances
// ============================================================================

struct Token;
struct TokenBalance;
struct TokenCollection;
struct IndexerUpdate;

// ============================================================================
// Sistema de Consultas y Paginación
// ============================================================================

struct Pagination;
struct Query;
struct Clause;
struct KeysClause;
struct MemberClause;
struct CompositeClause;
struct OrderBy;

// ============================================================================
// Blockchain y Transacciones
// ============================================================================

struct Call;
struct BlockId;
struct Policy;

// ============================================================================
// Páginas y Paginación
// ============================================================================

struct PageEntity;
struct PageToken;
struct PageTokenBalance;
struct PageTokenCollection;

// ============================================================================
// Tipos de Resultado
// ============================================================================

struct ResultToriiClient;
struct ResultProvider;
struct ResultAccount;
struct ResultControllerAccount;
struct ResultSubscription;
struct ResultPageEntity;
struct ResultPageToken;
struct ResultPageTokenBalance;
struct ResultPageTokenCollection;
struct ResultWorldMetadata;
struct ResultFieldElement;
struct ResultCArrayFieldElement;
struct ResultCArrayController;
struct Resultbool;
struct Resultc_char;
struct ResultSignature;

// ============================================================================
// Arrays de C
// ============================================================================

struct CArrayController;
struct CArrayEntity;
struct CArrayStruct;
struct CArrayMember;
struct CArrayEnumOption;
struct CArrayTy;
struct CArrayOrderBy;
struct CArrayClause;
struct CArrayFieldElement;
struct CArrayCOptionFieldElement;
struct CArrayc_char;
struct CArrayMemberValue;
struct CArrayToken;
struct CArrayTokenBalance;
struct CArrayTokenCollection;
struct CArrayCHashItemFieldElementModelMetadata;

// ============================================================================
// Tipos Opcionales
// ============================================================================

struct COptionFieldElement;
struct COptionc_char;
struct COptionClause;

// ============================================================================
// Tipos de Hash
// ============================================================================

struct CHashItemFieldElementModelMetadata;

// ============================================================================
// Manejo de Errores
// ============================================================================

struct Error;

} // namespace dojo_bindings

#endif // DOJO_TYPES_H