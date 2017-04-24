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

#include "eckit/utils/SHA1.h"
#include "eckit/log/Timer.h"
#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"

using namespace std;
using namespace eckit;

int main(int argc, char const *argv[])
{
    eckit::Buffer buffer(16*1024*1024);

    eckit::Timer timer;

    for(int i = 0; i < 250; ++i ) {

        timer.start();

        eckit::SHA1 hash;
        for(size_t j=0; j < 1; ++j) {
            hash.add(buffer, buffer.size());
        }

        hash.digest();

        timer.stop();

        std::cout << i << " sha1 " << hash.digest() << " rate " << Bytes(buffer.size(), timer) << std::endl;

    }
    return 0;
}

