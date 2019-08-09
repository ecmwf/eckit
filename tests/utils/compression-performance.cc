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

#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
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
    DataHandle *dh;

    eckit::PathName path2T("2t_sfc.grib");
    length = path2T.size();
    eckit::Buffer in2TGrib(length);
    eckit::ResizableBuffer out2TGrib(length);
    dh = path2T.fileHandle();
    dh->openForRead();
    dh->read(in2TGrib, length);
    dh->close();

    eckit::PathName path2Tregrid("2t_sfc_regrid.grib");
    length = path2Tregrid.size();
    eckit::Buffer in2TgGrib(length);
    eckit::ResizableBuffer out2TgGrib(length);
    dh = path2Tregrid.fileHandle();
    dh->openForRead();
    dh->read(in2TgGrib, length);
    dh->close();

    eckit::PathName pathVO_D("vo-d_6ml.grib");
    length = pathVO_D.size();
    dh = pathVO_D.fileHandle();
    eckit::Buffer inVO_DGrib(length);
    eckit::ResizableBuffer outVO_DGrib(length);
    dh->openForRead();
    dh->read(inVO_DGrib, length);
    dh->close();

    eckit::PathName pathU_V("u-v_6ml.grib");
    length = pathU_V.size();
    eckit::Buffer inU_VGrib(length);
    eckit::ResizableBuffer outU_VGrib(length);
    dh = pathU_V.fileHandle();
    dh->openForRead();
    dh->read(inU_VGrib, length);
    dh->close();

    eckit::PathName pathQ("q_6ml_regrid.grib");
    length = pathQ.size();
    eckit::Buffer inQgGrib(length);
    eckit::ResizableBuffer outQgGrib(length);
    dh = pathQ.fileHandle();
    dh->openForRead();
    dh->read(inQgGrib, length);
    dh->close();

    const char* compressors[5] = {"none", "lz4", "snappy", "aec", "bzip2"};

    for (int i = 0; i < 5; ++i) {

        std::string name = compressors[i];

        if (eckit::CompressorFactory::instance().has(name)) {

            std::cout << name << std::endl;

            std::unique_ptr<eckit::Compressor> compressor(eckit::CompressorFactory::instance().build(name));

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

            std::cout << "   GRIB vo/d layers (10-15 spherical harmonics)" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inVO_DGrib, outVO_DGrib, timer);
            Buffer compressedVO_DGrib(outVO_DGrib, compressedLenght);
            outVO_DGrib.zero();
            timeDecompress<5>(*compressor, compressedVO_DGrib, outVO_DGrib, timer);

            std::cout << "   GRIB u/v layers (10-15)" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inU_VGrib, outU_VGrib, timer);
            Buffer compressedU_VGrib(outU_VGrib, compressedLenght);
            outU_VGrib.zero();
            timeDecompress<5>(*compressor, compressedU_VGrib, outU_VGrib, timer);

            std::cout << "   GRIB q 6 layers (10-15) re-gridded" << std::endl;
            compressedLenght = timeCompress<5>(*compressor, inQgGrib, outQgGrib, timer);
            Buffer compressedQgGrib(outQgGrib, compressedLenght);
            outQgGrib.zero();
            timeDecompress<5>(*compressor, compressedQgGrib, outQgGrib, timer);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
