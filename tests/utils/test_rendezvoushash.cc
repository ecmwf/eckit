/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>

#include "eckit/types/Types.h"
#include "eckit/utils/RendezvousHash.h"
#include "eckit/utils/Translator.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_utils_rendezvous_hash_constructor") {
    Log::info() << Here() << std::endl;

    std::vector<std::string> nodes = {"node01", "node02", "node03", "node04"};

    eckit::RendezvousHash rendezvous(nodes, &RendezvousHash::md5);

    std::map<std::string, std::string> dict;

    dict["class"]  = "od";
    dict["stream"] = "oper";
    dict["type"]   = "fc";
    dict["level"]  = "1";


    std::vector<size_t> indices;
    std::vector<std::string> node_order;

    rendezvous.hashOrder(dict, indices);
    rendezvous.hashOrder(dict, node_order);

    EXPECT(indices.size() == 4);
    std::sort(indices.begin(), indices.end());
    EXPECT(indices == std::vector<size_t>({0, 1, 2, 3}));

    EXPECT(nodes.size() == 4);
    for (const std::string& node : nodes) {
        EXPECT(std::find(node_order.begin(), node_order.end(), node) != node_order.end());
    }
}

CASE("test_eckit_utils_rendezvous_hash_distribution") {
    Log::info() << Here() << std::endl;

    eckit::Translator<size_t, std::string> toStr;

    std::vector<std::string> params{"2t", "2d", "z", "u", "v", "130.128", "138.128"};

    std::vector<std::string> nodes = {"node01", "node02", "node03", "node04", "node05", "node06", "node07"};

    eckit::RendezvousHash rendezvous(nodes);

    std::map<std::string, std::string> dict{{"class", "od"}, {"stream", "oper"}, {"type", "fc"}};

    std::map<std::string, size_t> counts;
    std::vector<std::string> ordered_nodes;

    for (std::vector<std::string>::iterator param = params.begin(); param != params.end(); ++param) {

        dict["param"] = *param;

        for (size_t step = 0; step < 240; ++step) {

            dict["step"] = toStr(step);

            for (size_t level = 0; level < 10; ++level) {

                dict["level"] = toStr(level);

                rendezvous.hashOrder(dict, ordered_nodes);
                EXPECT(ordered_nodes.size() == 7);
                counts[ordered_nodes[0]]++;
            }
        }
    }

    // Test that we have << roughly >> similar counts on all the nodes. (Very stochastic).
    for (const auto& it : counts) {
        EXPECT(2320 < it.second && 2450 > it.second);
    }
}

CASE("test_eckit_utils_rendezvous_hash_empty_dict") {
    Log::info() << Here() << std::endl;

    std::map<std::string, std::string> dict;

    eckit::RendezvousHash rendezvous;
    rendezvous.addNode("node01");
    rendezvous.addNode("node02");

    std::vector<size_t> indices;
    EXPECT_NO_THROW(rendezvous.hashOrder(dict, indices)); /* don't throw on empty dictionary */
}

CASE("test_eckit_utils_rendezvous_hash_throws_empty_node_list") {
    Log::info() << Here() << std::endl;

    std::map<std::string, std::string> dict;

    eckit::RendezvousHash rendezvous(&RendezvousHash::md5);
    dict["class"]  = "od";
    dict["stream"] = "oper";
    dict["type"]   = "fc";

    std::vector<size_t> indices;
    EXPECT_THROWS_AS(rendezvous.hashOrder(dict, indices), eckit::BadParameter); /* throw on empty node list */

    rendezvous.addNode("node01");
    rendezvous.addNode("node02");

    EXPECT_NO_THROW(rendezvous.hashOrder(dict, indices));
}

CASE("test_eckit_utils_rendezvous_hash_add_node") {
    Log::info() << Here() << std::endl;

    eckit::Translator<size_t, std::string> toStr;

    std::vector<std::string> params{"2t", "2d", "z", "w"};

    std::map<std::string, std::string> dict{{"class", "od"}, {"stream", "oper"}, {"type", "fc"}};

    eckit::RendezvousHash rendezvous{std::vector<std::string>({"node01", "node02"})};


    const size_t nsteps  = 25;
    const size_t nlevels = 100;


    std::map<std::string, size_t> counts;
    std::vector<std::string> ordered_nodes;

    for (std::vector<std::string>::iterator param = params.begin(); param != params.end(); ++param) {

        dict["param"] = *param;

        for (size_t step = 0; step < nsteps; ++step) {

            dict["step"] = toStr(step);

            for (size_t level = 0; level < nlevels; ++level) {

                dict["level"] = toStr(level);

                rendezvous.hashOrder(dict, ordered_nodes);
                EXPECT(ordered_nodes.size() == 2);
                counts[ordered_nodes[0]]++;
            }
        }
    }

    // Test that we have << roughly >> similar counts on all the nodes. (Very stochastic).
    for (auto kv : counts) {
        EXPECT(4950 < kv.second && 5050 > kv.second);
    }

    std::map<std::string, size_t> counts2;

    rendezvous.addNode("node03");
    rendezvous.addNode("node04");

    for (std::vector<std::string>::iterator param = params.begin(); param != params.end(); ++param) {

        dict["param"] = *param;

        for (size_t step = 0; step < nsteps; ++step) {

            dict["step"] = toStr(step);

            for (size_t level = 0; level < nlevels; ++level) {

                dict["level"] = toStr(level);

                rendezvous.hashOrder(dict, ordered_nodes);
                EXPECT(ordered_nodes.size() == 4);
                counts2[ordered_nodes[0]]++;
            }
        }
    }

    // Test that we have << roughly >> similar counts on all the nodes. (Very stochastic).
    for (auto kv : counts2) {
        EXPECT(2430 < kv.second && 2551 > kv.second);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
