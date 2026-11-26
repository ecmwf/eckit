// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Dec 2013

#ifndef eckit_filesystem_RawFileHandle_h
#define eckit_filesystem_RawFileHandle_h

#include "eckit/io/DataHandle.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class RawFileHandle : public DataHandle {
public:

    RawFileHandle(const std::string& path, bool overwrite = false);

    ~RawFileHandle();

    Length openForRead() override;
    void openForWrite(const Length&) override;
    void openForAppend(const Length&) override;

    long read(void*, long) override;
    long write(const void*, long) override;
    void close() override;
    void print(std::ostream&) const override;

    Length size() override;
    Length estimate() override;
    Offset position() override;
    Offset seek(const Offset&) override;
    bool canSeek() const override { return true; }
    void skip(const Length&) override;

    void encode(Stream&) const override;

private:

    std::string path_;
    bool overwrite_;
    int fd_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
