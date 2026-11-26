// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
