/*
 * (C) Copyright 2017- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date   Jun 2020

#ifndef metkit_data_Message_H
#define metkit_data_Message_H

#include <iosfwd>
#include <vector>

#include "eckit/io/Buffer.h"
#include "eckit/message/Decoder.h"
#include "eckit/types/Types.h"

namespace eckit {
class DataHandle;
class Offset;
class PathName;
};  // namespace eckit

namespace eckit {

namespace mars {
class MarsRequest;
}

namespace message {

class MessageContent;
class CodesContent;

//----------------------------------------------------------------------------------------------------------------------

class MetadataGatherer {
public:

    virtual ~MetadataGatherer();
    virtual void setValue(const std::string& key, const std::string& value) = 0;
    virtual void setValue(const std::string& key, long value)               = 0;
    virtual void setValue(const std::string& key, double value)             = 0;
};

//----------------------------------------------------------------------------------------------------------------------

/// Message represents an immutable data object with metadata attached
/// Modifications to messages create new messages via transformation actions
class Message {
public:

    Message();

    explicit Message(MessageContent*);

    Message(const Message&);

    ~Message();

    Message& operator=(const Message&);

    explicit operator bool() const;

    void write(eckit::DataHandle&) const;

    size_t length() const;
    eckit::Offset offset() const;
    const void* data() const;

    std::string getString(const std::string& key) const;
    long getLong(const std::string& key) const;
    double getDouble(const std::string& key) const;
    void getDoubleArray(const std::string& key, std::vector<double>&) const;
    size_t getSize(const std::string& key) const;

    // Write double array at key to pre allocated array
    void getDoubleArray(const std::string& key, double* data, size_t len) const;

    void getMetadata(MetadataGatherer&, GetMetadataOptions options = GetMetadataOptions{}) const;

    eckit::Buffer decode() const;

    eckit::DataHandle* readHandle() const;

    mars::MarsRequest request() const;

    Message transform(const eckit::StringDict& modifiers) const;

private:

    MessageContent* content_;
    mutable MessageDecoder* decoder_ = nullptr;  // non-owning

    MessageDecoder& lookupDecoder() const;

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream& s, const Message& p) {
        p.print(s);
        return s;
    }
};

//----------------------------------------------------------------------------------------------------------------------

template <class T>
class StringSetter : public MetadataGatherer {
    T& object_;

    void setValue(const std::string& key, const std::string& value) override { object_.setValue(key, value); }

    void setValue(const std::string& /*key*/, long /*value*/) override {}

    void setValue(const std::string& /*key*/, double /*value*/) override {}

public:

    StringSetter(T& object) : object_(object) {}
};

template <class T>
class TypedSetter : public MetadataGatherer {
    T& object_;

    void setValue(const std::string& key, const std::string& value) override { object_.setValue(key, value); }

    void setValue(const std::string& key, long value) override { object_.setValue(key, value); }

    void setValue(const std::string& key, double value) override { object_.setValue(key, value); }

public:

    TypedSetter(T& object) : object_(object) {}
};
//----------------------------------------------------------------------------------------------------------------------


}  // namespace message
}  // namespace eckit

#endif
