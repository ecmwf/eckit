/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <string>
#include <memory>

#include "eckit/types/Types.h"

#include "eckit/filesystem/FileSystemSize.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpDir.h"
#include "eckit/filesystem/TmpFile.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/utils/Hash.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Building of paths") {
    EXPECT(LocalPathName().path() == "/");

    LocalPathName p1("/fred/bill");
    EXPECT(p1 == "/fred/bill");

    LocalPathName p2(p1);
    EXPECT(p1 == p2);
}

CASE("Assignement of paths") {
    LocalPathName p;
    LocalPathName pd;
    p = pd;
    EXPECT(p.path() == "/");

    LocalPathName p1("/fred/bill");
    LocalPathName p2;
    p2 = p1;
    EXPECT(p2.path() == "/fred/bill");

    LocalPathName p3;
    p3 = "/fred";
    EXPECT(p3.path() == "/fred");

    LocalPathName p4;
    p4 = std::string("/martin");
    EXPECT(p4.path() == "/martin");
}

CASE("Contactenation of paths") {

    LocalPathName p;
    p += "fred";
    EXPECT(p.path() == "/fred");

    p += "/joe/90";
    EXPECT(p.path() == "/fred/joe/90");

    // preserve trailing '/'
    p += '/';
    EXPECT(p == "/fred/joe/90/");
}

CASE("Extract dirname") {
    LocalPathName p("/fred/bill");
    EXPECT(p.dirName() == "/fred");
    EXPECT(p.fullName() == "/fred/bill");

    // when no leading '/' on pathname, we append cwd to path, for fullName()
    LocalPathName p2("fred");
    LocalPathName expected = LocalPathName::cwd() + LocalPathName("/fred");
    EXPECT(p2.fullName() == expected);
}

CASE("Check a path exists and is a directory") {
    LocalPathName cwd = LocalPathName::cwd();

    EXPECT(cwd.exists());
    EXPECT(cwd.isDir());

    LocalPathName acwd = LocalPathName::cwd();
    EXPECT(cwd.sameAs(acwd));

    Log::info() << "cwd " << cwd << std::endl;
    Log::info() << "cwd.mountPoint() " << cwd.mountPoint() << std::endl;
    Log::info() << "cwd.realName() " << cwd.realName() << std::endl;
    Log::info() << "cwd.node() " << cwd.node() << std::endl;
    Log::info() << "cwd.path() " << cwd.path() << std::endl;
    Log::info() << "size " << cwd.size() << std::endl;
    Log::info() << "lastAccess " << cwd.lastAccess() << std::endl;
    Log::info() << "lastModified " << cwd.lastModified() << std::endl;
    Log::info() << "created " << cwd.created() << std::endl;
}

CASE("Construct relative paths") {
    LocalPathName foobar("/foo/bar/1/2");
    LocalPathName foozing("/foo/zing/3");
    LocalPathName baz("/baz/koko/4");
    LocalPathName root("/");

    // shorter relative to longer

    LocalPathName r1 = foozing.relativePath(foobar);

    EXPECT(r1 == LocalPathName("../../../zing/3"));

    // longer relative to shorter

    LocalPathName r2 = foobar.relativePath(foozing);

    EXPECT(r2 == LocalPathName("../../bar/1/2"));

    // same relative to same

    LocalPathName r3 = foobar.relativePath(foobar);

    EXPECT(r3 == LocalPathName("."));

    // relative to root

    EXPECT(foobar.relativePath("/") == LocalPathName("foo/bar/1/2"));

    // root relative to path

    EXPECT(root.relativePath(foobar) == LocalPathName("../../../.."));
}

CASE("We can touch files/directories to update the timestamp") {
    TmpDir d;

    PathName foo = d / "foo";
    PathName bar = d / "bar";
    PathName baz = d / "baz";

    foo.mkdir();
    EXPECT(foo.exists());


    EXPECT_NO_THROW(foo.touch());  // updates the timestamp

    EXPECT(!bar.exists());
    EXPECT_NO_THROW(bar.touch());  // doesn't exist, so creates a file
    EXPECT(bar.exists());

    // time resolution is 1 second, so wait slightly longer
    ::usleep(1000001);

    EXPECT(!baz.exists());
    EXPECT_NO_THROW(baz.touch());  // doesn't exist, so creates a file
    EXPECT(baz.exists());

    EXPECT(bar.lastAccess() < baz.lastAccess());
    EXPECT(bar.lastModified() < baz.lastModified());

    // update timestamp of older file
    ::usleep(1000001);

    EXPECT_NO_THROW(bar.touch());  // exists, so updates last accessed time

    EXPECT(bar.lastAccess() > baz.lastAccess());
    EXPECT(bar.lastModified() > baz.lastModified());
}

