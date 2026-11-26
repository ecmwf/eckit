// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File Watcher.h
// Baudouin Raoult - ECMWF Jun 98

#ifndef eckit_Watcher_h
#define eckit_Watcher_h

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Watcher {
public:

    // -- Methods
    virtual ~Watcher() {}

    virtual void watch() = 0;

    // -- Class methods

    static Watcher& dummy();
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
