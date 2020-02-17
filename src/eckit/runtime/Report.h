/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   Jan 2020

#ifndef eckit_runtime_Report_h
#define eckit_runtime_Report_h

#include <string>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

class Report : public NonCopyable {
public:
    static bool active();

    static void monitor(const std::string& domain, const std::string& msg);

    static void meter(const std::string& domain, const long m);
    static void meter(const std::string& domain, const size_t m);
    static void meter(const std::string& domain, const double m);

};


} // namespace eckit

#endif
