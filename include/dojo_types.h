//
// Created by hazel on 26/05/25.
//
#ifndef DOJO_TYPES_H
#define DOJO_TYPES_H
#include "dojo.h" // 1.7.0-alpha.5

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

// ===========================================================================
// Structs
// ===========================================================================

struct Account;
struct BlockId;
struct Call;
struct CArrayClause;
struct CArrayCOptionFieldElement;
struct CArrayController;
struct CArrayc_char;
struct CArrayEntity;
struct CArrayEnumOption;
struct CArrayFieldElement;
struct CArrayMember;
struct CArrayMemberValue;
struct CArrayModel;
struct CArrayStruct;
struct CArrayToken;
struct CArrayTokenBalance;
struct CArrayTokenCollection;
struct CArrayTransaction;
struct CArrayTransactionCall;
struct CArrayTy;
struct CArrayU256;
struct Clause;
struct CompositeClause;
struct Controller;
struct ControllerAccount;
struct ControllerQuery;
struct COptionClause;
struct COptionc_char;
struct COptionFieldElement;
struct COptionTransactionFilter;
struct COptionu32;
struct COptionu64;
struct COptionU256;
struct Entity;
struct Enum;
struct EnumOption;
struct Error;
struct Event;
struct FieldElement;
struct FixedSizeArray;
struct IndexerUpdate;
struct KeysClause;
struct Member;
struct MemberClause;
struct MemberValue;
struct Message;
struct Model;
struct OrderBy;
struct PageController;
struct PageEntity;
struct PageToken;
struct PageTokenBalance;
struct PageTokenCollection;
struct PageTransaction;
struct Pagination;
struct Policy;
struct Primitive;
struct Provider;
struct Query;
struct ResultAccount;
struct Resultbool;
struct ResultCArrayFieldElement;
struct Resultc_char;
struct ResultControllerAccount;
struct ResultFieldElement;
struct ResultPageController;
struct ResultPageEntity;
struct ResultPageToken;
struct ResultPageTokenBalance;
struct ResultPageTokenCollection;
struct ResultPageTransaction;
struct ResultProvider;
struct ResultSignature;
struct ResultSubscription;
struct ResultToriiClient;
struct ResultWorld;
struct Signature;
struct Struct;
struct Subscription;
struct Token;
struct TokenBalance;
struct TokenBalanceQuery;
struct TokenCollection;
struct TokenQuery;
struct ToriiClient;
struct Transaction;
struct TransactionCall;
struct TransactionFilter;
struct TransactionQuery;
struct Ty;
struct U256;
struct World;

// ===========================================================================
// Enums
// ===========================================================================

enum BlockId_Tag;
enum BlockTag;
enum CallType;
enum Clause_Tag;
enum ComparisonOperator;
enum LogicalOperator;
enum MemberValue_Tag;
enum OrderDirection;
enum PaginationDirection;
enum PatternMatching;
enum Primitive_Tag;
enum Ty_Tag;

// Enum Tags
enum COptionClause_Tag;
enum COptionc_char_Tag;
enum COptionFieldElement_Tag;
enum COptionTransactionFilter_Tag;
enum COptionu32_Tag;
enum COptionu64_Tag;
enum COptionU256_Tag;
enum ResultAccount_Tag;
enum Resultbool_Tag;
enum ResultCArrayFieldElement_Tag;
enum Resultc_char_Tag;
enum ResultControllerAccount_Tag;
enum ResultFieldElement_Tag;
enum ResultPageController_Tag;
enum ResultPageEntity_Tag;
enum ResultPageToken_Tag;
enum ResultPageTokenBalance_Tag;
enum ResultPageTokenCollection_Tag;
enum ResultPageTransaction_Tag;
enum ResultProvider_Tag;
enum ResultSignature_Tag;
enum ResultSubscription_Tag;
enum ResultToriiClient_Tag;
enum ResultWorld_Tag;

} // namespace dojo_bindings

#endif // DOJO_TYPES_H
