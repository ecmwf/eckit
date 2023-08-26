/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/geo/util/regex.h"

#include <map>

#include "eckit/log/Log.h"


namespace eckit::geo::util {


regex_match_type regex_match(const std::string& pattern, const std::string& s) {
    static const std::map<std::regex_constants::error_type, std::string> __regex_code{
        {std::regex_constants::error_backref, "error_backref"},
        {std::regex_constants::error_badbrace, "error_badbrace"},
        {std::regex_constants::error_badrepeat, "error_badrepeat"},
        {std::regex_constants::error_brace, "error_brace"},
        {std::regex_constants::error_brack, "error_brack"},
        {std::regex_constants::error_collate, "error_collate"},
        {std::regex_constants::error_complexity, "error_complexity"},
        {std::regex_constants::error_ctype, "error_ctype"},
        {std::regex_constants::error_escape, "error_escape"},
        {std::regex_constants::error_paren, "error_paren"},
        {std::regex_constants::error_range, "error_range"},
        {std::regex_constants::error_space, "error_space"},
        {std::regex_constants::error_stack, "error_stack"},
    };

    std::regex regex;
    try {
        regex.assign(std::regex(pattern, std::regex_constants::extended));
    }
    catch (const std::regex_error& e) {
        Log::error() << "regex_error caught: " << e.what() << ", code: " << __regex_code.at(e.code()) << std::endl;
        throw;
    }

    regex_match_type m;
    std::regex_match(s, m, regex);
    return m;
}


}  // namespace eckit::geo::util
