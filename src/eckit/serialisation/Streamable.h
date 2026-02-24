/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date May 1996

#ifndef eckit_Streamable_h
#define eckit_Streamable_h

#include "eckit/serialisation/Reanimator.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {

//-----------------------------------------------------------------------------

class Streamable {
public:

    friend Stream& operator<<(Stream&, const Streamable&);

    // -- Contructors

    Streamable() {}

    Streamable(Stream&);

    Streamable(const Stream&)            = delete;
    Streamable& operator=(const Stream&) = delete;
    Streamable(Stream&&)                 = delete;
    Streamable& operator=(Stream&&)      = delete;

    // -- Destructor

    virtual ~Streamable() {}

    // -- Methods

    virtual std::string className() const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

    // -- Operators

    // void *operator new(size_t s)          { return MemoryPool::fastAllocate(s);}
    // void *operator new(size_t s,void *p)  { return p;                          }
    // void operator delete(void* p)         { MemoryPool::fastDeallocate(p);     }

protected:

    // -- Methods

    virtual void encode(Stream&) const;

    bool sameClass(const Streamable&) const;

private:

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<Streamable> reanimator_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
