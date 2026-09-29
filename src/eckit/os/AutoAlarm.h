// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File AutoAlarm.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_AutoAlarm_h
#define eckit_AutoAlarm_h

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class AutoAlarm {
public:

    // -- Contructors

    AutoAlarm(int, bool = false);

    // -- Destructor

    ~AutoAlarm();

    // - Class methods

    static bool caught() { return caught_; }

private:

    // No copy allowed

    AutoAlarm(const AutoAlarm&);
    AutoAlarm& operator=(const AutoAlarm&);

    // -- Members

    using proc = void (*)(int);
    proc old_;
    bool saveThrow_;
    int saveSec_;

    // -- Class members

    static bool caught_;
    static bool throw_;
    static int sec_;

    // -- Class methods

    static void sigAlarm(int);
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
