// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/web/HtmlResource.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

HtmlResource::HtmlResource(const std::string& s) : HttpResource(s) {}

HtmlResource::~HtmlResource() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
