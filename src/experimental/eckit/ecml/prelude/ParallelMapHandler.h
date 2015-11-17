/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, August 2015

#ifndef eckit_ecml_ParallelMapHandler_H
#define eckit_ecml_ParallelMapHandler_H

#include <sstream>

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/RequestHandler.h"

namespace eckit {

class ParallelMapHandler : public eckit::RequestHandler {
public:
    ParallelMapHandler(const std::string&);
    virtual eckit::Values handle(eckit::ExecutionContext&);

private:
    eckit::Values apply(eckit::ExecutionContext&, eckit::Cell* closure, eckit::Cell* values);
};

} // namespace eckit

#endif
