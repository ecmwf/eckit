/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define OMPI_SKIP_MPICXX 1
#define MPICH_SKIP_MPICXX 1

#include <algorithm>
#include <vector>
#include "eckit/mpi/Comm.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::mpi;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

int argc;
char** argv;

CASE("Test MPI Communicator group") {
    // Get default communicator
    Comm& all = eckit::mpi::comm();

    std::string start = all.name();

    // Check sizes and ranks
    EXPECT(all.size() == 4);
    EXPECT(all.rank() < 4);

    Group parentGroup = all.group();
    EXPECT(parentGroup.size() == 4);
    EXPECT(parentGroup.rank() == all.rank());


    // Split communicator in half
    if (all.rank() < 2) {
        all.split(1, "FirstHalf");
    }
    else {
        all.split(2, "SecondHalf");
    }

    // Fetch the new communicator
    const Comm& newcomm = eckit::mpi::comm((all.rank() < 2 ? "FirstHalf" : "SecondHalf"));

    Group childGroup = newcomm.group();

    // Check sizes and ranks
    EXPECT(newcomm.size() == 2);
    EXPECT(newcomm.size() == childGroup.size());
    EXPECT(newcomm.rank() < 2);
    EXPECT(newcomm.rank() == childGroup.rank());

    // Test intersection to be similar
    Group intersectionGroup = parentGroup.intersection(childGroup);
    EXPECT(intersectionGroup.compare(childGroup) >= 0);

    Group remoteChildGroup = parentGroup.difference(childGroup);
    EXPECT(remoteChildGroup.size() == 2);
    EXPECT(remoteChildGroup.compare(childGroup) < 0);

    // Reconstruct parent - similar
    Group parentG1 = remoteChildGroup.union_group(childGroup);
    EXPECT(parentG1.compare(parentGroup) >= 0);
    Group parentG2 = childGroup.union_group(remoteChildGroup);
    EXPECT(parentG2.compare(parentGroup) >= 0);

    // Both parent groups are similar but different, i.e. one compare should return 0, the other 1
    EXPECT(parentG2.compare(parentGroup) != parentG1.compare(parentGroup));

    // New union groups should be similar but not identical
    EXPECT(parentG2.compare(parentG1) >= 0);

    std::vector<int> localRanks = all.rank() < 2 ? std::vector<int>{0, 1} : std::vector<int>{2, 3};

    std::vector<int> remoteRanks = all.rank() < 2 ? std::vector<int>{2, 3} : std::vector<int>{0, 1};

    auto localTranslated = childGroup.translate_ranks(std::vector<int>{0, 1}, parentGroup);
    EXPECT(localTranslated.find(0) != localTranslated.end());
    EXPECT(localTranslated.find(1) != localTranslated.end());
    EXPECT(std::find(localRanks.begin(), localRanks.end(), localTranslated.at(0)) != localRanks.end());
    EXPECT(std::find(localRanks.begin(), localRanks.end(), localTranslated.at(1)) != localRanks.end());

    auto remoteTranslated = remoteChildGroup.translate_ranks(std::vector<int>{0, 1}, parentGroup);
    EXPECT(remoteTranslated.find(0) != remoteTranslated.end());
    EXPECT(remoteTranslated.find(1) != remoteTranslated.end());
    EXPECT(std::find(remoteRanks.begin(), remoteRanks.end(), remoteTranslated.at(0)) != remoteRanks.end());
    EXPECT(std::find(remoteRanks.begin(), remoteRanks.end(), remoteTranslated.at(1)) != remoteRanks.end());

    // Should be similar
    EXPECT(childGroup.compare(parentGroup.incl(localRanks)) >= 0);
    EXPECT(remoteChildGroup.compare(parentGroup.incl(remoteRanks)) >= 0);
    EXPECT(childGroup.compare(parentGroup.excl(remoteRanks)) >= 0);
    EXPECT(remoteChildGroup.compare(parentGroup.excl(localRanks)) >= 0);

    // Expressed by range incl
    EXPECT(childGroup.compare(parentGroup.range_incl(std::vector<std::array<int, 3>>{{localRanks[0], localRanks[1], 1}})) >= 0);
    EXPECT(remoteChildGroup.compare(parentGroup.range_incl(std::vector<std::array<int, 3>>{{remoteRanks[0], remoteRanks[1], 1}})) >= 0);
    EXPECT(childGroup.compare(parentGroup.range_excl(std::vector<std::array<int, 3>>{{remoteRanks[0], remoteRanks[1], 1}})) >= 0);
    EXPECT(remoteChildGroup.compare(parentGroup.range_excl(std::vector<std::array<int, 3>>{{localRanks[0], localRanks[1], 1}})) >= 0);

    // Expressed by range incl (explicit)
    EXPECT(childGroup.compare(parentGroup.range_incl(std::vector<std::array<int, 3>>{{localRanks[0], localRanks[0], 1}, {localRanks[1], localRanks[1], 1}})) >= 0);
    EXPECT(remoteChildGroup.compare(parentGroup.range_incl(std::vector<std::array<int, 3>>{{remoteRanks[0], remoteRanks[0], 1}, {remoteRanks[1], remoteRanks[1], 1}})) >= 0);
    EXPECT(childGroup.compare(parentGroup.range_excl(std::vector<std::array<int, 3>>{{remoteRanks[0], remoteRanks[0], 1}, {remoteRanks[1], remoteRanks[1], 1}})) >= 0);
    EXPECT(remoteChildGroup.compare(parentGroup.range_excl(std::vector<std::array<int, 3>>{{localRanks[0], localRanks[0], 1}, {localRanks[1], localRanks[1], 1}})) >= 0);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::test::argc = argc;
    eckit::test::argv = argv;
    return run_tests(argc, argv);
}
