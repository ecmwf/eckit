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
#include "eckit/testing/Setup.h"

namespace eckit {
namespace test {
    template<typename T> std::vector<T> make_vector( const T& t1, const T& t2 ) {
        std::vector<T> result;
        result.push_back(t1);
        result.push_back(t2);
        return result;
    }
    template<typename T> std::vector<T> make_vector( const T& t1, const T& t2, const T& t3 ) {
        std::vector<T> result;
        result.push_back(t1);
        result.push_back(t2);
        result.push_back(t3);
        return result;
    }
}
}

using namespace std;
using namespace eckit;
using namespace eckit::test;
using namespace eckit::testing;

//-----------------------------------------------------------------------------

BOOST_GLOBAL_FIXTURE( Setup );

BOOST_AUTO_TEST_SUITE( test_eckit_config )

BOOST_AUTO_TEST_CASE( test_configuration_interface )
{
    bool                value_bool            = bool(true); 
    int                 value_int             = int(1);
    long                value_long            = long(2);
    size_t              value_size_t          = size_t(3);
    float               value_float           = float(1.234567);
    double              value_double          = double(1.2345678912345789123456789);
    std::string         value_string          = std::string("string");
    std::vector<int>    value_arr_int         = make_vector(1,2,3);
    std::vector<long>   value_arr_long        = make_vector(4l,5l);
    std::vector<size_t> value_arr_size_t      = make_vector(6ul,7ul);
    std::vector<float>  value_arr_float       = make_vector(1.234567f,2.345678f);
    std::vector<double> value_arr_double      = make_vector(1.234567,2.345678);
    std::vector<std::string> value_arr_string = make_vector(std::string("hello"),std::string("world"));
  
    bool                      result_bool;
    int                       result_int;
    long                      result_long;
    size_t                    result_size_t;
    float                     result_float;
    double                    result_double;
    std::string               result_string;
    std::vector<int>          result_arr_int;
    std::vector<long>         result_arr_long;
    std::vector<size_t>       result_arr_size_t;
    std::vector<float>        result_arr_float;
    std::vector<double>       result_arr_double;
    std::vector<std::string>  result_arr_string;

    LocalConfiguration local;
    {
        local.set("bool",       value_bool      );
        local.set("int",        value_int       );
        local.set("long",       value_long      );
        local.set("size_t",     value_size_t    );
        local.set("float",      value_float     );
        local.set("double",     value_double    );
        local.set("string",     value_string    );
        local.set("arr_int",    value_arr_int   );
        local.set("arr_long",   value_arr_long  );
        local.set("arr_size_t", value_arr_size_t);
        local.set("arr_float",  value_arr_float );
        local.set("arr_double", value_arr_double);
        local.set("arr_string", value_arr_string);
    }
    const Configuration& conf = local;
  
    BOOST_CHECK( ! conf.get("missing",result_bool)       );
    BOOST_CHECK( ! conf.get("missing",result_int)        );
    BOOST_CHECK( ! conf.get("missing",result_long)       );
    BOOST_CHECK( ! conf.get("missing",result_size_t)     );
    BOOST_CHECK( ! conf.get("missing",result_float)      );
    BOOST_CHECK( ! conf.get("missing",result_double)     );
    BOOST_CHECK( ! conf.get("missing",result_string)     );
    BOOST_CHECK( ! conf.get("missing",result_arr_int)    );
    BOOST_CHECK( ! conf.get("missing",result_arr_long)   );
    BOOST_CHECK( ! conf.get("missing",result_arr_size_t) );
    BOOST_CHECK( ! conf.get("missing",result_arr_float)  );
    BOOST_CHECK( ! conf.get("missing",result_arr_double) );
    BOOST_CHECK( ! conf.get("missing",result_arr_string) );
  
    BOOST_CHECK( conf.get("bool",       result_bool)       );
    BOOST_CHECK( conf.get("int",        result_int)        );
    BOOST_CHECK( conf.get("long",       result_long)       );
    BOOST_CHECK( conf.get("size_t",     result_size_t)     );
    BOOST_CHECK( conf.get("float",      result_float)      );
    BOOST_CHECK( conf.get("double",     result_double)     );
    BOOST_CHECK( conf.get("string",     result_string)     );
    BOOST_CHECK( conf.get("arr_int",    result_arr_int)    );
    BOOST_CHECK( conf.get("arr_long",   result_arr_long)   );
    BOOST_CHECK( conf.get("arr_size_t", result_arr_size_t) );
    BOOST_CHECK( conf.get("arr_float",  result_arr_float)  );
    BOOST_CHECK( conf.get("arr_double", result_arr_double) );
    BOOST_CHECK( conf.get("arr_string", result_arr_string) );
  
    BOOST_CHECK_EQUAL( result_bool,       value_bool      );
    BOOST_CHECK_EQUAL( result_int,        value_int       );
    BOOST_CHECK_EQUAL( result_long,       value_long      );
    BOOST_CHECK_EQUAL( result_size_t,     value_size_t    );
    BOOST_CHECK_EQUAL( result_float,      value_float     );
    BOOST_CHECK_EQUAL( result_double,     value_double    );
    BOOST_CHECK_EQUAL( result_string,     value_string    );
    BOOST_CHECK_EQUAL( result_arr_int,    value_arr_int   );
    BOOST_CHECK_EQUAL( result_arr_long,   value_arr_long  );
    BOOST_CHECK_EQUAL( result_arr_size_t, value_arr_size_t);
    BOOST_CHECK_EQUAL( result_arr_float,  value_arr_float );
    BOOST_CHECK_EQUAL( result_arr_double, value_arr_double);
    BOOST_CHECK_EQUAL( result_arr_string, value_arr_string);

    BOOST_CHECK_EQUAL( conf.getBool          ("bool"),       value_bool       );
    BOOST_CHECK_EQUAL( conf.getInt           ("int"),        value_int        );
    BOOST_CHECK_EQUAL( conf.getLong          ("long"),       value_long       );
    BOOST_CHECK_EQUAL( conf.getUnsigned      ("size_t"),     value_size_t     );
    BOOST_CHECK_EQUAL( conf.getFloat         ("float"),      value_float      );
    BOOST_CHECK_EQUAL( conf.getDouble        ("double"),     value_double     );
    BOOST_CHECK_EQUAL( conf.getString        ("string"),     value_string     );
    BOOST_CHECK_EQUAL( conf.getIntVector     ("arr_int"),    value_arr_int    );
    BOOST_CHECK_EQUAL( conf.getLongVector    ("arr_long"),   value_arr_long   );
    BOOST_CHECK_EQUAL( conf.getUnsignedVector("arr_size_t"), value_arr_size_t );
    BOOST_CHECK_EQUAL( conf.getFloatVector   ("arr_float"),  value_arr_float  );
    BOOST_CHECK_EQUAL( conf.getDoubleVector  ("arr_double"), value_arr_double );
    BOOST_CHECK_EQUAL( conf.getStringVector  ("arr_string"), value_arr_string );

    // Get throwing

    BOOST_CHECK_THROW( conf.getBool          ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getInt           ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getLong          ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getUnsigned      ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getFloat         ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getDouble        ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getString        ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getIntVector     ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getLongVector    ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getUnsignedVector("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getFloatVector   ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getDoubleVector  ("missing"), eckit::Exception );
    BOOST_CHECK_THROW( conf.getStringVector  ("missing"), eckit::Exception );

  
    // Get with default values instead of throwing
    BOOST_CHECK_EQUAL( conf.getBool          ("missing",value_bool),       value_bool       );
    BOOST_CHECK_EQUAL( conf.getInt           ("missing",value_int),        value_int        );
    BOOST_CHECK_EQUAL( conf.getLong          ("missing",value_long),       value_long       );
    BOOST_CHECK_EQUAL( conf.getUnsigned      ("missing",value_size_t),     value_size_t     );
    BOOST_CHECK_EQUAL( conf.getFloat         ("missing",value_float),      value_float      );
    BOOST_CHECK_EQUAL( conf.getDouble        ("missing",value_double),     value_double     );
    BOOST_CHECK_EQUAL( conf.getString        ("missing",value_string),     value_string     );
    BOOST_CHECK_EQUAL( conf.getIntVector     ("missing",value_arr_int),    value_arr_int    );
    BOOST_CHECK_EQUAL( conf.getLongVector    ("missing",value_arr_long),   value_arr_long   );
    BOOST_CHECK_EQUAL( conf.getUnsignedVector("missing",value_arr_size_t), value_arr_size_t );
    BOOST_CHECK_EQUAL( conf.getFloatVector   ("missing",value_arr_float ), value_arr_float  );
    BOOST_CHECK_EQUAL( conf.getDoubleVector  ("missing",value_arr_double), value_arr_double );
    BOOST_CHECK_EQUAL( conf.getStringVector  ("missing",value_arr_string), value_arr_string );

}

//-----------------------------------------------------------------------------


BOOST_AUTO_TEST_CASE( test_json_configuration )
{
  PathName jsonpath = "test_json_configuration.json";

  std::string jsonstr(
    "\n" "{"
    "\n" "  \"manager\" : { \"name\" : \"Sidonia\" , \"office\" : 1 },"
    "\n" "  \"staff\" : ["
    "\n" "    { \"name\" : \"Suske\" , \"office\" : 2 },"
    "\n" "    { \"name\" : \"Wiske\" , \"office\" : 3 }"
    "\n" "  ]"
    "\n" "}"
    "\n");

  {
    std::ofstream jsonfile (jsonpath.localPath());
    jsonfile << jsonstr;
  }

  JSONConfiguration conf_from_str(jsonstr);
  JSONConfiguration conf(jsonpath);
  LocalConfiguration manager;
  std::vector<LocalConfiguration> staff;

  BOOST_CHECK( conf.get("manager",manager) );
  BOOST_CHECK( conf.get("staff",staff) );

  std::string name;
  int office;

  BOOST_CHECK( manager.get("name",name) );
  BOOST_CHECK( manager.get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Sidonia") );
  BOOST_CHECK_EQUAL( office, 1 );

  BOOST_CHECK( staff[0].get("name",name) );
  BOOST_CHECK( staff[0].get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Suske") );
  BOOST_CHECK_EQUAL( office, 2 );

  BOOST_CHECK( staff[1].get("name",name) );
  BOOST_CHECK( staff[1].get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Wiske") );
  BOOST_CHECK_EQUAL( office, 3 );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE( test_local_configuration )
{
  LocalConfiguration local;
  {
    LocalConfiguration manager;
    manager.set("name","Sidonia");
    manager.set("office",1);

    std::vector<LocalConfiguration> staff(2);
    staff[0].set("name","Suske");
    staff[1].set("name","Wiske");
    staff[0].set("office",2);
    staff[1].set("office",3);

    local.set("manager",manager);
    local.set("staff",staff);
  }
  const Configuration& conf = local;

  LocalConfiguration manager;
  std::vector<LocalConfiguration> staff;

  BOOST_CHECK( conf.get("manager",manager) );
  BOOST_CHECK( conf.get("staff",staff) );

  std::string name;
  int office;

  BOOST_CHECK( manager.get("name",name) );
  BOOST_CHECK( manager.get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Sidonia") );
  BOOST_CHECK_EQUAL( office, 1 );

  BOOST_CHECK( staff[0].get("name",name) );
  BOOST_CHECK( staff[0].get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Suske") );
  BOOST_CHECK_EQUAL( office, 2 );

  BOOST_CHECK( staff[1].get("name",name) );
  BOOST_CHECK( staff[1].get("office",office) );
  BOOST_CHECK_EQUAL( name, std::string("Wiske") );
  BOOST_CHECK_EQUAL( office, 3 );
}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()
