// Traits classes and helper meta-functions used to classify and characterize
// rules and parsers.
//
#ifndef HITTOP_PARSER_TRAITS_H
#define HITTOP_PARSER_TRAITS_H

#include <type_traits>

namespace hittop {
namespace parser {

// Meta-function; true if predicate is true for all of the following types.
//
template <template <typename> class Predicate, typename... T> struct TrueForAll;

template <template <typename> class Predicate>
struct TrueForAll<Predicate> : std::true_type {};

template <template <typename> class Predicate, typename T>
struct TrueForAll<Predicate, T>
    : std::integral_constant<bool, Predicate<T>::value> {};

template <template <typename> class Predicate, typename T, typename... U>
struct TrueForAll<Predicate, T, U...>
    : std::integral_constant<bool, Predicate<T>::value &&
                                       TrueForAll<Predicate, U...>::value> {};

// Meta-function; true if predicate is true for any of the following types.
//
template <template <typename> class Predicate, typename... T> struct TrueForAny;

template <template <typename> class Predicate>
struct TrueForAny<Predicate> : std::false_type {};

template <template <typename> class Predicate, typename T>
struct TrueForAny<Predicate, T>
    : std::integral_constant<bool, Predicate<T>::value> {};

template <template <typename> class Predicate, typename T, typename... U>
struct TrueForAny<Predicate, T, U...>
    : std::integral_constant<bool, Predicate<T>::value ||
                                       TrueForAny<Predicate, U...>::value> {};

// Used to tag grammar constructs that accept a single char on success.

template <typename T> struct IsSingleCharRule : std::false_type {};

template <typename> struct SingleArgType;
template <typename T> struct SingleArgType<void(T)> { using type = T; };

} // namespace parser
} // namespace hittop

#endif // HITTOP_PARSER_TRAITS_H
