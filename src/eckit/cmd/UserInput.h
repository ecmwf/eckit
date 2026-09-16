// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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

    using completion_proc = bool (*)(const char*, int pos, char* insert, int insertmax);

    static const char* getUserInput(const char* prompt, completion_proc callback = nullptr);

    static void saveHistory(const char* path, int max = 0);
    static void loadHistory(const char* path);

    static void printHistory(int max = 0);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
