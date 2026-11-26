// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
