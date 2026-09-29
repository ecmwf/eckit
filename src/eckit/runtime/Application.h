// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   Application.h
/// @date   May 1996
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber

#ifndef eckit_Application_h
#define eckit_Application_h

#include "eckit/runtime/Main.h"
#include "eckit/runtime/Task.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Application : public Task, public Main {
public:

    static Application& instance();

    Application(int argc, char** argv, const char* homeenv = nullptr);

    ~Application() override;

    // -- Methods

    virtual void run() = 0;

    /// call this function once to make this application unique on this host
    void unique();

    // -- Overridden methods

    // From Task

    void start() override;
    void stop() override;
    void kill() override;
    void wait() override {}
    bool active() override { return true; }

    bool running() const { return running_; }

    void terminate() override;

    time_t uptime();

private:  // methods

    LogTarget* createInfoLogTarget() const override;
    LogTarget* createWarningLogTarget() const override;
    LogTarget* createErrorLogTarget() const override;
    LogTarget* createDebugLogTarget() const override;


    /// overriden from Configurable
    virtual std::string kind() const { return "Application"; }

private:  // members

    bool running_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
