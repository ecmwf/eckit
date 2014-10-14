/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. In applying
 * this licence, ECMWF does not waive the privileges and immunities granted to it by virtue
 * of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE TestDispatcher
#include "ecbuild/boost_test_framework.h"

#include "eckit/runtime/Dispatcher.h"

using namespace eckit;

namespace eckit {
namespace test {

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

}
}
