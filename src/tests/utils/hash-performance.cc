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
void timeAdd(Hash& hash, eckit::Buffer& buffer, eckit::Timer& timer) {

    timer.start();

    for(int i = 0; i < N; ++i ) {
        for(size_t j=0; j < M; ++j) {
            hash.add(buffer, buffer.size());
        }
        hash.digest();
    }

    timer.stop();

    std::cout << " - add()     rate " << Bytes(N*M*buffer.size(), timer) << std::endl;
}

template <int N>
void timeCompute(Hash& hash, eckit::Buffer& buffer, eckit::Timer& timer) {

    timer.start();

    for(int i = 0; i < N; ++i ) {
            std::string s = hash.compute(buffer, buffer.size());
        }

    timer.stop();

    std::cout << " - compute() rate " << Bytes(N*buffer.size(), timer) << std::endl;
}


int main(int argc, char const *argv[])
{
    eckit::Buffer buffer(4*1024*1024);
    eckit::Buffer buffer2(64*1024*1024);
    eckit::Timer timer;

    const char *hashes[4] =
    {
        "xxHash",
        "MD4",
        "MD5",
        "SHA1"
    };

    for(int i = 0; i < 4; ++i) {

        std::string name = hashes[i];

        if(eckit::HashFactory::has(name)) {

            std::cout << name << std::endl;

            eckit::ScopedPtr<eckit::Hash> hash( eckit::HashFactory::build(name) );

            timeAdd<20,1>  (*hash, buffer, timer);
            timeCompute<5> (*hash, buffer2, timer);

        }
    }

    return 0;
}

