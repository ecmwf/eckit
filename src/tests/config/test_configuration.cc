/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_configuration

#include "ecbuild/boost_test_framework.h"

#include <fstream>

#include "eckit/log/Log.h"
#include "eckit/config/LocalConfiguration.h"
#include "eckit/config/JSONConfiguration.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_config )

BOOST_AUTO_TEST_CASE( test_json_configuration )
{
  std::string jsonpath = "test_json_configuration.json";

  {
    std::string jsonstr(
      "\n" "{"
      "\n" "  \"manager\" : { \"name\" : \"Sidonia\" , \"office\" : 1 },"
      "\n" "  \"staff\" : ["
      "\n" "    { \"name\" : \"Suske\" , \"office\" : 2 },"
      "\n" "    { \"name\" : \"Wiske\" , \"office\" : 3 }"
      "\n" "  ]"
      "\n" "}"
      "\n");
    std::ofstream jsonfile (jsonpath);
    jsonfile << jsonstr;
  }

  JSONConfiguration conf(jsonpath);
  LocalConfiguration manager;
  std::vector<LocalConfiguration> staff;

  BOOST_CHECK( conf.get("manager",manager) );
  BOOST_CHECK( conf.get("staff",staff) );

  std::string name;
  long office;

  BOOST_CHECK( manager.get("name",name) );
  BOOST_CHECK( manager.get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Sidonia") );
  BOOST_CHECK_EQUAL( office, 1l );

  BOOST_CHECK( staff[0].get("name",name) );
  BOOST_CHECK( staff[0].get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Suske") );
  BOOST_CHECK_EQUAL( office, 2l );

  BOOST_CHECK( staff[1].get("name",name) );
  BOOST_CHECK( staff[1].get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Wiske") );
  BOOST_CHECK_EQUAL( office, 3l );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_local_configuration )
{
  LocalConfiguration local;
  {
    LocalConfiguration manager;
    manager.set("name","Sidonia");
    manager.set("office",1l);

    std::vector<LocalConfiguration> staff(2);
    staff[0].set("name","Suske");
    staff[1].set("name","Wiske");
    staff[0].set("office",2l);
    staff[1].set("office",3l);

    local.set("manager",manager);
    local.set("staff",staff);
  }
  const Configuration& conf = local;

  LocalConfiguration manager;
  std::vector<LocalConfiguration> staff;

  BOOST_CHECK( conf.get("manager",manager) );
  BOOST_CHECK( conf.get("staff",staff) );

  std::string name;
  long office;

  BOOST_CHECK( manager.get("name",name) );
  BOOST_CHECK( manager.get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Sidonia") );
  BOOST_CHECK_EQUAL( office, 1l );

  BOOST_CHECK( staff[0].get("name",name) );
  BOOST_CHECK( staff[0].get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Suske") );
  BOOST_CHECK_EQUAL( office, 2l );

  BOOST_CHECK( staff[1].get("name",name) );
  BOOST_CHECK( staff[1].get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Wiske") );
  BOOST_CHECK_EQUAL( office, 3l );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
