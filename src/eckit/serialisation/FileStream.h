/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
///
/// @date   May 96

#ifndef eckit_serialisation_FileStream_h
#define eckit_serialisation_FileStream_h

#include "eckit/filesystem/PathName.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/io/StdFile.h"


namespace eckit {

/// Stream to serialise to FILE*

class FileStream : public Stream {

public: // methods

    FileStream(const PathName& name, const char *mode);

    virtual ~FileStream();

    // Overriden from Stream

	virtual long read(void*,long);
	virtual long write(const void*,long);
	virtual void rewind();

    void close();

protected: // members

    StdFile     file_;
    bool        read_;
    PathName    name_;

protected: // methods

    // Overriden from Stream

	virtual std::string name() const;

    virtual void print(std::ostream& s) const;

};

} // namespace eckit

#endif
