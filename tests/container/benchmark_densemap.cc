/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>
#include <cstdlib>
#include <map>

#include "eckit/container/DenseMap.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/Timer.h"
#include "eckit/types/FixedString.h"
#include "eckit/utils/Translator.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

#define MSIZE 500
#define NSAMPLES 50000

template <typename MAP>
void benchmark_densemap_int_string(const std::string& tname) {
    std::cout << "-------------------------------------------------------------" << std::endl;
    std::cout << tname << std::endl;

    Translator<int, std::string> itos;

    MAP m;

    {
        Timer timer("insert");
        for (int i = 0; i < MSIZE; ++i) {
            m.insert(i, "foo" + itos(i));
        }
    }

    {
        Timer timer("sort");
        m.sort();
    }

    {
        Timer timer("find");
        for (int i = 0; i < NSAMPLES; ++i) {
            int idx = rand() % MSIZE;

            typename MAP::const_iterator it = m.find(idx);
            if (it != m.cend())
                ASSERT(it->second == "foo" + itos(idx));
            else
                std::cout << "failed: " << idx << " " << std::string("foo") << itos(idx) << std::endl;
        }
    }
}


template <typename MAP>
void benchmark_stdmap_int_string(const std::string& tname) {
    std::cout << "-------------------------------------------------------------" << std::endl;
    std::cout << tname << std::endl;

    Translator<int, std::string> itos;

    MAP m;

    {
        Timer timer("insert");
        for (int i = 0; i < MSIZE; ++i) {
            m.insert(std::make_pair(i, "foo" + itos(i)));
        }
    }

    {
        Timer timer("find");
        for (int i = 0; i < NSAMPLES; ++i) {
            int idx                         = rand() % MSIZE;
            typename MAP::const_iterator it = m.find(idx);
            if (it != m.end())
                ASSERT(it->second == "foo" + itos(idx));
            else
                std::cout << "failed: " << idx << " " << std::string("foo") << itos(idx) << std::endl;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("benchmark_densemap") {

    benchmark_densemap_int_string<DenseMap<int, std::string> >("DenseMap<int,string>");
    benchmark_stdmap_int_string<std::map<int, std::string> >("std::map<int,string>");

    ///

    benchmark_densemap_int_string<DenseMap<int, FixedString<256> > >("DenseMap<int,FixedString>");
    benchmark_stdmap_int_string<std::map<int, FixedString<256> > >("std::map<int,FixedString>");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
