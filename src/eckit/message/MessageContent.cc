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

#include "eckit/memory/Counted.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/message/Decoder.h"
#include "eckit/message/MessageContent.h"

namespace eckit::message {

MessageContent::operator bool() const {
    return true;
}

void MessageContent::write(eckit::DataHandle&) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " write()";
    throw eckit::SeriousBug(oss.str());
}

size_t MessageContent::length() const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " length(get)";
    throw eckit::SeriousBug(oss.str());
}

std::string MessageContent::getString(const std::string& key) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " getString()";
    throw eckit::SeriousBug(oss.str());
}

long MessageContent::getLong(const std::string& key) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " getLong()";
    throw eckit::SeriousBug(oss.str());
}

double MessageContent::getDouble(const std::string& key) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " getDouble()";
    throw eckit::SeriousBug(oss.str());
}

void MessageContent::getDoubleArray(const std::string& key, std::vector<double>&) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " getDoubleArray(key, vector<double>&)";
    throw eckit::SeriousBug(oss.str());
}

void MessageContent::getFloatArray(const std::string& key, std::vector<float>&) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " getFloatArray(key, vector<float>&)";
    throw eckit::SeriousBug(oss.str());
}

void MessageContent::getDoubleArray(const std::string& key, double* data, size_t len) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " getDoubleArray(key, double*, len)";
    throw eckit::SeriousBug(oss.str());
}

void MessageContent::getFloatArray(const std::string& key, float* data, size_t len) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " getFloatArray(key, float*, len)";
    throw eckit::SeriousBug(oss.str());
}

size_t MessageContent::getSize(const std::string& key) const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " getSize(key)";
    throw eckit::SeriousBug(oss.str());
}

eckit::DataHandle* MessageContent::readHandle() const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " readHandle()";
    throw eckit::SeriousBug(oss.str());
}

eckit::Offset MessageContent::offset() const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " offset()";
    throw eckit::SeriousBug(oss.str());
}

const void* MessageContent::data() const {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " data()";
    throw eckit::SeriousBug(oss.str());
}

void MessageContent::transform(const Transformer&) {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " tranform(Transformer)";
    throw eckit::SeriousBug(oss.str());
}

void MessageContent::transform(const StringDict&) {
    std::ostringstream oss;
    oss << "Not implemented " << *this << " tranform(StringDict)";
    throw eckit::SeriousBug(oss.str());
}

}  // namespace eckit::message
