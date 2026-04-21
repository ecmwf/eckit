/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/*
 * This software was developed as part of the Horizon Europe programme funded project OpenCUBE
 * (Grant agreement: 101092984) horizon-opencube.eu
 */

/// @file   test_famlist.cc
/// @author Metin Cakircali
/// @date   May 2024

#include "test_fam_common.h"

#include <set>
#include <string>

#include "eckit/io/Buffer.h"
#include "eckit/io/fam/FamList.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------
// HELPERS

namespace {

using fam::TestFam;

TestFam tester;

const auto list_name = "L" + fam::random_number();

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: create an empty list and validate size, empty, front, back") {

    constexpr const eckit::fam::size_t region_size = 1024;

    auto list_region = tester.makeRandomRegion(region_size);
    const auto list  = FamList(list_region, list_name);

    // empty list should have size 0
    EXPECT(list.empty());
    EXPECT(list.size() == 0);

    // front/back should throw as it's undefined behavior
    EXPECT_THROWS({ auto front = list.front(); });
    EXPECT_THROWS({ auto back = list.back(); });
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: pop on empty list throws") {

    constexpr const eckit::fam::size_t region_size = 1024;

    auto region =
        FamRegionName(fam::test_endpoint, "").withRegion("RP" + fam::random_number()).create(region_size, 0640, true);
    auto list = FamList(region, "LP" + fam::random_number());

    EXPECT(list.empty());
    EXPECT_EQUAL(list.size(), 0);

    EXPECT_THROWS({ list.popFront(); });
    EXPECT_THROWS({ list.popBack(); });

    region.destroy();
}

CASE("FamList: populate a list and validate size, !empty, front, back") {

    constexpr const eckit::fam::size_t region_size = 1024;

    auto region =
        FamRegionName(fam::test_endpoint, "").withRegion("LR" + fam::random_number()).create(region_size, 0640, true);
    auto list = FamList(region, list_name);

    // empty list should have size 0
    EXPECT(list.empty());
    EXPECT_EQUAL(list.size(), 0);

    std::string front = "Front FAM List!";
    EXPECT_NO_THROW(list.pushBack(front));
    EXPECT_EQUAL(list.size(), 1);
    EXPECT_EQUAL(list.front().view(), front);
    EXPECT_EQUAL(list.back().view(), front);


    std::string back = "Back FAM List!";
    EXPECT_NO_THROW(list.pushBack(back));
    EXPECT_EQUAL(list.size(), 2);
    EXPECT_EQUAL(list.front().view(), front);
    EXPECT_EQUAL(list.back().view(), back);

    region.destroy();
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: pop front/back updates size and values") {

    constexpr const eckit::fam::size_t region_size = 1024;

    auto region =
        FamRegionName(fam::test_endpoint, "").withRegion("RP" + fam::random_number()).create(region_size, 0640, true);
    auto list = FamList(region, "LP" + fam::random_number());

    std::string first  = "first";
    std::string second = "second";
    std::string third  = "third";

    EXPECT_NO_THROW(list.pushBack(first));
    EXPECT_NO_THROW(list.pushBack(second));
    EXPECT_NO_THROW(list.pushBack(third));

    EXPECT_EQUAL(list.size(), 3);
    EXPECT_EQUAL(list.front().view(), first);
    EXPECT_EQUAL(list.back().view(), third);

    EXPECT_NO_THROW(list.popFront());
    EXPECT_EQUAL(list.size(), 2);
    EXPECT_EQUAL(list.front().view(), second);
    EXPECT_EQUAL(list.back().view(), third);

    EXPECT_NO_THROW(list.popBack());
    EXPECT_EQUAL(list.size(), 1);
    EXPECT_EQUAL(list.front().view(), second);
    EXPECT_EQUAL(list.back().view(), second);

    EXPECT_NO_THROW(list.popBack());
    EXPECT(list.empty());
    EXPECT_EQUAL(list.size(), 0);

    region.destroy();
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: concurrent pushBack from 4 processes") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;
    constexpr int num_procs                  = 4;
    constexpr int items_per_proc             = 50;

    auto region = tester.makeRandomRegion(region_size);
    auto name   = "MPL" + fam::random_number();

    { FamList list(region, name); }

    bool ok = fork_and_run(num_procs, [&](int child_id) {
        FamList list(region, name);
        for (int i = 0; i < items_per_proc; ++i) {
            auto data = "c" + std::to_string(child_id) + "-i" + std::to_string(i);
            list.pushBack(data.data(), data.size());
        }
    });

    EXPECT(ok);

    FamList list(region, name);
    EXPECT_EQUAL(list.size(), num_procs * items_per_proc);

    std::set<std::string> found;
    for (const auto& item : list) {
        found.insert(std::string(item.view()));
    }
    EXPECT_EQUAL(found.size(), static_cast<std::size_t>(num_procs * items_per_proc));

    for (int c = 0; c < num_procs; ++c) {
        for (int i = 0; i < items_per_proc; ++i) {
            EXPECT(found.count("c" + std::to_string(c) + "-i" + std::to_string(i)) == 1);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: one writer process, parent reads") {
    constexpr eckit::fam::size_t region_size = 2 * 1024 * 1024;
    constexpr int count                      = 20;

    auto region = tester.makeRandomRegion(region_size);
    auto name   = "MPWR" + fam::random_number();

    { FamList list(region, name); }

    bool ok = fork_and_run(1, [&](int) {
        FamList list(region, name);
        for (int i = 0; i < count; ++i) {
            auto data = "item-" + std::to_string(i);
            list.pushBack(data.data(), data.size());
        }
    });

    EXPECT(ok);

    FamList list(region, name);
    EXPECT_EQUAL(list.size(), count);

    std::set<std::string> found;
    for (const auto& item : list) {
        found.insert(std::string(item.view()));
    }
    for (int i = 0; i < count; ++i) {
        EXPECT(found.count("item-" + std::to_string(i)) == 1);
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("FamList: concurrent pushFront and pushBack from 4 processes") {
    constexpr eckit::fam::size_t region_size = 4 * 1024 * 1024;
    constexpr int num_procs                  = 4;
    constexpr int items_per_proc             = 50;

    auto region = tester.makeRandomRegion(region_size);
    auto name   = "MPFB" + fam::random_number();

    { FamList list(region, name); }

    bool ok = fork_and_run(num_procs, [&](int child_id) {
        FamList list(region, name);
        for (int i = 0; i < items_per_proc; ++i) {
            auto data = "c" + std::to_string(child_id) + "-i" + std::to_string(i);
            if (child_id % 2 == 0) {
                list.pushFront(data.data(), data.size());
            }
            else {
                list.pushBack(data.data(), data.size());
            }
        }
    });

    EXPECT(ok);

    FamList list(region, name);
    EXPECT_EQUAL(list.size(), num_procs * items_per_proc);

    std::set<std::string> found;
    for (const auto& item : list) {
        found.insert(std::string(item.view()));
    }
    EXPECT_EQUAL(found.size(), static_cast<std::size_t>(num_procs * items_per_proc));
}

}  // namespace eckit::test

//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
