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

using namespace std;
using namespace eckit;

namespace eckit {
namespace test {

template <typename T>
class TestItem : public Streamable {

public:
	TestItem(const T& s) : payload_(s) {}

	TestItem(Stream& s) : Streamable(s), payload_() {
		s >> payload_;
	}

	// From Streamble
	virtual void encode(eckit::Stream& s) const {
		Streamable::encode(s);
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

typedef unsigned char uchar;

template <typename T>
Reanimator< TestItem<T> > TestItem<T>::reanimator_;

/// Partially specialise ClassSpecs since they must have unique names
template <>
ClassSpec TestItem<char>::classSpec_ = {&Streamable::classSpec(),"TestItemChar",};
template <>
ClassSpec TestItem<uchar>::classSpec_ = {&Streamable::classSpec(),"TestItemUChar",};
template <>
ClassSpec TestItem<bool>::classSpec_ = {&Streamable::classSpec(),"TestItemBool",};
template <>
ClassSpec TestItem<int>::classSpec_ = {&Streamable::classSpec(),"TestItemInt",};
template <>
ClassSpec TestItem<uint>::classSpec_ = {&Streamable::classSpec(),"TestItemUInt",};
template <>
ClassSpec TestItem<long>::classSpec_ = {&Streamable::classSpec(),"TestItemLong",};
template <>
ClassSpec TestItem<ulong>::classSpec_ = {&Streamable::classSpec(),"TestItemULong",};
template <>
ClassSpec TestItem<string>::classSpec_ = {&Streamable::classSpec(),"TestItemString",};

#define test_decode(TYPE, INITIAL) \
BOOST_AUTO_TEST_CASE( test_decode_##TYPE ) \
{ \
	BOOST_TEST_MESSAGE("Manually (de)serialise Streamable with " #TYPE " member"); \
	PathName filename = PathName::unique( "data" ); \
	std::string filepath = filename.asString(); \
	TestItem<TYPE> t(INITIAL); \
	{ \
		FileStream sout( filepath.c_str(), "w" ); \
		t.encode(sout); \
	} \
	{ \
		FileStream sin( filepath.c_str(), "r" ); \
		TestItem<TYPE> t2(sin); \
		BOOST_TEST_MESSAGE("original: " << t.payload_); \
		BOOST_TEST_MESSAGE("streamed: " << t2.payload_); \
		BOOST_CHECK(t.payload_ == t2.payload_); \
	} \
	if (filename.exists()) filename.unlink(); \
}

#define test_reanimate(TYPE, INITIAL) \
BOOST_AUTO_TEST_CASE( test_reanimate_##TYPE ) \
{ \
	BOOST_TEST_MESSAGE("(de)serialise Streamable with " #TYPE " member via Reanimator"); \
	PathName filename = PathName::unique( "data" ); \
	std::string filepath = filename.asString(); \
	TestItem<TYPE> t(INITIAL); \
	{ \
		FileStream sout( filepath.c_str(), "w" ); \
		sout << t; \
	} \
	{ \
		FileStream sin( filepath.c_str(), "r" ); \
		TestItem<TYPE>* t2 = eckit::Reanimator< TestItem<TYPE> >::reanimate(sin); \
		BOOST_TEST_MESSAGE("orginal: " << t.payload_); \
		BOOST_TEST_MESSAGE("streamed: " << t2->payload_); \
		BOOST_CHECK(t.payload_ == t2->payload_); \
	} \
	if (filename.exists()) filename.unlink(); \
}

BOOST_AUTO_TEST_SUITE( TestStreamable )

test_decode(char, 'a')
test_reanimate(char, 'a')

test_decode(uchar, 'b')
test_reanimate(uchar, 'b')

test_decode(bool, true)
test_reanimate(bool, true)

test_decode(int, 2147483647)
test_reanimate(int, 2147483647)

test_decode(uint, 4294967295)
test_reanimate(uint, 4294967295)

test_decode(long, 2147483647)
test_reanimate(long, 2147483647)

test_decode(ulong, 4294967295)
test_reanimate(ulong, 4294967295)

test_decode(string, "2147483647")
test_reanimate(string, "2147483647")

BOOST_AUTO_TEST_SUITE_END()

} // namespace test
} // namespace hermes
