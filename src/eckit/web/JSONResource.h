// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
///
/// @date Jun 2012

#ifndef eckit_web_JSONResource_H
#define eckit_web_JSONResource_H

#include "eckit/web/HttpResource.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class JSON;
class Value;

class JSONResource : public HttpResource {
public:

    JSONResource(const std::string&);

    ~JSONResource() override;

private:

    void GET(std::ostream&, Url&) override;
    void POST(std::ostream&, Url&) override;
    virtual void json(eckit::JSON&, const eckit::Value&) = 0;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
