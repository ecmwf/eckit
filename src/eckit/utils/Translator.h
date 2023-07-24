/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Translator.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Jun 1996

#ifndef eckit_Translator_h
#define eckit_Translator_h

#include <set>
#include <string>
#include <type_traits>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


// Translate a type from From to To

template <class From, class To>
struct Translator {
    // To allow using IsTranslatable with SFINAE it is important the the operator is templated and has a `auto` return type
    // C++14 version would be with enable_if_t and decay_t. FDB5 seems not to have migrated to C++17 compilation yet.

#if __cplusplus >= 201402L
    // Test for explicit conversion through constructor (also involves implicit conversion or or user-defined conversion operator).
    // Note: To(from) is called with () brackets and not with {} because it includes conversion of plain datatypes - MIR is using this
    template <typename F, std::enable_if_t<
                              (!std::is_same<std::decay_t<F>, To>::value && (std::is_same<std::decay_t<decltype(To(std::declval<F>()))>, To>::value)),
                              bool> = true>
    auto operator()(F&& from) {
        return To(std::forward<F>(from));
    }

    // If from and to type are same - simply forward - i.e. allow moving or passing references instead of performing copies
    template <typename F, std::enable_if_t<
                              (std::is_same<std::decay_t<F>, To>::value && !(std::is_lvalue_reference<F>::value && !std::is_const<F>::value)),
                              bool> = true>
    decltype(auto) operator()(F&& from) {
        return std::forward<F>(from);
    }
    
    // If mutable references are passed, a const ref is returened to avoid modifications downstream...
    template <typename F, std::enable_if_t<
                              (std::is_same<std::decay_t<F>, To>::value && (std::is_lvalue_reference<F>::value && !std::is_const<F>::value)),
                              bool> = true>
    const To& operator()(F&& from) {
        return const_cast<const To&>(from);
    }
#else
    // If conversion is possible
    template <typename F, typename std::enable_if<
                              (std::is_same<typename std::decay<decltype(To(std::declval<F>()))>::type, To>::value),
                              bool>::type
                          = true>
    To operator()(F&& from) {
        return To(std::forward<F>(from));
    }
#endif
};

// Those are predefined

template <>
struct Translator<bool, std::string> {
    std::string operator()(bool);
};


template <>
struct Translator<std::string, bool> {
    bool operator()(const std::string&);
};


template <>
struct Translator<unsigned char, std::string> {
    std::string operator()(unsigned char);
};

template <>
struct Translator<float, std::string> {
    std::string operator()(float);
};

template <>
struct Translator<short, std::string> {
    std::string operator()(short);
};

template <>
struct Translator<int, std::string> {
    std::string operator()(int);
};

template <>
struct Translator<unsigned int, std::string> {
    std::string operator()(unsigned int);
};

template <>
struct Translator<std::string, int> {
    int operator()(const std::string&);
};

template <>
struct Translator<std::string, unsigned int> {
    unsigned int operator()(const std::string&);
};

template <>
struct Translator<double, std::string> {
    std::string operator()(double);
};

template <>
struct Translator<std::string, double> {
    double operator()(const std::string&);
};

template <>
struct Translator<std::string, float> {
    float operator()(const std::string&);
};

template <>
struct Translator<long, std::string> {
    std::string operator()(long);
};

template <>
struct Translator<std::string, long> {
    long operator()(const std::string&);
};

template <>
struct Translator<std::string, short> {
    short operator()(const std::string&);
};

template <>
struct Translator<unsigned long, std::string> {
    std::string operator()(unsigned long);
};

template <>
struct Translator<std::string, unsigned long> {
    unsigned long operator()(const std::string&);
};

template <>
struct Translator<std::string, unsigned char> {
    unsigned char operator()(const std::string&);
};

template <>
struct Translator<std::string, unsigned long long> {
    unsigned long long operator()(const std::string&);
};

template <>
struct Translator<std::string, long long> {
    long long operator()(const std::string&);
};

template <>
struct Translator<unsigned long long, std::string> {
    std::string operator()(unsigned long long);
};

template <>
struct Translator<long long, std::string> {
    std::string operator()(long long);
};

template <>
struct Translator<std::string, char> {
    char operator()(const std::string&);
};

template <>
struct Translator<char, std::string> {
    std::string operator()(char);
};

template <>
struct Translator<std::string, std::vector<std::string>> {
    std::vector<std::string> operator()(const std::string&);
};

template <>
struct Translator<std::string, std::vector<long>> {
    std::vector<long> operator()(const std::string&);
};

template <>
struct Translator<std::vector<long>, std::string> {
    std::string operator()(const std::vector<long>&);
};

template <>
struct Translator<std::vector<std::string>, std::string> {
    std::string operator()(const std::vector<std::string>&);
};

template <>
struct Translator<std::string, std::set<std::string>> {
    std::set<std::string> operator()(const std::string&);
};

template <>
struct Translator<std::set<std::string>, std::string> {
    std::string operator()(const std::set<std::string>&);
};

template <>
struct Translator<signed char, std::string> {
    std::string operator()(signed char v) { return Translator<int, std::string>{}(static_cast<int>(v)); };
};


//----------------------------------------------------------------------------------------------------------------------

// This allows using the Translator without having to explicitly name the type of an argument. For example in case of
// generic string conversion: translate<std::strig>(someVariable)
#if __cplusplus >= 201402L
template <typename To, typename From>
decltype(auto) translate(From&& from) {
    return Translator<typename std::decay<From>::type, To>{}(std::forward<From>(from));
}
#else
template <typename To, typename From>
To translate(From&& from) {
    return Translator<typename std::decay<From>::type, To>{}(std::forward<From>(from));
}
#endif

//----------------------------------------------------------------------------------------------------------------------

#if __cplusplus >= 201703L

// primary template handles types that do not support pre-increment:
template <typename From, typename To, class = void>
struct IsTranslatable : std::false_type {};

// specialization recognizes types that do support pre-increment:
template <typename From, typename To>
struct IsTranslatable<From, To,
                      std::void_t<decltype(Translator<From, To>{}(std::declval<const From&>()))>> : std::true_type {};

#endif

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
