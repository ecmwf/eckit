/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <string>

#include "eckit/io/URLHandle.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Get URL without content-lenght") {
    PathName out("/tmp/foo.html");
    {
        URLHandle h("https://tinyurl.com/2ndhvukc");
        h.saveInto(out);
    }
    Log::info() << out << " size " << out.size() << std::endl;
    EXPECT(out.size() > Length(0));
    out.unlink();
}

CASE("Get URL 301 and follow Location redirect") {
    PathName out("/tmp/foo.html");
    {
        URLHandle h("https://www.wikipedia.net");  // redirects to https://www.wikipedia.org
        Log::info() << "Size " << h.size() << ", estimate " << h.estimate() << std::endl;
        h.saveInto(out);
    }
    Log::info() << out << " size " << out.size() << std::endl;
    EXPECT(out.size() > Length(0));
    out.unlink();
}

CASE("Get URL small file 41 bytes") {
    PathName out("/tmp/t.grib.md5");
    {
        URLHandle h("http://sites.ecmwf.int/repository/eckit/tests/io/t.grib.md5");
        h.saveInto(out);
    }
    Log::info() << out << " size " << out.size() << std::endl;
    EXPECT(out.size() == Length(41));
    out.unlink();
}

CASE("Get URL 800K file") {
    PathName out("/tmp/single-field.grib");
    {
        URLHandle h("http://sites.ecmwf.int/repository/eckit/tests/io/single-field.grib");
        h.saveInto(out);
    }
    Log::info() << out << " size " << out.size() << std::endl;
    EXPECT(out.size() == Length(822360));
    out.unlink();
}

CASE("Handle URLException 404") {
    PathName out("/tmp/foobar");
    URLHandle h("http://sites.ecmwf.int/repository/eckit/tests/io/foobar");
    try {
        h.saveInto(out);
    }
    catch (URLException& e) {
        EXPECT(e.code() == 404);
    }
}

CASE("No use of SSL") {
    bool useSSL = false;
    PathName out("/tmp/foobar");
    URLHandle h("https://sites.ecmwf.int/repository/eckit/geo/grid/icon/ICON_56_R02B05.icon.ek", useSSL);
    { h.saveInto(out); }
    Log::info() << out << " size " << out.size() << std::endl;
    EXPECT(out.size() == Length(43943));
    out.unlink();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
