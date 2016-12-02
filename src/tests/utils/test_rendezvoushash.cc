/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_utils

#include "ecbuild/boost_test_framework.h"

#include "eckit/utils/RendezvousHash.h"
#include "eckit/utils/MD5.h"
#include "eckit/log/Log.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Translator.h"

using namespace std;
using namespace eckit;



namespace eckit_test {};

//----------------------------------------------------------------------------------------------------------------------

using namespace eckit_test;

BOOST_AUTO_TEST_SUITE( test_eckit_rendezvous_hash )

static  std::string md5_hasher(const std::string& str) {
    eckit::MD5 md5(str.c_str(), str.size());
    return md5.digest();
}


BOOST_AUTO_TEST_CASE( test_eckit_utils_rendezvous_hash_constructor )
{
    std::set<std::string> nodes;

    nodes.insert("node01");
    nodes.insert("node02");
    nodes.insert("node03");
    nodes.insert("node04");

    eckit::RendezvousHash rendezvous(nodes, &md5_hasher);

    std::map<std::string, std::string> dict;

    dict["class"]  = "od";
    dict["stream"] = "oper";
    dict["type"]   = "fc";
    dict["level"]  = "1";

    std::string n = rendezvous.selectNode(dict);

    eckit::Log::info() << n  << std::endl;

    BOOST_CHECK_EQUAL( nodes.count(n), 1 );
}

BOOST_AUTO_TEST_CASE( test_eckit_utils_rendezvous_hash_distribution )
{

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

    eckit::RendezvousHash rendezvous(nodes, &md5_hasher);

    std::map<std::string, std::string> dict;

    dict["class"]  = "od";
    dict["stream"] = "oper";
    dict["type"]   = "fc";

    std::string n = rendezvous.selectNode(dict);

    eckit::Log::info() << n  << std::endl;

    std::map<std::string, size_t> counts;

    eckit::Translator<size_t,std::string> toStr;

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

    eckit::Log::info() << counts << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
