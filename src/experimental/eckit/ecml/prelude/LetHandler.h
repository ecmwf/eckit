/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
/// @author Piotr Kuchta, February 2015

#ifndef eckit_ecml_LetHandler_H
#define eckit_ecml_LetHandler_H

#include "eckit/filesystem/PathName.h"
#include "experimental/eckit/ecml/parser/Request.h"
#include "experimental/eckit/ecml/core/SpecialFormHandler.h"

namespace eckit {

class ExecutionContext;

class LetHandler : public eckit::SpecialFormHandler {
public:
    LetHandler(const std::string&);

    virtual eckit::Request handle(const eckit::Request, eckit::ExecutionContext&);
};

} // namespace eckit

#endif
