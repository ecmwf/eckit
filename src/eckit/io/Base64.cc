/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Base64.h"
#include "eckit/io/BitIO.h"
#include "eckit/io/DataHandle.h"

// This code is written for readibility, not speed


namespace eckit {


static const char* codes_b64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static const char* codes_url =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789-_";
Base64::Base64(bool url) {
    const char* p = url ? codes_url : codes_b64;

    size_t i = 0;
    while (*p) {
        size_t j   = *p;
        encode_[i] = *p;
        decode_[j] = i;
        p++;
        i++;
    }
}


size_t Base64::encode(DataHandle& in, DataHandle& out) {
    const size_t EOI_MARKER = 256;

    BitIO bin(in, true);
    BitIO bout(out);

    size_t c;

    while ((c = bin.read(6, EOI_MARKER)) != EOI_MARKER) {
        bout.write(encode_[c], 8);
    }

    switch (bin.bitCount() % 6) {
        case 0:  // No padding
            break;

        case 2:
            bout.write('=', 8);
            bout.write('=', 8);
            break;

        case 4:
            bout.write('=', 8);
            break;

        default: {
            std::ostringstream oss;
            oss << "Base64: invalid padding: " << (bin.bitCount() % 6);
            throw eckit::SeriousBug(oss.str());
        }
    }

    return bout.byteCount();
}
//----------------------------------------------------------------------------------------------------------------------

size_t Base64::decode(DataHandle& in, DataHandle& out) {
    const size_t EOI_MARKER = 256;

    BitIO bin(in);
    BitIO bout(out);

    size_t c    = bin.read(8, EOI_MARKER);
    size_t prev = EOI_MARKER;

    while (c != EOI_MARKER) {
        if (c == '=') {
            size_t left = 8 - (bout.bitCount() % 8);
            bout.write(decode_[prev] >> (6 - left), left);
            prev = EOI_MARKER;
            break;
        }

        if (prev != EOI_MARKER) {
            bout.write(decode_[prev], 6);
        }

        prev = c;
        c    = bin.read(8, EOI_MARKER);
    }

    if (prev != EOI_MARKER) {
        bout.write(decode_[prev], 6);
    }

    return bout.byteCount();
}
//----------------------------------------------------------------------------------------------------------------------
}  // namespace eckit
