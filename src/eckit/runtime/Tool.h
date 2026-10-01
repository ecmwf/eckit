// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Tiago Quintino
/// @date Sep 2012

#ifndef eckit_Tool_h
#define eckit_Tool_h

#include "eckit/runtime/Main.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Tool : public Main {

public:  // methods

    Tool(int argc, char** argv, const char* homeenv = nullptr);

    ~Tool() override;

    int start();

    virtual void run() = 0;

    LogTarget* createWarningLogTarget() const override;
    LogTarget* createErrorLogTarget() const override;

protected:  // method

    void sendLogErrWarnToStdOut(bool send) { sendLogErrWarnToStdOut_ = send; }

private:  // members

    bool sendLogErrWarnToStdOut_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
