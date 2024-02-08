/*
 * Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/// @file   test_s3handle.cc
/// @author Metin Cakircali
/// @date   Jan 2024

#include "eckit/filesystem/URI.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/s3/S3Handle.h"
#include "eckit/testing/Test.h"

#include <cstring>

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("S3Handle") {
    const char buf[] = "abcdefghijklmnopqrstuvwxyz";

    S3Name name("myS3region", "myS3bucket", "myS3object");

    {
        auto h = name.dataHandle();
        h->openForWrite(sizeof(buf));
        h->write(buf, sizeof(buf));
    }
    // h->close();

    URI uri("s3://hostname:port/region/bucket/object");
    // credentials

    // std::unique_ptr<DataHandle> dh = uri.dataHandle();
    // dh->openForRead();
    // AutoClose closer(dh);
    // dh->read(buf, length);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
