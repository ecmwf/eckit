/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/io/Base64.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/BitIO.h"

// This code is written for readibility, not speed


namespace eckit {


static const char* codes_b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/";

static const char* codes_url = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789-_";
Base64::Base64(bool url) {
    const char *p = url ? codes_url : codes_b64;

    for (size_t i = 0; i < sizeof(encode_); i++) {
        encode_[i] = 0;
    }

    size_t i = 0;
    while (*p) {
        encode_[i] = *p;
        decode_[*p] = i;
        p++;
        i++;
    }
}


size_t Base64::encode(DataHandle& in, DataHandle& out)
{
    BitIO bin(in);
    BitIO bout(out);

    size_t nbits = 6;
    size_t c;

    while ((c = bin.read(nbits, 256)) != 256) {
        std::cout << c << " -> " << encode_[c] << std::endl;
        ASSERT(encode_[c]);
        bout.write(encode_[c], 8);
    }

    bout.flush();
    return bout.count();

}
//----------------------------------------------------------------------------------------------------------------------

size_t Base64::decode(DataHandle& in, DataHandle& out)
{

    BitIO bin(in);
    BitIO bout(out);

    size_t nbits = 8;
    size_t c;

    while ((c = bin.read(nbits, 256)) != 256) {
        bout.write(decode_[c], 6);
    }

    bout.flush();
    return bout.count();
}
//----------------------------------------------------------------------------------------------------------------------
} // namespace eckit
