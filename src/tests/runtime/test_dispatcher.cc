/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. In applying
 * this licence, ECMWF does not waive the privileges and immunities granted to it by virtue
 * of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_runtime_dispatcher

#include "ecbuild/boost_test_framework.h"

#include "eckit/runtime/Dispatcher.h"

using namespace eckit;

//-----------------------------------------------------------------------------

namespace eckit_test {

struct TestResource {
	TestResource() : i(0) {}
	int i;
};

struct TestRequest {
	TestRequest(TestResource& r) : resource(r) {}

	void json(JSON & s) const
	{
		s << resource.i;
	}

	friend std::ostream& operator<<(std::ostream& s, const TestRequest& r)
	{
		s << r.resource.i << std::endl;
		return s;
	}

	TestResource& resource;
};

struct TestHandler : public DefaultHandler<TestRequest> {
	typedef std::vector<TestRequest*> Requests;
	void handle(Requests& requests) {
		for(Requests::iterator t = requests.begin(); t != requests.end(); ++t) {
			((*t)->resource.i)++;
		}
	}
};

struct Traits {
	static const char* name() { return "eckit"; }
	typedef TestRequest     Request;
	typedef TestHandler     Handler;
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE( test_eckit_runtime_dispatcher )

BOOST_AUTO_TEST_CASE( test_push_one )
{
	Dispatcher<Traits> d;
	TestResource r;
	d.push(new TestRequest(r));
	d.waitForAll();
	// All request should have been processed
	BOOST_CHECK( d.size() == 0 );
	BOOST_CHECK( d.running() == 0 );
	Log::debug() << "TestResource::i " << r.i << std::endl;
	BOOST_CHECK( r.i == 1 );
};

BOOST_AUTO_TEST_CASE( test_push_ten )
{
	Dispatcher<Traits> d;
	TestResource r;
	std::vector<TestRequest*> v;
	for (int i = 0; i < 10; ++i) {
		v.push_back(new TestRequest(r));
	}
	d.push(v);
	d.waitForAll();
	// All request should have been processed
	BOOST_CHECK( d.size() == 0 );
	BOOST_CHECK( d.running() == 0 );
	Log::debug() << "TestResource::i " << r.i << std::endl;
	BOOST_CHECK( r.i == 10 );
};

//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE_END()

} // namespace eckit_test
