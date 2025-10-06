/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   TermBuf.h
/// @author Manuel Fuentes
/// @date   Aug 1996

#ifndef eckit_cmd_TermBuf_H
#define eckit_cmd_TermBuf_H

#include <iosfwd>
#include <streambuf>

#include "eckit/eckit.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TermBuf : public std::streambuf {
public:

    // -- Contructors

    TermBuf(std::ostream&);

    // -- Destructor

    ~TermBuf();

    // -- Class Methods

    void init();
    void clear();
    void home();
    void clearEOL();
    void clearEOS();

private:

    // No copy allowed

    TermBuf(const TermBuf&);
    TermBuf& operator=(const TermBuf&);

    // -- Members

    char buffer_[1024];
    std::ostream& out_;

    // -- Methods

    virtual int overflow(int c);
    virtual int sync();
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
