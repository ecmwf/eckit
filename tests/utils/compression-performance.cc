/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cassert>
#include <iostream>
#include <memory>

#include "eckit/io/Buffer.h"
#include "eckit/io/ResizableBuffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"

#include "eckit/utils/Compressor.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

template <int N>
size_t timeCompress(Compressor& compressor, eckit::Buffer& inBuffer, eckit::ResizableBuffer& outBuffer, eckit::Timer& timer) {

    timer.start();

    size_t out;
    for (int i = 0; i < N; ++i) {
        out = compressor.compress(inBuffer, outBuffer);
    }

    timer.stop();

    std::cout << " - compress()   rate " << Bytes(N * inBuffer.size(), timer) << " factor " << ((1.0*out)/inBuffer.size()) /*<< "|" << out << "|" << inBuffer.size()*/ << std::endl;
    return out;
}

template <int N>
void timeDecompress(Compressor& compressor, eckit::Buffer& inBuffer, eckit::ResizableBuffer& outBuffer, eckit::Timer& timer) {

    timer.start();

    size_t out;
    for (int i = 0; i < N; ++i) {
        out = compressor.uncompress(inBuffer, outBuffer);
    }

    timer.stop();

    std::cout << " - decompress() rate " << Bytes(N * out, timer) << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Test compression performance") {

    eckit::Buffer inBuffer(4 * 1024 * 1024);
    eckit::Buffer inBuffer2(64 * 1024 * 1024);
    eckit::ResizableBuffer outBuffer(4 * 1024 * 1024);
    eckit::ResizableBuffer outBuffer2(64 * 1024 * 1024);
    eckit::Timer timer;

    const char* compressors[4] = {"none", "lz4", "snappy", "bzip2"};

    for (int i = 0; i < 4; ++i) {

        std::string name = compressors[i];

        if (eckit::CompressorFactory::instance().has(name)) {

            std::cout << name << std::endl;

            std::unique_ptr<eckit::Compressor> compressor(eckit::CompressorFactory::instance().build(name));

            size_t compressedLenght = timeCompress<5>(*compressor, inBuffer, outBuffer, timer);
            Buffer compressed(outBuffer, compressedLenght);
            outBuffer.zero();
            timeDecompress<5>(*compressor, compressed, outBuffer, timer);

            compressedLenght = timeCompress<5>(*compressor, inBuffer2, outBuffer2, timer);
            Buffer compressed2(outBuffer2, compressedLenght);
            outBuffer2.zero();
            timeDecompress<5>(*compressor, compressed2, outBuffer2, timer);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
