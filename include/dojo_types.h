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
// Structs
// ============================================================================

struct ToriiClient;
struct Policy;
struct ControllerAccount;
struct Call;
struct Controller;
struct OrderBy;
struct Entity;
struct COptionFieldElement;
struct Model;
struct Transaction;
struct Subscription;
struct TransactionCall;
struct Struct;
struct Token;
struct TokenBalance;
struct TokenCollection;
struct Provider;
struct Account;
struct Ty;
struct Member;
struct EnumOption;
struct Error;
struct ResultToriiClient;
struct FieldElement;
struct ResultControllerAccount;
struct Resultbool;
struct ResultFieldElement;
struct CArrayFieldElement;
struct Message;
struct ResultCArrayFieldElement;
struct CArrayController;
struct COptionc_char;
struct PageController;
struct ResultPageController;
struct COptionu32;
struct CArrayOrderBy;
struct Pagination;
struct CArrayc_char;
struct ControllerQuery;
struct CArrayEntity;
struct PageEntity;
struct ResultPageEntity;
struct CArrayCOptionFieldElement;
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
struct ResultWorld;
struct CArrayTransaction;
struct PageTransaction;
struct ResultPageTransaction;
struct COptionu64;
struct TransactionFilter;
struct COptionTransactionFilter;
struct TransactionQuery;
struct ResultSubscription;
struct CArrayTransactionCall;
struct CArrayStruct;
struct Event;
struct CArrayToken;
struct PageToken;
struct ResultPageToken;
struct CArrayU256;
struct TokenQuery;
struct COptionU256;
struct CArrayTokenBalance;
struct PageTokenBalance;
struct ResultPageTokenBalance;
struct TokenBalanceQuery;
struct CArrayTokenCollection;
struct PageTokenCollection;
struct ResultPageTokenCollection;
struct IndexerUpdate;
struct Resultc_char;
struct Signature;
struct ResultSignature;
struct ResultProvider;
struct ResultAccount;
struct BlockId;
struct CArrayMember;
struct CArrayEnumOption;
struct Enum;
struct CArrayTy;
struct FixedSizeArray;

// ============================================================================
// Enums
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

// Enum Tags
enum ResultToriiClient_Tag;
enum ResultControllerAccount_Tag;
enum Resultbool_Tag;
enum ResultFieldElement_Tag;
enum ResultCArrayFieldElement_Tag;
enum COptionc_char_Tag;
enum ResultPageController_Tag;
enum COptionu32_Tag;
enum ResultPageEntity_Tag;
enum COptionClause_Tag;
enum ResultWorld_Tag;
enum ResultPageTransaction_Tag;
enum COptionu64_Tag;
enum COptionTransactionFilter_Tag;
enum ResultSubscription_Tag;
enum ResultPageToken_Tag;
enum COptionU256_Tag;
enum ResultPageTokenBalance_Tag;
enum ResultPageTokenCollection_Tag;
enum Resultc_char_Tag;
enum ResultSignature_Tag;
enum ResultProvider_Tag;
enum ResultAccount_Tag;
enum COptionFieldElement_Tag;

} // namespace dojo_bindings

#endif // DOJO_TYPES_H
