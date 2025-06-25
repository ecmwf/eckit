/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   UserInput.h
/// @author Baudouin Raoult
/// @date   Mar 2016

#ifndef eckit_cmd_UserInput_H
#define eckit_cmd_UserInput_H

#include <string>
#include <vector>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class UserInput {
public:

    typedef bool (*completion_proc)(const char*, int pos, char* insert, int insertmax);

    static const char* getUserInput(const char* prompt, completion_proc callback = 0);

    static void saveHistory(const char* path, int max = 0);
    static void loadHistory(const char* path);

    static void printHistory(int max = 0);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
