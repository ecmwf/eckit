/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/PipeHandle.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_filesystem_PipeHandle_h
#define eckit_filesystem_PipeHandle_h

#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class PipeHandle : public DataHandle {
public:

    // -- Contructors

    PipeHandle(const std::string&);
    PipeHandle(Stream&);

    // -- Destructor

    ~PipeHandle();

    // --  Methods

    void advance(const Length&);

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void rewind() override;
    void print(std::ostream&) const override;
    /*
    virtual void restartReadFrom(const Offset& from);
    virtual void restartWriteFrom(const Offset& from);
    */
    /* virtual void toRemote(Stream&) const; */
    /* virtual void cost(std::map<std::string,Length>&, bool) const; */
    /* std::string title() const override; */
    bool moveable() const override { return false; }

    Offset seek(const Offset&) override;
    bool canSeek() const override { return false; }

    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    // -- Members

    std::string name_;
    FILE* file_;
    bool read_;

    // -- Methods

    void open(const char*);

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<PipeHandle> reanimator_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
