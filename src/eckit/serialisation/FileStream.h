// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
///
/// @date   May 96

#ifndef eckit_serialisation_FileStream_h
#define eckit_serialisation_FileStream_h

#include "eckit/filesystem/PathName.h"
#include "eckit/io/StdFile.h"
#include "eckit/serialisation/Stream.h"


namespace eckit {

/// Stream to serialise to FILE*

class FileStream : public Stream {

public:  // methods

    FileStream(const PathName& name, const char* mode);

    ~FileStream() override;

    // Overriden from Stream

    long read(void*, long) override;
    long write(const void*, long) override;
    void rewind() override;

    void close();

protected:  // members

    StdFile file_;
    bool read_;
    PathName name_;

protected:  // methods
            // Overriden from Stream

    std::string name() const override;

    void print(std::ostream& s) const override;
};

}  // namespace eckit

#endif
