/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

CASE ( "test_eckit_utils_rendezvous_hash_constructor" )
{
    Log::info() << Here() << std::endl;

    std::set<std::string> nodes;

    nodes.insert("node01");
    nodes.insert("node02");
    nodes.insert("node03");
    nodes.insert("node04");

    eckit::RendezvousHash rendezvous(nodes, &RendezvousHash::md5);

    std::map<std::string, std::string> dict;

    dict["class"]  = "od";
    dict["stream"] = "oper";
    dict["type"]   = "fc";
    dict["level"]  = "1";

    std::string n = rendezvous.selectNode(dict);

    Log::info() << n << std::endl;

    EXPECT( nodes.count(n) == 1 );
}

CASE ( "test_eckit_utils_rendezvous_hash_distribution" )
{
    Log::info() << Here() << std::endl;

    eckit::Translator<size_t,std::string> toStr;

    std::vector<std::string> params;

    params.push_back( "2t" );
    params.push_back( "2d" );
    params.push_back( "z" );
    params.push_back( "u" );
    params.push_back( "v" );
    params.push_back( "130.128" );
    params.push_back( "138.128" );


    std::set<std::string> nodes;

    nodes.insert("node01");
    nodes.insert("node02");
    nodes.insert("node03");
    nodes.insert("node04");
    nodes.insert("node05");
    nodes.insert("node06");
    nodes.insert("node07");

    eckit::RendezvousHash rendezvous(nodes);

    std::map<std::string, std::string> dict;
    dict["class"]  = "od";
    dict["stream"] = "oper";
    dict["type"]   = "fc";

    std::map<std::string, size_t> counts;


    for(std::vector<std::string>::iterator param = params.begin(); param != params.end(); ++param ) {

        dict["param"] = *param;

        for(size_t step = 0; step < 240; ++step) {

            dict["step"]  = toStr(step);

            for( size_t level = 0; level < 10; ++level ) {

                dict["level"]  = toStr(level);

                counts[ rendezvous.selectNode(dict) ]++;

            }
        }

    }

    Log::info() << counts << std::endl;
}

CASE ( "test_eckit_utils_rendezvous_hash_empty_dict" )
{
    Log::info() << Here() << std::endl;

    std::map<std::string, std::string> dict;

    eckit::RendezvousHash rendezvous;
    rendezvous.addNode("node01");
    rendezvous.addNode("node02");

    EXPECT_NO_THROW(rendezvous.selectNode(dict)); /* don't throw on empty dictionary */
}

CASE ( "test_eckit_utils_rendezvous_hash_throws_empty_node_list" )
{
    Log::info() << Here() << std::endl;

    std::map<std::string, std::string> dict;

    eckit::RendezvousHash rendezvous(&RendezvousHash::md5);
    dict["class"]  = "od";
    dict["stream"] = "oper";
    dict["type"]   = "fc";

    EXPECT_THROWS_AS( rendezvous.selectNode(dict), eckit::BadParameter ); /* throw on empty node list */

    rendezvous.addNode("node01");
    rendezvous.addNode("node02");

    EXPECT_NO_THROW( rendezvous.selectNode(dict) );
}

CASE ( "test_eckit_utils_rendezvous_hash_add_node" )
{
    Log::info() << Here() << std::endl;

    eckit::Translator<size_t,std::string> toStr;

    std::vector<std::string> params;
    params.push_back( "2t" );
    params.push_back( "2d" );
    params.push_back( "z" );
    params.push_back( "w" );

    std::map<std::string, std::string> dict;
    dict["class"]  = "od";
    dict["stream"] = "oper";
    dict["type"]   = "fc";

    eckit::RendezvousHash rendezvous;

    rendezvous.addNode("node01");
    rendezvous.addNode("node02");


    const size_t nsteps  = 25;
    const size_t nlevels = 100;


    std::map<std::string, size_t> counts;

    for(std::vector<std::string>::iterator param = params.begin(); param != params.end(); ++param ) {

        dict["param"] = *param;

        for(size_t step = 0; step < nsteps; ++step) {

            dict["step"]  = toStr(step);

            for( size_t level = 0; level < nlevels; ++level ) {

                dict["level"]  = toStr(level);

                counts[ rendezvous.selectNode(dict) ]++;

            }
        }
    }

    Log::info() << counts << std::endl;

    std::map<std::string, size_t> counts2;

    rendezvous.addNode("node03");
    rendezvous.addNode("node04");

    for(std::vector<std::string>::iterator param = params.begin(); param != params.end(); ++param ) {

        dict["param"] = *param;

        for(size_t step = 0; step < nsteps; ++step) {

            dict["step"]  = toStr(step);

            for( size_t level = 0; level < nlevels; ++level ) {

                dict["level"]  = toStr(level);

                counts2[ rendezvous.selectNode(dict) ]++;

            }
        }
    }

    Log::info() << counts2 << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace test
} // namespace eckit

int main (int argc, char * argv[]) {
    return run_tests( argc, argv );
}
