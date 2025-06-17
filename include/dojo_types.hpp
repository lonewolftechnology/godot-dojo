// dojo.hpp
#ifndef DOJO_TYPES_HPP
#define DOJO_TYPES_HPP

#include "dojo.hpp"
#define DOJO dojo_bindings
#define GET_DOJO_VALUE(obj) ((obj)._0)
#define SET_DOJO_VALUE(target, value) ((target)._0 = value)

#define GET_DOJO_ERROR(result) ((result).err._0.message)
#define GET_DOJO_OK(result) ((result).ok._0)
#define GET_DOJO_OK_PTR(result) ((&result).ok._0)

namespace dojo_bindings {

// Forward declarations de clases y estructuras:
struct ToriiClient;
struct Policy;
struct ControllerAccount;
struct Call;
struct Ty;
struct Subscription;
struct Provider;
struct Account;

// Declaraciones anticipadas de los enums:
enum class BlockTag;
enum class ComparisonOperator;
enum class LogicalOperator;
enum class OrderDirection;
enum class PaginationDirection;
enum class PatternMatching;

// Más estructuras y clases:
struct Error;
struct FieldElement;
struct Controller;
struct Member;
struct Struct;
struct Entity;
struct OrderBy;
struct Pagination;
struct KeysClause;
struct U256;
struct Primitive;
struct MemberValue;
struct MemberClause;
struct CompositeClause;
struct Clause;
struct Query;
struct EnumOption;
struct Enum;
struct ModelMetadata;
struct WorldMetadata;
struct Event;
struct Token;
struct TokenBalance;
struct TokenCollection;
struct IndexerUpdate;
struct Signature;
struct BlockId;

// Declaraciones de plantillas de clases genéricas:
template<typename T>
struct Result;

template<typename T>
struct CArray;

template<typename T>
struct COption;

template<typename K, typename V>
struct CHashItem;

template<typename T>
struct Page;

} // namespace dojo_bindings

#endif // DOJO_TYPES_HPP