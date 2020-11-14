/*
 * (C) Copyright 2017- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/message/Reader.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/message/Message.h"
#include "eckit/message/Splitter.h"

namespace eckit {
namespace message {

Reader::Reader(eckit::DataHandle* h, bool opened) : handle_(h), opened_(opened) {

    init();
}

Reader::Reader(eckit::DataHandle& h, bool opened) : handle_(h), opened_(opened) {

    init();
}

Reader::Reader(const eckit::PathName& path) : handle_(path.fileHandle()), opened_(false) {

    init();
}

void Reader::init() {
    if (!opened_) {
        handle_.openForRead();
    }

    splitter_.reset(SplitterFactory::lookup(handle_));
    // std::cout << "----- " << *splitter_ << std::endl;
}

Reader::~Reader() {

    if (!opened_) {
        handle_.close();
    }
}

Message Reader::next() {
    return splitter_->next();
}

void Reader::print(std::ostream& s) const {
    s << "Reader[" << handle_ << "," << *splitter_ << "]";
}

eckit::Offset Reader::position() {
    return handle_.position();
}


}  // namespace message
}  // namespace eckit
