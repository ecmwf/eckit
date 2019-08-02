/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>

#include <iomanip>
#include <memory>
#include <string>

#include "eckit/config/Resource.h"
#include "eckit/filesystem/FileMode.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathExpander.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/io/DataHandle.h"
#include "eckit/os/AutoUmask.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------


CASE("Create a file controling its mode") {
    SECTION("---,---,---") {
        FileMode m(("---,---,---"));
        FileMode m2;
        EXPECT_NO_THROW(m2 = ("---,---,---"));
        EXPECT(m == m2);
        std::cout << m << " = " << std::oct << std::setw(4) << std::setfill('0') << m.mode() << std::dec << std::endl;
        EXPECT(m.str() == std::string(("---,---,---")));
    }
    SECTION("rwx,rwx,rwx") {
        FileMode m("rwx,rwx,rwx");
        FileMode m2;
        EXPECT_NO_THROW(m2 = "rwx,rwx,rwx");
        EXPECT(m == m2);
        std::cout << m << " = " << std::oct << std::setw(4) << std::setfill('0') << m.mode() << std::dec << std::endl;
        EXPECT(m.str() == std::string("rwx,rwx,rwx"));
    }
    SECTION("rwx,---,---") {
        FileMode m("rwx,---,---");
        FileMode m2;
        EXPECT_NO_THROW(m2 = "rwx,---,---");
        EXPECT(m == m2);
        std::cout << m << " = " << std::oct << std::setw(4) << std::setfill('0') << m.mode() << std::dec << std::endl;
        EXPECT(m.str() == std::string("rwx,---,---"));
    }
    SECTION("---,rwx,---") {
        FileMode m("---,rwx,---");
        FileMode m2;
        EXPECT_NO_THROW(m2 = "---,rwx,---");
        EXPECT(m == m2);
        std::cout << m << " = " << std::oct << std::setw(4) << std::setfill('0') << m.mode() << std::dec << std::endl;
        EXPECT(m.str() == std::string("---,rwx,---"));
    }
    SECTION("---,---,rwx") {
        FileMode m("---,---,rwx");
        FileMode m2;
        EXPECT_NO_THROW(m2 = "---,---,rwx");
        EXPECT(m == m2);
        std::cout << m << " = " << std::oct << std::setw(4) << std::setfill('0') << m.mode() << std::dec << std::endl;
        EXPECT(m.str() == std::string("---,---,rwx"));
    }
    SECTION("rw-,r--,r--") {
        FileMode m("rw-,r--,r--");
        FileMode m2;
        EXPECT_NO_THROW(m2 = "rw-,r--,r--");
        EXPECT(m == m2);
        std::cout << m << " = " << std::oct << std::setw(4) << std::setfill('0') << m.mode() << std::dec << std::endl;
        EXPECT(m.str() == std::string("rw-,r--,r--"));
    }
    SECTION("rwx,r-x,r-x") {
        FileMode m("rwx,r-x,r-x");
        FileMode m2;
        EXPECT_NO_THROW(m2 = "rwx,r-x,r-x");
        EXPECT(m == m2);
        std::cout << m << " = " << std::oct << std::setw(4) << std::setfill('0') << m.mode() << std::dec << std::endl;
        EXPECT(m.str() == std::string("rwx,r-x,r-x"));
    }
}

CASE("Handle bad strings") {

    FileMode m;

    SECTION("Bad chars") {
        EXPECT_THROWS_AS(m = "Owx,rwx,rwx", BadValue);
        EXPECT_THROWS_AS(m = "rOx,rwx,rwx", BadValue);
        EXPECT_THROWS_AS(m = "rwO,rwx,rwx", BadValue);

        EXPECT_THROWS_AS(m = "rwx,Owx,rwx", BadValue);
        EXPECT_THROWS_AS(m = "rwx,rOx,rwx", BadValue);
        EXPECT_THROWS_AS(m = "rwx,rwO,rwx", BadValue);

        EXPECT_THROWS_AS(m = "rwx,rwx,Owx", BadValue);
        EXPECT_THROWS_AS(m = "rwx,rwx,rOx", BadValue);
        EXPECT_THROWS_AS(m = "rwx,rwx,rwO", BadValue);

        EXPECT_THROWS_AS(m = "w--,---,---", BadValue);
        EXPECT_THROWS_AS(m = "x--,---,---", BadValue);
        EXPECT_THROWS_AS(m = "-r-,---,---", BadValue);
        EXPECT_THROWS_AS(m = "-x-,---,---", BadValue);
        EXPECT_THROWS_AS(m = "--r,---,---", BadValue);
        EXPECT_THROWS_AS(m = "--w,---,---", BadValue);

        EXPECT_THROWS_AS(m = "---,w--,---", BadValue);
        EXPECT_THROWS_AS(m = "---,x--,---", BadValue);
        EXPECT_THROWS_AS(m = "---,-r-,---", BadValue);
        EXPECT_THROWS_AS(m = "---,-x-,---", BadValue);
        EXPECT_THROWS_AS(m = "---,--r,---", BadValue);
        EXPECT_THROWS_AS(m = "---,--w,---", BadValue);

        EXPECT_THROWS_AS(m = "---,---,w--", BadValue);
        EXPECT_THROWS_AS(m = "---,---,x--", BadValue);
        EXPECT_THROWS_AS(m = "---,---,-r-", BadValue);
        EXPECT_THROWS_AS(m = "---,---,-x-", BadValue);
        EXPECT_THROWS_AS(m = "---,---,--r", BadValue);
        EXPECT_THROWS_AS(m = "---,---,--w", BadValue);

        EXPECT_THROWS_AS(m = "0228", BadValue);
    }
}

