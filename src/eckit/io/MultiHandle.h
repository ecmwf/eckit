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
/// @author Emanuele Danovaro
/// @author Simon Smart
/// @author Tiago Quintino
/// @date May 96

#ifndef eckit_filesystem_MultiHandle_h
#define eckit_filesystem_MultiHandle_h

#include "eckit/io/DataHandle.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MultiHandle : public DataHandle {
public:

    typedef std::vector<DataHandle*> HandleList;

    // -- Contructors

    MultiHandle();
    MultiHandle(const LengthList&);
    MultiHandle(const HandleList&);
    MultiHandle(const HandleList&, const LengthList&);
    MultiHandle(Stream&);

    // -- Destructor

    ~MultiHandle() override;

    // -- Operators

    virtual void operator+=(DataHandle*);
    virtual void operator+=(const Length&);

    // -- Overridden methods

    // From DataHandle

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void flush() override;
    void rewind() override;
    void print(std::ostream&) const override;
    void restartReadFrom(const Offset& from) override;

    Offset position() override;
    Offset seek(const Offset&) override;
    bool canSeek() const override;

    bool merge(DataHandle*) override;
    bool compress(bool = false) override;

    Length size() override;
    Length estimate() override;
    void toRemote(Stream&) const override;
    void toLocal(Stream&) const override;
    DataHandle* toLocal() override;
    void selectMover(MoverTransferSelection&, bool read) const override;

    std::string title() const override;
    bool moveable() const override;
    DataHandle* clone() const override;
    void collectMetrics(const std::string& what) const override;
    // From Streamable

    void encode(Stream&) const override;
    const ReanimatorBase& reanimator() const override { return reanimator_; }

    // -- Class methods

    static const ClassSpec& classSpec() { return classSpec_; }

private:

    // -- Members

    HandleList datahandles_;
    HandleList::iterator current_;
    LengthList::iterator curlen_;
    LengthList length_;
    Length written_;
    mutable std::set<std::string> requiredAttributes_;
    bool read_;

    // -- Methods

    void openCurrent();
    void open();
    long read1(char*, long);

    // -- Class members

    static ClassSpec classSpec_;
    static Reanimator<MultiHandle> reanimator_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
