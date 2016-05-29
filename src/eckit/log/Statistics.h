/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   Statistics.h
/// @author Baudouin Raoult
/// @date   April 2016

#ifndef eckit_Statistics_H
#define eckit_Statistics_H

#include <iosfwd>

#include "eckit/io/Length.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Statistics {
public:
    static void reportCount(std::ostream& out, const char* title, size_t value, const char* indent = "");
    static void reportBytes(std::ostream& out, const char* title, eckit::Length value, const char* indent = "");
    static void reportTime(std::ostream& out, const char* title, double value, const char* indent = "");

};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
