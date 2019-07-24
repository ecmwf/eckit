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
#include <fstream>
#include <memory>
#include <cstdio>
#include <locale>
#include <iomanip>

#include "eckit/io/Buffer.h"
#include "eckit/io/ResizableBuffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"

#include "eckit/utils/Compressor.h"

#include "eckit/testing/Test.h"


//#define DUMMY_BUFFER
#define GRIB_RAW
#define GRIB_POINT

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

struct space_out : std::numpunct<char> {
    char do_thousands_sep()   const { return ' '; }  // separate with spaces
};

template <int N>
size_t timeCompress(Compressor& compressor, eckit::Buffer& inBuffer, eckit::ResizableBuffer& outBuffer, eckit::Timer& timer) {

    timer.start();

    size_t out;
    for (int i = 0; i < N; ++i) {
        out = compressor.compress(inBuffer, outBuffer);
    }

    timer.stop();

    std::cout << " - compress()   rate " << Bytes(N * inBuffer.size(), timer) << " factor " << std::fixed << std::setprecision(2) << ((100.0*out)/inBuffer.size()) << "% - ";
    std::cout.imbue(std::locale(std::cout.getloc(), new space_out));
    std::cout << std::fixed << std::setprecision(1) << (out/1024.0) << "/" << (inBuffer.size()/1024.0) << " KB" << std::endl;
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

    eckit::Timer timer;
    size_t length;
    size_t compressedLenght;

#ifdef DUMMY_BUFFER
    eckit::Buffer inBuffer(4 * 1024 * 1024);
    eckit::Buffer inBuffer2(64 * 1024 * 1024);
    eckit::ResizableBuffer outBuffer(4 * 1024 * 1024);
    eckit::ResizableBuffer outBuffer2(64 * 1024 * 1024);
#endif

#ifdef GRIB_RAW
    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\nexpver=1,\nlevtype=sfc,\nparam=167.128,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream is( "sample.grib", std::ios::binary );
    is.seekg (0, ios::end);
    length = is.tellg();
    is.seekg (0, ios::beg);

    eckit::Buffer inGrib(length);
    eckit::ResizableBuffer outGrib(length);
    is.read(inGrib, length);

    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\nexpver=1,\nlevelist=10/to/15,\nlevtype=ml,\nparam=133,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream is6( "sample.grib", std::ios::binary );
    is6.seekg (0, ios::end);
    length = is6.tellg();
    is6.seekg (0, ios::beg);

    eckit::Buffer inGrib6(length);
    eckit::ResizableBuffer outGrib6(length);
    is6.read(inGrib6, length);
    remove( "sample.grib" );
#endif

#ifdef GRIB_POINT
    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\ngrid=0.1/0.1,\nexpver=1,\nlevtype=sfc,\nparam=167.128,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream isP( "sample.grib", std::ios::binary );
    isP.seekg (0, ios::end);
    length = isP.tellg();
    isP.seekg (0, ios::beg);

    eckit::Buffer inGribP(length);
    eckit::ResizableBuffer outGribP(length);
    isP.read(inGribP, length);

    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\ngrid=0.1/0.1,\nexpver=1,\nlevelist=10/to/15,\nlevtype=ml,\nparam=133,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream isP6( "sample.grib", std::ios::binary );
    isP6.seekg (0, ios::end);
    length = isP6.tellg();
    isP6.seekg (0, ios::beg);

    eckit::Buffer inGribP6(length);
    eckit::ResizableBuffer outGribP6(length);
    isP6.read(inGribP6, length);
    remove( "sample.grib" );
#endif

    const char* compressors[5] = {"none", "lz4", "snappy", "bzip2", "aec"};

    for (int i = 0; i < 5; ++i) {

        std::string name = compressors[i];

        if (eckit::CompressorFactory::instance().has(name)) {

            std::cout << name << std::endl;

            std::unique_ptr<eckit::Compressor> compressor(eckit::CompressorFactory::instance().build(name));

#ifdef DUMMY_BUFFER
            size_t compressedLenght = timeCompress<5>(*compressor, inBuffer, outBuffer, timer);
            Buffer compressed(outBuffer, compressedLenght);
            outBuffer.zero();
            timeDecompress<5>(*compressor, compressed, outBuffer, timer);

            compressedLenght = timeCompress<5>(*compressor, inBuffer2, outBuffer2, timer);
            Buffer compressed2(outBuffer2, compressedLenght);
            outBuffer2.zero();
            timeDecompress<5>(*compressor, compressed2, outBuffer2, timer);
#endif
#ifdef GRIB_RAW
            std::cout << "   GRIB t2 surface layer" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inGrib, outGrib, timer);
            Buffer compressedGrib(outGrib, compressedLenght);
            outGrib.zero();
            timeDecompress<5>(*compressor, compressedGrib, outGrib, timer);

            std::cout << "   GRIB rh 6 layers (10-15)" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inGrib6, outGrib6, timer);
            Buffer compressedGrib6(outGrib6, compressedLenght);
            outGrib6.zero();
            timeDecompress<5>(*compressor, compressedGrib6, outGrib6, timer);
#endif
#ifdef GRIB_POINT
            std::cout << "   GRIB t2 surface layer - grid points" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inGribP, outGribP, timer);
            Buffer compressedGribP(outGribP, compressedLenght);
            outGribP.zero();
            timeDecompress<5>(*compressor, compressedGribP, outGribP, timer);

            std::cout << "   GRIB rh 6 layers (10-15) - grid points" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inGribP6, outGribP6, timer);
            Buffer compressedGribP6(outGribP6, compressedLenght);
            outGribP6.zero();
            timeDecompress<5>(*compressor, compressedGribP6, outGribP6, timer);
#endif
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
