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
