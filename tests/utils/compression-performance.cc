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
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <memory>

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
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
size_t timeCompress(const Compressor& compressor, eckit::Buffer& inBuffer, eckit::Buffer& outBuffer,
                    eckit::Timer& timer) {

    timer.start();

    size_t out;
    for (int i = 0; i < N; ++i) {
        out = compressor.compress(inBuffer, inBuffer.size(), outBuffer);
    }

    timer.stop();

    std::cout << "       - compress()   rate " << Bytes(N * inBuffer.size(), timer) << " -- factor " << std::fixed
              << std::setprecision(2) << ((100.0 * out) / inBuffer.size()) << "% -- ";
    std::cout << std::fixed << std::setprecision(1) << (out / 1024.0) << "/" << (inBuffer.size() / 1024.0) << " KB"
              << std::endl;
    return out;
}

template <int N>
void timeDecompress(const Compressor& compressor, eckit::Buffer& inBuffer, size_t inlen, eckit::Buffer& outBuffer, size_t outlen,
                    eckit::Timer& timer) {

    timer.start();

    for (int i = 0; i < N; ++i) {
        compressor.uncompress(inBuffer, inlen, outBuffer, outlen);
    }

    timer.stop();

    std::cout << "       - decompress() rate " << Bytes(N * outlen, timer) << std::endl;
}


struct BinaryData {
    eckit::Buffer in;
    std::string description;
    BinaryData(const eckit::PathName& path, const std::string& desc) :
        in(path.size()), description(desc) {
        std::unique_ptr<DataHandle> dh(path.fileHandle());
        dh->openForRead();
        dh->read(in, in.size());
        dh->close();
    }
};

//----------------------------------------------------------------------------------------------------------------------

CASE("Test compression performance") {

    eckit::Timer timer;

    std::vector<BinaryData> data;

    data.emplace_back("2t_sfc.grib", "GRIB t2 surface layer");
    data.emplace_back("2t_sfc_regrid.grib", "GRIB t2 surface layer re-gridded");
    data.emplace_back("vo-d_6ml.grib", "GRIB vo/d layers (10-15 spherical harmonics)");
    data.emplace_back("u-v_6ml.grib", "GRIB u/v layers (10-15)");
    data.emplace_back("q_6ml_regrid.grib", "GRIB q 6 layers (10-15) re-gridded");

    std::vector<std::string> compressors{"none", "lz4", "snappy", "aec", "bzip2"};

    constexpr int N = 5;  // Number of iterations to use for each case

    auto test_case = [&](const Compressor& compressor, BinaryData& data) {
        // Allocations here to prevent them from being timed in timings during first iteration
        // Allocation with more than required can prevent internal buffers and memcopies, depending on implementation

        auto ulen       = data.in.size();
        size_t oversize = size_t(1.2 * ulen);  // used in AEC
        Buffer compressed{oversize};
        Buffer uncompressed{oversize};

        // touch memory
        compressed.zero();
        uncompressed.zero();

        // Compress
        size_t clen = timeCompress<N>(compressor, data.in, compressed, timer);

        // Decompress
        timeDecompress<N>(compressor, compressed, clen, uncompressed, ulen, timer);
    };


    for (const auto& name : compressors) {

        if (eckit::CompressorFactory::instance().has(name)) {

            std::cout << name << std::endl;

            std::unique_ptr<eckit::Compressor> compressor(eckit::CompressorFactory::instance().build(name));

            for (auto& d : data) {
                std::cout << "    " << d.description << std::endl;
                test_case(*compressor, d);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