CASE("Find children files and dirs") {
    std::set<LocalPathName> ref_dirs;
    ref_dirs.insert("testdir/foo");
    ref_dirs.insert("testdir/foo/1");
    ref_dirs.insert("testdir/foo/2");
    ref_dirs.insert("testdir/foo/2/1");
    ref_dirs.insert("testdir/bar");
    ref_dirs.insert("testdir/baz");

    std::set<LocalPathName> ref_files;
    ref_files.insert("testdir/foo/file.1");
    ref_files.insert("testdir/foo/file.2");
    ref_files.insert("testdir/bar/file.3");

    for (std::set<LocalPathName>::const_iterator j = ref_files.begin(); j != ref_files.end(); ++j)
        j->touch();

    PathName t(LocalPathName::cwd());
    t += "/testdir";

    std::vector<PathName> files;
    std::vector<PathName> dirs;

    SECTION("Find in one level") {
        t.children(files, dirs);

        EXPECT(files.size() == 0);
        EXPECT(dirs.size() == 3);

        std::cout << files << std::endl;
        std::cout << dirs << std::endl;

        for (std::vector<PathName>::const_iterator j = dirs.begin(); j != dirs.end(); ++j) {
            LocalPathName d(j->localPath());
            LocalPathName r = d.relativePath(LocalPathName::cwd());
            std::cout << "relative path " << r << std::endl;
            EXPECT(ref_dirs.find(r) != ref_dirs.end());
        }
    }

    SECTION("Find recursively") {
        t.childrenRecursive(files, dirs);

        EXPECT(files.size() == 3);
        EXPECT(dirs.size() == 6);

        std::cout << files << std::endl;
        std::cout << dirs << std::endl;

        for (std::vector<PathName>::const_iterator j = dirs.begin(); j != dirs.end(); ++j) {
            LocalPathName d(j->localPath());
            LocalPathName r = d.relativePath(LocalPathName::cwd());
            std::cout << "relative path " << r << std::endl;
            EXPECT(ref_dirs.find(r) != ref_dirs.end());
        }

        for (std::vector<PathName>::const_iterator j = files.begin(); j != files.end(); ++j) {
            LocalPathName d(j->localPath());
            LocalPathName r = d.relativePath(LocalPathName::cwd());
            std::cout << "relative path " << r << std::endl;
            EXPECT(ref_files.find(r) != ref_files.end());
        }
    }
}

CASE("Extract basename") {
    LocalPathName p1;
    EXPECT(p1.baseName(false) == "");
    EXPECT(p1.baseName(true) == "");

    LocalPathName p2("fred");
    EXPECT(p2.baseName(false) == "fred");
    EXPECT(p2.baseName(true) == "fred");

    LocalPathName p("/a/made/up/path/that/does/not/exist/file.ok");
    EXPECT(p.baseName(false) == "file");
    EXPECT(p.baseName(true) == "file.ok");
}

CASE("Extract extension from Path") {
    EXPECT(LocalPathName().extension() == "");
    EXPECT(LocalPathName("fred").extension() == "");
    EXPECT(LocalPathName("/path/to/fred").extension() == "");
    EXPECT(LocalPathName("/path/with.dot/to/fred").extension() == "");

    EXPECT(LocalPathName("fred.").extension() == ".");
    EXPECT(LocalPathName("fred..").extension() == ".");
    EXPECT(LocalPathName("/path/to/fred.ext").extension() == ".ext");
    EXPECT(LocalPathName("/path/with.dot/to/fred.ext").extension() == ".ext");
}

CASE("Calculate file system size") {
    LocalPathName cwd = LocalPathName::cwd();
    FileSystemSize fs;
    cwd.fileSystemSize(fs);

    Log::info() << "cwd " << cwd << std::endl;
    Log::info() << "fs.available " << fs.available << std::endl;
    Log::info() << "fs.total " << fs.total << std::endl;

    EXPECT(fs.available > 0);
    EXPECT(fs.total > 0);
}

CASE("Create a unique path") {
    LocalPathName unique = LocalPathName::unique(LocalPathName::cwd());

    unique.mkdir();

    EXPECT(unique.exists());

    unique.rmdir();
    EXPECT(!unique.exists());
}

CASE("Check link") {

    LocalPathName unique = LocalPathName::unique(LocalPathName::cwd());
    unique.mkdir();

    LocalPathName linkpath = LocalPathName::cwd() + "link";

    ::symlink(unique.c_str(), linkpath.c_str());

    EXPECT(linkpath.exists());
    EXPECT(linkpath.isLink());

    unique.rmdir();
    linkpath.unlink();
}

CASE("Special characters not expanded in baseName") {

    LocalPathName p = "/base/path/~filename";

    EXPECT(p.dirName() == "/base/path");
    EXPECT(std::string(p.baseName()) == "~filename");
}

