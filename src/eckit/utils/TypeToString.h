/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

#include <string_view>

/// This file provides utilities to convert types to readable strings
///
/// > IMPORTANT: These utilities are intended to be used for log and exception
/// > messages. Do not use them to carry type information and build logic on top.


namespace eckit {

/// Very common and accepted approach: https://stackoverflow.com/a/56766138/576911
/// Demo: https://godbolt.org/z/Mr1ercr49
/// Works for Clang, GCC, ICC, NVC++
/// Only types like `std::string` usually get printed as `std::basic_string<char>` or `std::__cxx11::basic_string<char,
/// std::char_traits<char>, std::allocator<char>>` For this reason a wrapper `TypeToString` is presented.
template <typename T>
constexpr auto typeName() {
    std::string_view name, prefix, suffix;
#ifdef __clang__
    name   = __PRETTY_FUNCTION__;
    prefix = "auto eckit::typeName() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name   = __PRETTY_FUNCTION__;
    prefix = "constexpr auto eckit::typeName() [with T = ";
    suffix = "]";
#elif defined(_MSC_VER)
    name   = __FUNCSIG__;
    prefix = "auto __cdecl eckit::typeName<";
    suffix = ">(void)";
#endif
    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());
    return name;
}

/// Template class to customize string representation for specific types for *log and error messages*.
/// By default it uses `typeName<T>()` - but in some cases a prettier output is desired (e.g. std::string)
///
/// A few template `std` types are specialized to pretty print the template arguments.
///
/// Instead of using a string_view, a call operator is used at runtime that allows concatenation of strings.
/// With C++20 all could be written with constexpr strings.
template <typename T>
struct TypeToString {
    std::string operator()() const { return std::string(typeName<T>()); };
};


template <typename T>
std::string typeToString() {
    return TypeToString<T>{}();
}

template <typename T>
struct TypeToString<const T> {
    std::string operator()() const { return std::string("const ") + typeToString<std::remove_cv_t<T>>(); };
};

template <typename T>
struct TypeToString<T&> {
    std::string operator()() const { return typeToString<std::remove_reference_t<T>>() + std::string("&"); };
};
template <typename T>
struct TypeToString<T&&> {
    std::string operator()() const { return typeToString<std::remove_reference_t<T>>() + std::string("&&"); };
};

template <>
struct TypeToString<std::string> {
    std::string operator()() const { return "std::string"; };
};

template <typename T>
struct TypeToString<std::optional<T>> {
    std::string operator()() const { return std::string("std::optional<") + typeToString<T>() + std::string(">"); };
};

template <typename T, typename Alloc>
struct TypeToString<std::vector<T, Alloc>> {
    std::string operator()() const { return std::string("std::vector<") + typeToString<T>() + std::string(">"); };
};

template <typename... T>
struct TypeToString<std::tuple<T...>> {
    std::string operator()() const {
        return std::string("std::tuple<") + ((typeToString<T>() + std::string(", ")) + ... + std::string(">"));
    };
};

template <typename... T>
struct TypeToString<std::variant<T...>> {
    std::string operator()() const {
        return std::string("std::variant<") + ((typeToString<T>() + std::string(", ")) + ... + std::string(">"));
    };
};

template <typename T>
struct TypeToString<std::reference_wrapper<T>> {
    std::string operator()() const {
        return std::string("std::reference_wrapper<") + typeToString<T>() + std::string(">");
    };
};

template <typename T, typename Deleter>
struct TypeToString<std::unique_ptr<T, Deleter>> {
    std::string operator()() const {
        return std::string("std::unique_ptr<") + typeToString<T>() + std::string(", Deleter>");
    };
};

template <typename T>
struct TypeToString<std::shared_ptr<T>> {
    std::string operator()() const { return std::string("std::shared_ptr<") + typeToString<T>() + std::string(">"); };
};


//-----------------------------------------------------------------------------


}  // namespace eckit