CASE("Handle bad constructor") {
    EXPECT_THROWS_AS(FileMode m(10000), BadValue);
}


CASE("Create a file and set its permissions") {
    AutoUmask mask(0);
    PathName p("foo.txt");
    std::unique_ptr<DataHandle> dh(p.fileHandle());
    dh->openForAppend(0);
    FileMode m("rw-,r--,r--");
    p.chmod(m);
    dh->close();

    EXPECT(FileMode::fromPath(p) == m);
    p.unlink();
}

CASE("Octal mode") {
    FileMode mode1("0666");
    FileMode mode2(0666);
    FileMode mode3("rw-,rw-,rw-");
    FileMode mode4 = std::string("rw-,rw-,rw-");

    std::cout << "mode1 = " << mode1 << " oct=" << std::oct << std::setw(4) << std::setfill('0') << mode1.mode()
              << std::dec << std::endl;
    std::cout << "mode2 = " << mode2 << " oct=" << std::oct << std::setw(4) << std::setfill('0') << mode2.mode()
              << std::dec << std::endl;
    std::cout << "mode3 = " << mode3 << " oct=" << std::oct << std::setw(4) << std::setfill('0') << mode3.mode()
              << std::dec << std::endl;
    std::cout << "mode4 = " << mode4 << " oct=" << std::oct << std::setw(4) << std::setfill('0') << mode4.mode()
              << std::dec << std::endl;

    EXPECT(mode1 == mode2);
    EXPECT(mode1 == mode3);
    EXPECT(mode1 == mode4);
}


CASE("Use as umask") {
    FileMode filemode(0644);

    EXPECT(filemode.mask() == 022);

    AutoUmask mask(filemode.mask());

    SECTION("File creation") {
        PathName p("bar.txt");
        std::unique_ptr<DataHandle> dh(p.fileHandle());
        dh->openForAppend(0);
        dh->close();

        FileMode result = FileMode::fromPath(p);

        std::cout << "result = " << result << " oct=" << std::oct << std::setw(4) << std::setfill('0') << result.mode()
                  << std::dec << std::endl;

        EXPECT(result == filemode);
        p.unlink();
    }

    SECTION("Directory creation") {
        PathName p("mydir");
        p.mkdir();

        FileMode result = FileMode::fromPath(p);

        std::cout << "result = " << result << " oct=" << std::oct << std::setw(4) << std::setfill('0') << result.mode()
                  << std::dec << std::endl;

        EXPECT(result == FileMode(0755));  // directories get added --x,--x,--x (0111) by default
        p.rmdir();
    }
}

CASE("From resource use as mask") {
    eckit::FileMode filemode(eckit::Resource<std::string>("testFileMode", std::string("0644")));

    EXPECT(filemode.mask() == 022);

    AutoUmask mask(filemode.mask());

    SECTION("File creation") {
        PathName p("bar.txt");
        std::unique_ptr<DataHandle> dh(p.fileHandle());
        dh->openForAppend(0);
        dh->close();

        FileMode result = FileMode::fromPath(p);

        std::cout << "result = " << result << " oct=" << std::oct << std::setw(4) << std::setfill('0') << result.mode()
                  << std::dec << std::endl;

        EXPECT(result == filemode);
        p.unlink();
    }

    SECTION("Directory creation") {
        PathName p("mydir");
        p.mkdir();

        FileMode result = FileMode::fromPath(p);

        std::cout << "result = " << result << " oct=" << std::oct << std::setw(4) << std::setfill('0') << result.mode()
                  << std::dec << std::endl;

        EXPECT(result == FileMode(0755));  // directories get added --x,--x,--x (0111) by default
        p.rmdir();
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
