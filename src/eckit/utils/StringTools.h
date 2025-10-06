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

#ifndef eckit_StringTools_h
#define eckit_StringTools_h


#include <map>
#include <string>
#include <vector>

#include "eckit/memory/NonCopyable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


class StringTools : private NonCopyable {
public:

    static std::string substitute(const std::string&, const std::map<std::string, std::string>&);

    /// @deprecated Use extract variables
    static std::vector<std::string> substituteVariables(const std::string&);

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

    static bool startsWith(const std::string& str, const std::string& substr);
    static bool beginsWith(const std::string& str, const std::string& substr);
    static bool endsWith(const std::string& str, const std::string& substr);

    static bool isQuoted(const std::string& value);
    static std::string unQuote(const std::string& value);

private:

    StringTools();  // Non instantiable
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

#endif
