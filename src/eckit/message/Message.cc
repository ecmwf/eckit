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

#include <iostream>

#include "eckit/io/Offset.h"
#include "eckit/message/Decoder.h"
#include "eckit/message/Message.h"
#include "eckit/message/MessageContent.h"
#include "eckit/types/Types.h"


namespace eckit {
namespace message {


//----------------------------------------------------------------------------------------------------------------------

class NoContent : public MessageContent {

    virtual operator bool() const { return false; }

    void print(std::ostream& s) const { s << "NoContent[]"; }

    void* operator new(size_t);

public:
    NoContent() { attach(); }
};

static NoContent noContent;

//----------------------------------------------------------------------------------------------------------------------


Message::Message() :
    content_(&noContent) {
    content_->attach();
}

Message::Message(MessageContent* content) :
    content_(content ? content : &noContent) {
    content_->attach();
}

Message::Message(const Message& other) :
    content_(other.content_) {
    content_->attach();
}

Message& Message::operator=(const Message& other) {
    if (content_ != other.content_) {
        content_->detach();
        content_ = other.content_;
        content_->attach();
    }
    return *this;
}

Message::~Message() {
    content_->detach();
}

void Message::print(std::ostream& s) const {
    s << "Message[" << *content_ << "]";
}

Message::operator bool() const {
    return content_->operator bool();
}

void Message::write(eckit::DataHandle& handle) const {
    content_->write(handle);
}

size_t Message::length() const {
    return content_->length();
}

std::string Message::getString(const std::string& key) const {
    return content_->getString(key);
}

long Message::getLong(const std::string& key) const {
    return content_->getLong(key);
}

double Message::getDouble(const std::string& key) const {
    return content_->getDouble(key);
}

void Message::getDoubleArray(const std::string& key, std::vector<double>& v) const {
    return content_->getDoubleArray(key, v);
}

Message Message::transform(const eckit::StringDict& dict) const {
    return Message(content_->transform(dict));
}

eckit::DataHandle* Message::readHandle() const {
    return content_->readHandle();
}

eckit::Offset Message::offset() const {
    return content_->offset();
}

const void* Message::data() const {
    return content_->data();
}

void Message::getMetadata(MetadataGatherer& gather) const {
    return Decoder::lookup(*this).getMetadata(*this, gather);
}

//----------------------------------------------------------------------------------------------------------------------

MetadataGatherer::~MetadataGatherer() {}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace message
}  // namespace eckit
