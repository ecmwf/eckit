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

#include "eckit/io/rados/RadosCluster.h"
#include "eckit/io/rados/RadosObject.h"
#include "eckit/io/rados/RadosHandle.h"
#include "eckit/io/rados/RadosMultiObjReadHandle.h"
#include "eckit/io/rados/RadosMultiObjWriteHandle.h"

#include "eckit/io/Buffer.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("RadosHandle") {

    const char buf[] = "abcdefghijklmnopqrstuvwxyz";

    RadosPool pool("mars");
    /// @todo: auto pool destroyer
    pool.ensureCreated();

    RadosObject obj(pool.name(), "default", "foobar");

    RadosHandle h(obj);
    std::cout << "====> " << h << std::endl;

    h.openForWrite(sizeof(buf));
    h.write(buf, sizeof(buf));
    h.close();

    std::cout << "write done" << std::endl;

    Buffer mem(1024);
    RadosHandle g(obj);
    std::cout << "====> " << g << std::endl;

    std::cout << "Size is " << g.openForRead() << std::endl;
    g.read(mem, mem.size());
    g.close();

    std::cout << "read done" << std::endl;


    EXPECT(buf == std::string(mem));

    obj.ensureDestroyed();

    pool.destroy();

}


CASE("RadosMultiObjWriteHandle") {

    const char buf[] =
        "abcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyz"
        "abcdefghijklmnopqrstuvwxyz";

    RadosPool pool("mars");
    /// @todo: auto pool destroyer
    pool.ensureCreated();

    RadosObject obj(pool.name(), "default", "foobar");

    RadosMultiObjWriteHandle h(obj, false, 16);
    std::cout << "====> " << h << std::endl;

    h.openForWrite(sizeof(buf));
    h.write(buf, sizeof(buf));
    h.close();

    Buffer mem(1024);
    RadosMultiObjReadHandle g(obj);
    std::cout << "====> " << g << std::endl;

    std::cout << "Size is " << g.openForRead() << std::endl;
    g.read(mem, mem.size());
    g.close();

    std::cout << "read done" << std::endl;


    EXPECT(buf == std::string(mem));

    RadosCluster::instance().removeAll(obj);

    pool.destroy();

}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
