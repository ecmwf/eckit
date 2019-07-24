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
    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\nexpver=1,\nlevelist=10/to/15,\nlevtype=ml,\nparam=vo/d,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream isVO_D( "sample.grib", std::ios::binary );
    isVO_D.seekg (0, ios::end);
    length = isVO_D.tellg();
    isVO_D.seekg (0, ios::beg);

    eckit::Buffer inVO_DGrib(length);
    eckit::ResizableBuffer outVO_DGrib(length);
    isVO_D.read(inVO_DGrib, length);
    remove( "sample.grib" );
#endif

#ifdef GRIB_POINT
    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\nexpver=1,\nlevelist=10/to/15,\ngrid=O1280,\nlevtype=ml,\nparam=u/v,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream isU_V( "sample.grib", std::ios::binary );
    isU_V.seekg (0, ios::end);
    length = isU_V.tellg();
    isU_V.seekg (0, ios::beg);

    eckit::Buffer inU_VGrib(length);
    eckit::ResizableBuffer outU_VGrib(length);
    isU_V.read(inU_VGrib, length);


    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\nexpver=1,\nlevtype=sfc,\nparam=2t,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream is2T( "sample.grib", std::ios::binary );
    is2T.seekg (0, ios::end);
    length = is2T.tellg();
    is2T.seekg (0, ios::beg);

    eckit::Buffer in2TGrib(length);
    eckit::ResizableBuffer out2TGrib(length);
    is2T.read(in2TGrib, length);


    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\ngrid=0.1/0.1,\nexpver=1,\nlevtype=sfc,\nparam=2t,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream is2Tg( "sample.grib", std::ios::binary );
    is2Tg.seekg (0, ios::end);
    length = is2Tg.tellg();
    is2Tg.seekg (0, ios::beg);

    eckit::Buffer in2TgGrib(length);
    eckit::ResizableBuffer out2TgGrib(length);
    is2Tg.read(in2TgGrib, length);

    //system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\nexpver=1,\nlevelist=10/to/15,\nlevtype=ml,\nparam=133,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    system("mars <<EOF\nretrieve,\nclass=od,\ndate=-1,\ngrid=0.1/0.1,\nexpver=1,\nlevelist=10/to/15,\nlevtype=ml,\nparam=133,\nstep=48,\nstream=oper,\ntime=00:00:00,\ntype=fc,\ntarget=\"sample.grib\"\nEOF");
    std::ifstream isQg( "sample.grib", std::ios::binary );
    isQg.seekg (0, ios::end);
    length = isQg.tellg();
    isQg.seekg (0, ios::beg);

    eckit::Buffer inQgGrib(length);
    eckit::ResizableBuffer outQgGrib(length);
    isQg.read(inQgGrib, length);
    remove( "sample.grib" );
#endif

    const char* compressors[5] = {"none", "lz4", "snappy", "aec", "bzip2"};

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
            std::cout << "   GRIB vo/d layers (10-15 spherical harmonics)" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inVO_DGrib, outVO_DGrib, timer);
            Buffer compressedVO_DGrib(outVO_DGrib, compressedLenght);
            outVO_DGrib.zero();
            timeDecompress<5>(*compressor, compressedVO_DGrib, outVO_DGrib, timer);
#endif
#ifdef GRIB_POINT
            std::cout << "   GRIB u/v layers (10-15)" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inU_VGrib, outU_VGrib, timer);
            Buffer compressedU_VGrib(outU_VGrib, compressedLenght);
            outU_VGrib.zero();
            timeDecompress<5>(*compressor, compressedU_VGrib, outU_VGrib, timer);

            std::cout << "   GRIB t2 surface layer" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, in2TGrib, out2TGrib, timer);
            Buffer compressed2TGrib(out2TGrib, compressedLenght);
            out2TGrib.zero();
            timeDecompress<5>(*compressor, compressed2TGrib, out2TGrib, timer);

            std::cout << "   GRIB t2 surface layer re-gridded" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, in2TgGrib, out2TgGrib, timer);
            Buffer compressed2TgGrib(out2TgGrib, compressedLenght);
            out2TgGrib.zero();
            timeDecompress<5>(*compressor, compressed2TgGrib, out2TgGrib, timer);

            std::cout << "   GRIB q 6 layers (10-15) re-gridded" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inQgGrib, outQgGrib, timer);
            Buffer compressedQgGrib(outQgGrib, compressedLenght);
            outQgGrib.zero();
            timeDecompress<5>(*compressor, compressedQgGrib, outQgGrib, timer);
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
