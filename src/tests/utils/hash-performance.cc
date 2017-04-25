/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/log/Timer.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"

#include "eckit/utils/SHA1.h"
#include "eckit/utils/MD5.h"
#include "eckit/utils/MD4.h"
#include "eckit/utils/xxHash.h"

using namespace std;
using namespace eckit;

template <class HASH, int N, int M>
void incremental(const char* name, eckit::Buffer& buffer, eckit::Timer& timer) {

    timer.start();

    for(int i = 0; i < N; ++i ) {

        HASH hash;
        for(size_t j=0; j < M; ++j) {
            hash.add(buffer, buffer.size());
        }
        hash.digest();
    }

    timer.stop();

    std::cout << name << " rate " << Bytes(N*M*buffer.size(), timer) << std::endl;

}


int main(int argc, char const *argv[])
{
    eckit::Buffer buffer(8*1024*1024);

    eckit::Timer timer;

    incremental<eckit::xxHash,  200, 1>("xxHash", buffer, timer);
    incremental<eckit::MD4,     20, 1>("MD4",    buffer, timer);
    incremental<eckit::MD5,     20, 1>("MD5",    buffer, timer);
    incremental<eckit::SHA1,    20, 1>("SHA1",   buffer, timer);

    return 0;
}

