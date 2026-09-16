// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
