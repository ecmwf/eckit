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

#include "eckit/utils/Hash.h"
#include "eckit/memory/ScopedPtr.h"

using namespace std;
using namespace eckit;

template <int N, int M>
void incremental(const char* name, Hash& hash, eckit::Buffer& buffer, eckit::Timer& timer) {

    timer.start();

    for(int i = 0; i < N; ++i ) {
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

    const char *hashes[5] =
    {
        "xxHash",
        "MD4",
        "MD5",
        "SHA1",
        "None"
    };

    for(int i = 0; i < 5; ++i) {

        std::string name = hashes[i];

        if(eckit::HashFactory::has(name)) {

            eckit::ScopedPtr<eckit::Hash> hash( eckit::HashFactory::build(name) );

            incremental<20,1>(hashes[i], *hash, buffer, timer);
        }
    }

    return 0;
}

