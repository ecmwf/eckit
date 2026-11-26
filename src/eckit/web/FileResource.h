// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   September 1997

#ifndef eckit_web_FileResource_H
#define eckit_web_FileResource_H

#include "eckit/web/HtmlResource.h"


namespace eckit {


class FileResource : public HtmlResource {
public:

    FileResource();

    ~FileResource() override;

protected:

    std::string name_;

    void GET(std::ostream&, Url&) override;
};


}  // namespace eckit


#endif
