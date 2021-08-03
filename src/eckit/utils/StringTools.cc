/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cctype>
#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/StringTools.h"
#include "eckit/utils/Tokenizer.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


std::string StringTools::substitute(const std::string& s, const std::map<std::string, std::string>& m) {
    std::string result;
    size_t len = s.length();
    bool var   = false;
    std::string word;
    std::map<std::string, std::string>::const_iterator j;

    for (size_t i = 0; i < len; i++) {
        switch (s[i]) {
            case '{':
                if (var) {
                    std::ostringstream os;
                    os << "StringTools::substitute: unexpected { found in " << s << " at position " << i;
                    throw UserError(os.str());
                }
                var  = true;
                word = "";
                break;

            case '}':
                if (!var) {
                    std::ostringstream os;
                    os << "StringTools::substitute: unexpected } found in " << s << " at position " << i;
                    throw UserError(os.str());
                }
                var = false;

                j = m.find(word);
                if (j == m.end()) {
                    std::ostringstream os;
                    os << "StringTools::substitute: cannot find a value for '" << word << "' in " << s
                       << " at position " << i;
                    throw UserError(os.str());
                }
                result += (*j).second;
                break;

            default:
                if (var)
                    word += s[i];
                else
                    result += s[i];
                break;
        }
    }
    if (var) {
        std::ostringstream os;
        os << "StringTools::substitute: missing } in " << s;
        throw UserError(os.str());
    }
    return result;
}

std::vector<std::string> StringTools::substituteVariables(const std::string& s) {
    return listVariables(s);
}

std::vector<std::string> StringTools::listVariables(const std::string& s) {
    std::vector<std::string> result;
    size_t len = s.length();
    bool var   = false;
    std::string word;

    for (size_t i = 0; i < len; i++) {
        switch (s[i]) {
            case '{':
                if (var) {
                    std::ostringstream os;
                    os << "StringTools::substituteVariables: unexpected { found in " << s << " at position " << i;
                    throw UserError(os.str());
                }
                var  = true;
                word = "";
                break;

            case '}':
                if (!var) {
                    std::ostringstream os;
                    os << "StringTools::substituteVariables: unexpected } found in " << s << " at position " << i;
                    throw UserError(os.str());
                }
                var = false;
                result.push_back(word);
                break;

            default:
                if (var)
                    word += s[i];
                break;
        }
    }
    if (var) {
        std::ostringstream os;
        os << "StringTools::substituteVariables: missing } in " << s;
        throw UserError(os.str());
    }
    return result;
}

std::string StringTools::upper(const std::string& v) {
    std::string r = v;
    std::transform(r.begin(), r.end(), r.begin(), static_cast<int (*)(int)>(toupper));
    return r;
}

std::string StringTools::lower(const std::string& v) {
    std::string r = v;
    std::transform(r.begin(), r.end(), r.begin(), static_cast<int (*)(int)>(tolower));
    return r;
}

std::string StringTools::trim(const std::string& str) {
    return trim(str, " \t\n");
}

std::string StringTools::trim(const std::string& str, const std::string& chars) {
    size_t startpos = str.find_first_not_of(chars);
    size_t endpos   = str.find_last_not_of(chars);

    if ((std::string::npos == startpos) || (std::string::npos == endpos))
        return "";
    else
        return str.substr(startpos, endpos - startpos + 1);
}

std::string StringTools::front_trim(const std::string& str) {
    return front_trim(str, " \t");
}

std::string StringTools::front_trim(const std::string& str, const std::string& chars) {
    size_t startpos = str.find_first_not_of(chars);

    if (std::string::npos == startpos)
        return "";
    else
        return str.substr(startpos);
}

std::string StringTools::back_trim(const std::string& str) {
    return back_trim(str, " \t");
}

std::string StringTools::back_trim(const std::string& str, const std::string& chars) {
    size_t endpos = str.find_last_not_of(chars);

    if (std::string::npos == endpos)
        return "";
    else
        return str.substr(0, endpos + 1);
}

std::vector<std::string> StringTools::split(const std::string& delim, const std::string& text) {
    std::vector<std::string> ss;
    Tokenizer tokenizer(delim);
    tokenizer(text, ss);
    return ss;
}

bool StringTools::startsWith(const std::string& str, const std::string& substr) {
    if (substr.empty() || str.size() < substr.size())
        return false;

    for (std::string::size_type i = 0; i < substr.size(); ++i)
        if (substr[i] != str[i])
            return false;

    return true;
}

bool StringTools::beginsWith(const std::string& str, const std::string& substr) {
    return startsWith(str, substr);
}

bool StringTools::endsWith(const std::string& str, const std::string& substr) {
    if (substr.empty() || str.size() < substr.size())
        return false;

    std::string::const_reverse_iterator rj = str.rbegin();
    for (std::string::const_reverse_iterator ri = substr.rbegin(); ri != substr.rend(); ++ri, ++rj) {
        if (*ri != *rj)
            return false;
    }

    return true;
}

bool StringTools::isQuoted(const std::string& value) {
    return value.size() > 1 && ((value[0] == '"' && value[value.size() - 1] == '"') || (value[0] == '\'' && value[value.size() - 1] == '\''));
}

std::string StringTools::unQuote(const std::string& value) {
    if (isQuoted(value)) {
        return value.substr(1, value.size() - 2);
    }
    return value;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
