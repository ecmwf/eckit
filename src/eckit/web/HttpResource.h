// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult

#ifndef eckit_web_HttpResource_H
#define eckit_web_HttpResource_H

#include <string>

#include "eckit/web/HtmlObject.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Stream;
class HttpStream;

class HttpResource : public HtmlObject {
public:  // methods

    HttpResource(const std::string&);

    HttpResource(const HttpResource&)            = delete;
    HttpResource& operator=(const HttpResource&) = delete;
    HttpResource(HttpResource&&)                 = delete;
    HttpResource& operator=(HttpResource&&)      = delete;

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
