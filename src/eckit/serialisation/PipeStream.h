// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File PipeStream.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_PipeStream_h
#define eckit_PipeStream_h

#include "eckit/serialisation/Stream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// Stream using pipes

class PipeStream : public Stream {
public:

    // -- Contructors

    PipeStream();
    PipeStream(int, int);

    // -- Destructor

    ~PipeStream();

    // -- Methods

    // After a fork:

    void parentProcess();  // Notify that we are in the parent
    void childProcess();   // Notify that we are in the child

    int in() { return in_; }
    int out() { return out_; }

private:

    // -- Members

    int toChild_[2];
    int toParent_[2];

    int in_;
    int out_;

    // -- Overridden methods

    // From Stream

    long write(const void*, long) override;
    long read(void*, long) override;
    std::string name() const override { return "pipe"; }
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
