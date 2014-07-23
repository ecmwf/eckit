/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "ecbuild/boost_test_framework.h"

#include "eckit/memory/Owned.h"
#include "eckit/memory/SharedPtr.h"
#include "eckit/memory/Builder.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

using namespace std;
using namespace eckit;

/// These tests are similar to the test for boost scoped_ptr and shared ptrs
/// This allows as in the future to drop out, our own home grown managed
/// ptr's in favour of the standards.

//-----------------------------------------------------------------------------

namespace eckit_test {

class Base0 : public Owned {
public:

	typedef SharedPtr<Base0> Ptr;

	static std::string className() { return "eckit_test.Base1"; }

	virtual ~Base0() {}
	virtual std::string foo() const = 0;

};

class A0 : public Base0 {
public:

	static std::string className() { return "eckit_test.A1"; }

	A0() : s1_( "A.0" ) {}

	virtual std::string foo() const { return className() + "." + s1_; }

private:

	std::string s1_;
};

class B0 : public Base0 {
public:

	static std::string className() { return "eckit_test.B1"; }

	B0() : s2_( "B.0") {}

	virtual std::string foo() const { return className() + "." + s2_; }

private:

	std::string s2_;
};

//ConcreteBuilder<Base0,A0> builder_A0;
//ConcreteBuilder<Base0,B0> builder_B0;

}

//-----------------------------------------------------------------------------

namespace eckit_test {

class Base1 : public Owned {
public:

	typedef SharedPtr<Base1> Ptr;

	static std::string className() { return "eckit_test.Base1"; }

	virtual ~Base1() {}
	virtual std::string foo() const = 0;

};

class A1 : public Base1 {
public:

	static std::string className() { return "eckit_test.A1"; }

	A1( const Params& p ) : s1_( p["mystr"].as<string>() + ".1") {}

	virtual std::string foo() const { return className() + "." + s1_; }

private:

	std::string s1_;
};

class B1 : public Base1 {
public:

	static std::string className() { return "eckit_test.B1"; }

	B1( const Params& p ) : s2_( p["mystr"].as<string>() + ".2") {}

	virtual std::string foo() const { return className() + "." + s2_; }

private:

	std::string s2_;
};

ConcreteBuilder<Base1,A1> builder_A1;
ConcreteBuilder<Base1,B1> builder_B1;

}

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_memory_factory )

BOOST_AUTO_TEST_CASE( test_eckit_memory_factory_1param )
{
	using namespace eckit_test;

	std::cout << Factory<Base1>::instance() << std::endl;

	BOOST_CHECK( Factory<Base1>::instance().size() == 2 );

	BOOST_CHECK( Factory<Base1>::instance().exists( "eckit_test.A1" ) );
	BOOST_CHECK( Factory<Base1>::instance().exists( "eckit_test.B1" ) );

	BOOST_CHECK( Factory<Base1>::instance().get( "eckit_test.A1" ).name() == "eckit_test.A1" );
	BOOST_CHECK( Factory<Base1>::instance().get( "eckit_test.B1" ).name() == "eckit_test.B1" );

	ValueParams p;
	p.set("mystr","lolo");

	Base1::Ptr p1 ( Factory<Base1>::instance().get( "eckit_test.A1" ).create(p) );
	Base1::Ptr p2 ( Factory<Base1>::instance().get( "eckit_test.B1" ).create(p) );

	BOOST_CHECK( p1 );
	BOOST_CHECK( p2 );

	BOOST_CHECK_EQUAL( p1->foo(), "eckit_test.A1.lolo.1" );
	BOOST_CHECK_EQUAL( p2->foo(), "eckit_test.B1.lolo.2" );
}

BOOST_AUTO_TEST_SUITE_END()
