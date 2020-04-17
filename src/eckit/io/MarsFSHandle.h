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
/// @date May 1996

#ifndef eckit_filesystem_MarsFSHandle_h
#define eckit_filesystem_MarsFSHandle_h

#include <memory>

#include "eckit/filesystem/marsfs/MarsFSFile.h"
#include "eckit/filesystem/marsfs/MarsFSPath.h"
#include "eckit/io/DataHandle.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MarsFSHandle : public DataHandle {
public:
    // -- Contructors

    MarsFSHandle(const MarsFSPath&, bool overwrite = false);
    MarsFSHandle(Stream&);

    // -- Destructor

    ~MarsFSHandle();

    // --  Methods

    void advance(const Length&);
    const MarsFSPath& path() const { return path_; }

    // -- Overridden methods

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*, long);
    virtual long write(const void*, long);
    virtual void close();
    virtual void rewind();
    virtual void skip(const Length& len);
    virtual Offset seek(const Offset&);
    virtual void print(std::ostream&) const;
    virtual Length size();
    virtual Length estimate();
    virtual Offset position();
    virtual bool isEmpty() const;
    virtual void restartReadFrom(const Offset& from);
    virtual void restartWriteFrom(const Offset& from);

    virtual DataHandle* toLocal();
    virtual void toLocal(Stream&) const;
    virtual void cost(std::map<std::string, Length>&, bool) const;

    virtual std::string title() const;
    virtual bool moveable() const { return true; }
    virtual void hash(MD5& md5) const;

    // From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:
    // -- Members

    MarsFSPath path_;
    bool read_;
    Length length_;
    Offset position_;
    bool overwrite_;

    std::unique_ptr<MarsFSFile> file_;

    // -- Methods

    void open(const char*);

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<MarsFSHandle> reanimator_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
