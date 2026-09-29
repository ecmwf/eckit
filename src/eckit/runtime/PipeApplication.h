// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file PipeApplication.h
/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @author Florian Rathgeber

#ifndef eckit_PipeApplication_h
#define eckit_PipeApplication_h

#include "eckit/config/Resource.h"
#include "eckit/runtime/Application.h"


namespace eckit {

class Stream;

//----------------------------------------------------------------------------------------------------------------------

class PipeApplication : public Application {

public:  // methods

    PipeApplication(int argc, char** argv, const char* homeenv);

    ~PipeApplication() override;

    virtual void process(Stream&) = 0;
    virtual void endBatch();
    virtual void init(Stream&);
    virtual void waiting();

    static void launch(const std::string& name, int in, int out);

private:  // members

    Resource<long> in_;
    Resource<long> out_;

    /// overridden from Application
    void run() override;

protected:  // members

    bool terminate_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
