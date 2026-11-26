// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_TransportHandle_h
#define eckit_TransportHandle_h


#include "eckit/eckit.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"


namespace eckit::distributed {

class Transport;

//----------------------------------------------------------------------------------------------------------------------

class TransportHandle : public eckit::DataHandle {

public:  // methods


    /// Contructor

    TransportHandle(Transport& transport, const std::string& path, size_t writers, bool exclusive);

    /// Destructor

    virtual ~TransportHandle() override;

    // From DataHandle

    virtual eckit::Length openForRead() override;
    virtual void openForWrite(const eckit::Length&) override;
    virtual void openForAppend(const eckit::Length&) override;

    virtual long read(void*, long) override;
    virtual long write(const void*, long) override;
    virtual void close() override;
    virtual void flush() override;
    virtual void rewind() override;
    virtual void print(std::ostream&) const override;

    virtual eckit::Length estimate() override;
    virtual eckit::Offset position() override;

private:  // members

    Transport& transport_;
    std::string path_;
    size_t writer_;

    bool opened_;
    bool append_;
    size_t position_;

    virtual std::string title() const override;

    void send(const void* buffer, size_t length);
};


//----------------------------------------------------------------------------------------------------------------------


}  // namespace eckit::distributed

#endif
