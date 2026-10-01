// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File AgentResource.h
// Baudouin Raoult - ECMWF Sep 97

#ifndef AgentResource_H
#define AgentResource_H

#include "eckit/web/HtmlResource.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class AgentResource : public HtmlResource {
public:

    AgentResource();

    ~AgentResource() override;

protected:  // members

    std::string name_;


protected:  // overridden methods

    void GET(std::ostream&, Url&) override;
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
