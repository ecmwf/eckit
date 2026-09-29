// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File CgiResource.h
// Baudouin Raoult - ECMWF Sep 97

#ifndef CgiResource_H
#define CgiResource_H

#include "eckit/web/HtmlResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class CgiResource : public HtmlResource {
public:

    CgiResource();

    ~CgiResource() override;

protected:  // members

    std::string name_;

protected:  // overridden methods

    void GET(std::ostream&, Url&) override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
