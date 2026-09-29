// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   Aug 2017


#ifndef eckit_message_Reader_H
#define eckit_message_Reader_H

#include <iosfwd>
#include <memory>

#include "eckit/io/PeekHandle.h"
#include "eckit/message/Message.h"


namespace eckit {
class DataHandle;
class PathName;
class Offset;
};  // namespace eckit

namespace eckit::message {

class Message;
class Splitter;

class Reader {
public:

    Reader(eckit::DataHandle*, bool opened = false);
    Reader(eckit::DataHandle&, bool opened = false);

    Reader(const eckit::PathName&);

    Reader(const Reader&)            = delete;
    Reader& operator=(const Reader&) = delete;
    Reader(Reader&&)                 = delete;
    Reader& operator=(Reader&&)      = delete;

    ~Reader();

    Message next();
    eckit::Offset position();

private:

    std::unique_ptr<Splitter> splitter_;
    eckit::PeekHandle handle_;

    void init();
    void print(std::ostream&) const;  // Change to virtual if base class

    friend std::ostream& operator<<(std::ostream& s, const Reader& p) {
        p.print(s);
        return s;
    }
};

}  // namespace eckit::message

#endif
