/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File JavaAgent.h
// Baudouin Raoult - ECMWF Nov 97

#ifndef JavaAgent_H
#define JavaAgent_H

#include "eckit/serialisation/Streamable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


class JavaAgent : public eckit::Streamable {
public:

    enum {
        none,
        user,
        oper,
        admin,
        root
    };

    // -- Contructors

    JavaAgent(eckit::Stream&);

    // -- Destructor

    ~JavaAgent() override;

    // -- Methods

    void startObject(const std::string&);
    void endObject();
    eckit::Stream& stream() { return stream_; }

    virtual void execute(eckit::Stream&, std::istream&, std::ostream&) = 0;
    virtual int clearance()                                            = 0;

    // -- Overridden methods

    void encode(eckit::Stream&) const override;
    const eckit::ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const eckit::ClassSpec& classSpec() { return classSpec_; }
    static void serve(eckit::Stream&, std::istream&, std::ostream&);

protected:  // members

    eckit::Stream& stream_;
    std::string user_;

protected:  // methods

    virtual void print(std::ostream&) const = 0;

private:  // members

    static eckit::ClassSpec classSpec_;
    static eckit::Reanimator<JavaAgent> reanimator_;

private:  // methods

    friend std::ostream& operator<<(std::ostream& s, const JavaAgent& p) {
        p.print(s);
        return s;
    }

    friend class JavaUser;
};

template <>
Streamable* Reanimator<JavaAgent>::ressucitate(Stream& s) const;

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
