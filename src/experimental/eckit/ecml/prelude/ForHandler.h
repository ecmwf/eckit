/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, December 2015

#ifndef eckit_ecml_ForHandler_H
#define eckit_ecml_ForHandler_H

#include "eckit/eckit_config.h"

#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/SpecialFormHandler.h"
#include "experimental/eckit/ecml/core/ExecutionContext.h"

namespace eckit {

class ForHandler : public eckit::SpecialFormHandler {
public:
    ForHandler(const std::string&);
    virtual eckit::Request handle(const eckit::Request, eckit::ExecutionContext&);

    static void repl(ExecutionContext&);
};

} // namespace eckit


#endif
