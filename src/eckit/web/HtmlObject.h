/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File HtmlObject.h
// Baudouin Raoult - ECMWF Oct 96

#ifndef HtmlObject_H
#define HtmlObject_H

#include "eckit/persist/Bless.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Url;
class JavaAgent;
class Bless;

class HtmlObject {
public:

    HtmlObject();

#include "eckit/web/HtmlObject.b"

    virtual ~HtmlObject();

    virtual void java(JavaAgent&);
    virtual void substitute(std::ostream&, const std::string&);

    virtual void HEAD(std::ostream&, Url&);
    virtual void GET(std::ostream&, Url&);
    virtual void POST(std::ostream&, Url&);
    virtual void PUT(std::ostream&, Url&);
    virtual void DELETE(std::ostream&, Url&);
    virtual void TRACE(std::ostream&, Url&);
    virtual void OPTIONS(std::ostream&, Url&);
    virtual void CONNECT(std::ostream&, Url&);
    virtual void PATCH(std::ostream&, Url&);

protected:

    virtual void html(std::ostream&, eckit::Url&);

    virtual void print(std::ostream&) const;

private:

    friend std::ostream& operator<<(std::ostream& s, const HtmlObject& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
