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

#ifndef eckit_web_HttpResource_H
#define eckit_web_HttpResource_H

#include <string>

#include "eckit/memory/NonCopyable.h"
#include "eckit/web/HtmlObject.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;
class HttpStream;

class HttpResource : public HtmlObject, public eckit::NonCopyable {
public:  // methods

    HttpResource(const std::string&);

    ~HttpResource() override;

    virtual bool restricted() { return false; }

    static void dispatch(eckit::Stream&, std::istream&, HttpStream&, Url&);
    static void index(std::ostream&, Url&);

    const std::string& resourceUrl() const;

protected:  // methods

    void print(std::ostream&) const override;

protected:  // members

    const std::string resourceUrl_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
