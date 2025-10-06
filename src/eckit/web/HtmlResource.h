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

#ifndef eckit_web_HtmlResource_H
#define eckit_web_HtmlResource_H

#include <string>

#include "eckit/web/HttpResource.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Intermediate Class
/// Derive all resources that provide an Html content

class HtmlResource : public HttpResource {
public:  // methods

    HtmlResource(const std::string&);

    ~HtmlResource() override;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
