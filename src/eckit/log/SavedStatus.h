/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file SavedStatus.h
/// @author Tiago Quintino

#ifndef eckit_log_SavedStatus_h
#define eckit_log_SavedStatus_h

#include <string>


namespace eckit {

//-----------------------------------------------------------------------------

/// Saves and Restores Monitor status line
class SavedStatus {
    std::string status_;

public:

    SavedStatus();
    ~SavedStatus();
};

//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
