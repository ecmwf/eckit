/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. In applying
 * this licence, ECMWF does not waive the privileges and immunities granted to it by virtue
 * of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/// @file test_streamable.cc
/// @date Jan 2015
/// @author Florian Rathgeber

#define BOOST_TEST_MODULE TestStreamable
#include "ecbuild/boost_test_framework.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/serialisation/FileStream.h"
#include "eckit/serialisation/Streamable.h"

using namespace eckit;

namespace eckit {
namespace test {

template <typename T>
class TestItem : public Streamable {

public:
	TestItem(const T& s) : payload_(s) {}

	TestItem(Stream& s) : Streamable(s), payload_(0) {
		s >> payload_;
	}

	// From Streamble
	virtual void encode(eckit::Stream& s) const {
		s << payload_;
	}
	virtual const eckit::ReanimatorBase& reanimator() const { return reanimator_; }

	// Class members
	static const eckit::ClassSpec& classSpec() { return classSpec_; }

	T payload_;

protected:

	virtual void print(std::ostream& s) const {
		s << "TestItem " << payload_;
	}

private:
	// -- Class members
	static eckit::ClassSpec            classSpec_;
	static eckit::Reanimator<TestItem> reanimator_;

	// -- Friends
	friend std::ostream& operator<<(std::ostream& s,const TestItem& p) {
		p.print(s); return s;
	}
};

template <typename T>
ClassSpec TestItem<T>::classSpec_ = {&Streamable::classSpec(),"TestItem",};
template <typename T>
Reanimator< TestItem<T> > TestItem<T>::reanimator_;

BOOST_AUTO_TEST_SUITE( TestStreamable )

BOOST_AUTO_TEST_CASE( test_decode_int )
{
	BOOST_TEST_MESSAGE("Manually (de)serialise Streamable with int member");

	PathName filename = PathName::unique( "data" );
	std::string filepath = filename.asString();
	TestItem<int> t(10);
	{
		FileStream sout( filepath.c_str(), "w" );
		t.encode(sout);
	}
	{
		FileStream sin( filepath.c_str(), "r" );
		TestItem<int> t2(sin);
		BOOST_CHECK(t.payload_ == t2.payload_);
	}
	if (filename.exists()) filename.unlink();
}

BOOST_AUTO_TEST_CASE( test_reanimate_int )
{
	BOOST_TEST_MESSAGE("(de)serialise Streamable with int member via Reanimator");

	PathName filename = PathName::unique( "data" );
	std::string filepath = filename.asString();
	TestItem<int> t(10);
	{
		FileStream sout( filepath.c_str(), "w" );
		sout << t;
	}
	{
		FileStream sin( filepath.c_str(), "r" );
		TestItem<int>* t2 = eckit::Reanimator< TestItem<int> >::reanimate(sin);
		BOOST_CHECK(t.payload_ == t2->payload_);
	}
	if (filename.exists()) filename.unlink();
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace hermes
