/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TransferWatcher.h
// Baudouin Raoult - ECMWF Jun 98

#ifndef eckit_TransferWatcher_h
#define eckit_TransferWatcher_h

//-----------------------------------------------------------------------------

namespace eckit {

class Offset;

//-----------------------------------------------------------------------------

class TransferWatcher {
public:

    // -- Methods

    virtual void watch(const void*, long) = 0;
    virtual void restartFrom(const Offset&) {}
    virtual void fromHandleOpened() {}
    virtual void toHandleOpened() {}

    virtual ~TransferWatcher() {}

    // -- Class methods

    static TransferWatcher& dummy();
};


//-----------------------------------------------------------------------------

}  // namespace eckit


#endif
