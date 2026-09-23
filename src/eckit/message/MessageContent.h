// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @date   Jun 2020

#ifndef eckit_message_MessageContent_H
#define eckit_message_MessageContent_H

#include <iosfwd>
#include <vector>

#include "eckit/memory/Counted.h"
#include "eckit/types/Types.h"

namespace eckit {

class DataHandle;
class Offset;
class PathName;

namespace mars {
class MarsRequest;
}

namespace message {

class MetadataGatherer;

//----------------------------------------------------------------------------------------------------------------------

class Transformer {
public:

    // virtual MessageContent* tranform(CodesContent*) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

class MessageContent : public eckit::Counted {

public:

    virtual operator bool() const;

    virtual void write(eckit::DataHandle&) const;

    virtual size_t length() const;

    virtual std::string getString(const std::string& key) const;

    virtual long getLong(const std::string& key) const;

    virtual double getDouble(const std::string& key) const;

    virtual void getDoubleArray(const std::string& key, std::vector<double>&) const;

    virtual void getFloatArray(const std::string& key, std::vector<float>&) const;

    virtual size_t getSize(const std::string& key) const;

    // Write double array at key to pre allocated array.
    virtual void getDoubleArray(const std::string& key, double* data, size_t len) const;
    virtual void getFloatArray(const std::string& key, float* data, size_t len) const;

    virtual eckit::DataHandle* readHandle() const;

    virtual eckit::Offset offset() const;

    virtual const void* data() const;

    virtual void transform(const Transformer&);
    virtual void transform(const eckit::OrderedStringDict&);

private:

    virtual void print(std::ostream&) const = 0;

    friend std::ostream& operator<<(std::ostream& s, const MessageContent& p) {
        p.print(s);
        return s;
    }
};


}  // namespace message
}  // namespace eckit


#endif
