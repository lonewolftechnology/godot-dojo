//
// Created by hazel on 26/05/25.
//
#ifndef DOJO_TYPES_H
#define DOJO_TYPES_H
#include "dojo.h" // 1.6.0

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
enum CallType;
enum ComparisonOperator;
enum LogicalOperator;
enum OrderDirection;
enum PaginationDirection;
enum PatternMatching;
enum Primitive_Tag;
enum MemberValue_Tag;
enum Clause_Tag;
enum BlockId_Tag;
enum Ty_Tag;

// Tags para Tipos Opcionales
enum COptionc_char_Tag;
enum COptionu32_Tag;
enum COptionu64_Tag;
enum COptionClause_Tag;
enum COptionTransactionFilter_Tag;
enum COptionU256_Tag;
enum COptionFieldElement_Tag;

// Tags para Tipos de Resultado
enum ResultToriiClient_Tag;
enum ResultControllerAccount_Tag;
enum Resultbool_Tag;
enum ResultFieldElement_Tag;
enum ResultCArrayFieldElement_Tag;
enum ResultPageController_Tag;
enum ResultPageEntity_Tag;
enum ResultWorld_Tag;
enum ResultPageTransaction_Tag;
enum ResultSubscription_Tag;
enum ResultPageToken_Tag;
enum ResultPageTokenBalance_Tag;
enum ResultPageTokenCollection_Tag;
enum Resultc_char_Tag;
enum ResultSignature_Tag;
enum ResultProvider_Tag;
enum ResultAccount_Tag;


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
struct Message;


// ============================================================================
// Cliente y Conexión
// ============================================================================

struct ToriiClient;
struct Provider;
struct Account;
struct ControllerAccount;
struct Subscription;


// ============================================================================
// Mundo, Modelos y Entidades
// ============================================================================

struct World;
struct Model;
struct Entity;
struct Controller;


// ============================================================================
// Tokens y Balances
// ============================================================================

struct Token;
struct TokenBalance;
struct TokenCollection;
struct IndexerUpdate;


// ============================================================================
// Sistema de Consultas y Cláusulas
// ============================================================================

struct Query;
struct ControllerQuery;
struct TransactionQuery;
struct TokenQuery;
struct TokenBalanceQuery;
struct Clause;
struct KeysClause;
struct MemberClause;
struct CompositeClause;
struct OrderBy;
struct TransactionFilter;


// ============================================================================
// Blockchain y Transacciones
// ============================================================================

struct Call;
struct Transaction;
struct TransactionCall;
struct BlockId;
struct Policy;
struct Event;


// ============================================================================
// Paginación y Páginas
// ============================================================================

struct Pagination;
struct PageController;
struct PageEntity;
struct PageTransaction;
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
struct ResultPageController;
struct ResultPageEntity;
struct ResultPageTransaction;
struct ResultPageToken;
struct ResultPageTokenBalance;
struct ResultPageTokenCollection;
struct ResultWorld;
struct ResultFieldElement;
struct ResultCArrayFieldElement;
struct Resultbool;
struct Resultc_char;
struct ResultSignature;


// ============================================================================
// Arrays de C
// ============================================================================

struct CArrayFieldElement;
struct CArrayController;
struct CArrayOrderBy;
struct CArrayc_char;
struct CArrayEntity;
struct CArrayCOptionFieldElement;
struct CArrayMemberValue;
struct CArrayClause;
struct CArrayModel;
struct CArrayTransaction;
struct CArrayTransactionCall;
struct CArrayStruct;
struct CArrayToken;
struct CArrayU256;
struct CArrayTokenBalance;
struct CArrayTokenCollection;
struct CArrayMember;
struct CArrayEnumOption;
struct CArrayTy;


// ============================================================================
// Tipos Opcionales
// ============================================================================

struct COptionFieldElement;
struct COptionc_char;
struct COptionu32;
struct COptionu64;
struct COptionClause;
struct COptionTransactionFilter;
struct COptionU256;


// ============================================================================
// Manejo de Errores
// ============================================================================

struct Error;

} // namespace dojo_bindings

#endif // DOJO_TYPES_H