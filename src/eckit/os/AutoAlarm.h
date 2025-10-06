/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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

    typedef void (*proc)(int);
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
