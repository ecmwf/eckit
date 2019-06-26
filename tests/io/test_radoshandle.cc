/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/io/rados/RadosHandle.h"

#include "eckit/config/Resource.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/MultiHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/FileHandle.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/types/Types.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Write") {

    const char buf[] = "abcdefghijklmnopqrstuvwxyz";

    RadosHandle h("foobar");
    std::cout << "====> " << h << std::endl;

    h.openForWrite(sizeof(buf));
    h.write(buf, sizeof(buf));
    h.close();



    Buffer mem(1024);
    RadosHandle g("foobar");
    std::cout << "====> " << g << std::endl;

    std::cout << "Size is " << g.openForRead();
    g.read(mem, mem.size());
    g.close();

    EXPECT(buf == std::string(mem));


}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
