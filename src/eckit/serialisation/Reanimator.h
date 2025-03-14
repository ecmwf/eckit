/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Reanimator.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_Reanimator_h
#define eckit_Reanimator_h

#include "eckit/serialisation/Stream.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Streamable;

// -----------------------------------------------------------------------------
// The class Streamable is a abstract class for encoding/decoding objects to/from
// a Stream.
//
// For a class to be "stream-able", it must:
//
//   1- Inherit from class Streamable
//   2- Have a constructor from a Stream&
//   3- Support a encode(Stream&) const method
//
//   When encoded, it must first encode its superclass, then all its members
//   objects, and write is simple members ...
//
// For a class 'Foo' to be "reanimate-able", i.e. to be recreated from a stream
// as a first class object, it must be "stream-able" and it must have the two following
// private members:
//
//    static  ClassSpec        classSpec_;
//    static  Reanimator<Foo> reanimator_;
//
// and the two public methods to access those members:
//
//	const ReanimatorBase& reanimator() const override { return reanimator_; }
//	static  const ClassSpec&  classSpec()        { return classSpec_; }
//
// -----------------------------------------------------------------------------
// A ClassSpec decribes the class:
//

struct ClassSpec {
    const ClassSpec* superClass_;  // Pointer to superclass ClassSpec  (Note that
                                   // multiple inheritance is not supported.)
    const char* name_;             // class name
};

// in the file Foo.cc, if Foo is a direct subclass of Bar, its ClassSpec can be
// defined as:
//
// ClassSpec Foo::classSpec_ = { &Bar::ClassSpec(), "Foo", 1, };
//
// and the reanimator as
//
// Reanimator<Foo> Foo::reanimator_;
//
// -----------------------------------------------------------------------------
// A ReanimatorBase is an 'factory' used to create objects from a Stream
//

class ReanimatorBase {
    const ClassSpec& spec_;

    virtual Streamable* ressucitate(Stream&) const = 0;

public:

    ReanimatorBase(const ClassSpec&);
    virtual ~ReanimatorBase();
    const ClassSpec& spec() const { return spec_; }

    static Streamable* reanimate(Stream&, const ClassSpec* = 0);
};


// -----------------------------------------------------------------------------

template <class T>
class Reanimator : public ReanimatorBase {
    Streamable* ressucitate(Stream& s) const;

public:

    Reanimator();
    static T* reanimate(Stream& s);
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#include "eckit/serialisation/Reanimator.cc"

#endif