std::string tidy(const std::string& p) {
    return LocalPathName(p).path();
}

CASE("Tidy a path") {


    SECTION("Constants") {
        EXPECT(tidy("") == "");
        EXPECT(tidy(".") == ".");
        EXPECT(tidy("..") == "..");
        EXPECT(tidy("/") == "/");
        EXPECT(tidy("a") == "a");
        EXPECT(tidy("/a") == "/a");
        EXPECT(tidy("aaa") == "aaa");
        EXPECT(tidy("/aaa") == "/aaa");
        EXPECT(tidy("a/b") == "a/b");
        EXPECT(tidy("/a/b") == "/a/b");
        EXPECT(tidy("/a/b/") == "/a/b/");
    }


    SECTION("Basic rules") {
        EXPECT(tidy("a/") == "a/");
        EXPECT(tidy("/a/.") == "/a");
        EXPECT(tidy("/a/b/..") == "/a");
        EXPECT(tidy("////") == "/");
        EXPECT(tidy("./.") == ".");
        EXPECT(tidy("./..") == "..");
        EXPECT(tidy("../..") == "../..");
        EXPECT(tidy("../.") == "..");
    }

    SECTION("Cleanup unnecessary tokens") {

        EXPECT(tidy("/a/./b/../../c/") == "/c/");
        EXPECT(tidy("/a/./b/../../c") == "/c");

        EXPECT(tidy("/../../../../../a") == "/a");

        EXPECT(tidy("/a/./b/./c/./d") == "/a/b/c/d");


        EXPECT(tidy("./../foo.bar") == "../foo.bar");  // ECKIT-421
        EXPECT(tidy(".//../foo.bar") == "../foo.bar");
        EXPECT(tidy("..//foo.bar") == "../foo.bar");
        EXPECT(tidy("././././././..//foo.bar") == "../foo.bar");

        EXPECT(tidy("/a//b//c//////d") == "/a/b/c/d");
    }

    SECTION("Merging of tokens") {
        EXPECT(tidy("/a//b/") == "/a/b/");
        EXPECT(tidy("/a/..//foo.bar") == "/foo.bar");
        EXPECT(tidy("/a/../b/foo.bar") == "/b/foo.bar");
        EXPECT(tidy("/a/b/../../c/foo.bar") == "/c/foo.bar");

        EXPECT(tidy("/a/../b/../c/../d") == "/d");
        EXPECT(tidy("../a/../b/../c/../d") == "../d");
        EXPECT(tidy("a/../b/../c/../d") == "d");
        EXPECT(tidy("/a/./b/./c/./d") == "/a/b/c/d");
    }

    SECTION("Absolute paths") {
        EXPECT(tidy("/a/..") == "/");
        EXPECT(tidy("/.") == "/");
    }

    SECTION("Relative paths") {
        EXPECT(tidy("../a") == "../a");
        EXPECT(tidy("a/b/c/..") == "a/b");
        EXPECT(tidy("a/b/c/../../..") == "");
        EXPECT(tidy("a/../b/../c") == "c");
        EXPECT(tidy("a/../b/../c/..") == "");
        EXPECT(tidy("a/../b/../c/../../..") == "../..");
        EXPECT(tidy("../a/b/../../c") == "../c");
        EXPECT(tidy("../a/../b/../../c") == "../../c");
    }

    SECTION("Odd ones") {
        EXPECT(tidy("/../") == "/");
        EXPECT(tidy("/./../foo.bar") == "/foo.bar");
        EXPECT(tidy("/a/b/../../../c/foo.bar") == "/c/foo.bar");
        EXPECT(tidy("../a/b/../../../c/foo.bar") == "../../c/foo.bar");
        EXPECT(tidy("/a/../.././../../.") == "/");
    }
}

CASE("Test PathName hashing") {

    // Create and hash some random data. Note that the buffer is bigger than and a non-integer
    // multiple of the buffer size used in hash generation

#ifdef eckit_HAVE_XXHASH
    const char* hash_method = "xxh64";
#else
    const char* hash_method = "MD5";
#endif

    Buffer tmp(131 * 1024 * 1024);

    for (int i = 0; i < tmp.size(); ++i) {
        tmp[i] = static_cast<char>(random() % 255);
    }

    std::unique_ptr<Hash> h(HashFactory::instance().build(hash_method));
    h->add(tmp, tmp.size());

    // Write the data to a file

    TmpFile file;
    {
        std::unique_ptr<DataHandle> dh(file.fileHandle());
        dh->openForWrite(0);
        AutoClose closer(*dh);
        dh->write(tmp, tmp.size());
    }

    // Check that the hash matches

    EXPECT(h->digest() == file.hash(hash_method));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
