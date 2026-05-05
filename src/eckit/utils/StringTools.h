/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino

#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

namespace detail {

/// @brief Lazy ostream-formatter for a delimited sequence.
///
/// Produced by StringTools::joinOstream; written to an ostream with operator<<
/// and never materialised as a std::string. Holds a reference to the source
/// container and delimiter, so it must be consumed in the same full-expression
/// in which it was created (the typical `os << join(...)` pattern). It is
/// intentionally non-copyable to make stale references harder to write
/// accidentally.
template <typename Container>
class JoinOstreamProxy {
public:

    JoinOstreamProxy(const Container& container, std::string_view delimiter) :
        container_(container), delimiter_(delimiter) {}

    JoinOstreamProxy(const JoinOstreamProxy&)            = delete;
    JoinOstreamProxy& operator=(const JoinOstreamProxy&) = delete;
    JoinOstreamProxy(JoinOstreamProxy&&)                 = default;
    JoinOstreamProxy& operator=(JoinOstreamProxy&&)      = delete;

    friend std::ostream& operator<<(std::ostream& os, const JoinOstreamProxy& p) {
        bool first = true;
        for (const auto& e : p.container_) {
            if (!first) {
                os << p.delimiter_;
            }
            os << e;
            first = false;
        }
        return os;
    }

private:

    const Container& container_;
    std::string_view delimiter_;
};

}  // namespace detail

//----------------------------------------------------------------------------------------------------------------------


class StringTools {
public:

    StringTools() = delete;

    static std::string substitute(const std::string&, const std::map<std::string, std::string>&);

    static std::vector<std::string> listVariables(const std::string&);

    static std::string upper(const std::string&);
    static std::string lower(const std::string&);

    static std::string trim(const std::string&);
    static std::string trim(const std::string&, const std::string&);

    static std::string front_trim(const std::string&);
    static std::string front_trim(const std::string&, const std::string&);

    static std::string back_trim(const std::string&);
    static std::string back_trim(const std::string&, const std::string&);

    static std::vector<std::string> split(const std::string& delim, const std::string& text);

    template <typename T>
    static std::string join(const std::string&, const T&);

    template <typename Iterator>
    static std::string join(const std::string&, Iterator begin, Iterator end);

    /// @brief Stream a delimited sequence directly to an ostream without building a temporary string.
    ///
    /// Returns a lightweight proxy that, when streamed, walks @p container and writes each
    /// element with `operator<<` separated by @p delimiter. The proxy holds references to
    /// both arguments and must be used in the same full-expression in which it was created;
    /// do not store it. Typical use:
    ///
    ///     Log::error() << "values: [" << StringTools::joinOstream(values, ", ") << "]";
    ///
    /// @tparam Container any range-iterable container whose elements have `operator<<(ostream&, ...)`.
    template <typename Container>
    static detail::JoinOstreamProxy<Container> joinOstream(const Container& container, std::string_view delimiter) {
        return detail::JoinOstreamProxy<Container>(container, delimiter);
    }

    static bool startsWith(const std::string& str, const std::string& substr);
    static bool beginsWith(const std::string& str, const std::string& substr);
    static bool endsWith(const std::string& str, const std::string& substr);

    static bool isQuoted(const std::string& value);
    static std::string unQuote(const std::string& value);
};

//----------------------------------------------------------------------------------------------------------------------

template <typename Iterator>
std::string StringTools::join(const std::string& delimiter, Iterator begin, Iterator end) {
    if (begin == end) {
        return "";
    }
    std::string r(*begin);
    for (Iterator it = ++begin; it != end; ++it) {
        r += delimiter;
        r += *it;
    }
    return r;
}

template <typename T>
std::string StringTools::join(const std::string& delimiter, const T& words) {
    return join(delimiter, words.begin(), words.end());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
