// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_web_HttpStream_H
#define eckit_web_HttpStream_H

#include "eckit/web/Url.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class HttpBuf;

class HttpStream : public std::ostream {
public:

    HttpStream();
    ~HttpStream();

    void reset();
    void write(std::ostream&, Url&, DataHandle&);

    void print(std::ostream& s) const;

    static std::ostream& dontEncode(std::ostream&);
    static std::ostream& doEncode(std::ostream&);

private:

    HttpBuf* buf_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
