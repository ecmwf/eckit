// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
