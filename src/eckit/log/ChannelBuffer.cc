/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <ostream>

#include "eckit/log/ChannelBuffer.h"
#include "eckit/log/LogTarget.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/log/IndentTarget.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ChannelBuffer::ChannelBuffer( std::size_t size ) :
    std::streambuf(),
    buffer_( size  )
{
    ASSERT( size );
    char *base = &buffer_.front();
    setp(base, base + buffer_.size() );
}

ChannelBuffer::~ChannelBuffer()
{
    clear();
}

void ChannelBuffer::clear() {
    sync();
    for(std::vector<LogTarget*>::iterator i = targets_.begin(); i != targets_.end(); ++i) {
         (*i)->detach();
    }
    targets_.clear();
}

void ChannelBuffer::setLogTarget(LogTarget* target) {
    clear();
    addLogTarget(target);
}

void ChannelBuffer::addLogTarget(LogTarget* target) {
    ASSERT(target);
    target->attach();
    targets_.push_back(target);
}

bool ChannelBuffer::dumpBuffer()
{
    for(std::vector<LogTarget*>::iterator i = targets_.begin(); i != targets_.end(); ++i) {
        (*i)->write(pbase(), pptr());
    }
    setp(pbase(), epptr());
    return true;
}

void ChannelBuffer::indent(const char* space) {
    for(size_t i = 0; i < targets_.size(); i++) {
        LogTarget* indent = new IndentTarget(space, targets_[i]);
        targets_[i]->detach();
        targets_[i] = indent;
        targets_[i]->attach();
    }
}

void ChannelBuffer::unindent() {
    for(size_t i = 0; i < targets_.size(); i++) {
        IndentTarget*  indent = dynamic_cast<IndentTarget*>(targets_[i]);
        if(indent == 0) {
            throw SeriousBug("Attempt to unindent a Channel that is not indented");
        }

        LogTarget* target = indent->target_;
        target->attach();
        targets_[i]->detach();
        targets_[i] = target;
    }

}

std::streambuf::int_type ChannelBuffer::overflow(std::streambuf::int_type ch)
{
    if (ch == traits_type::eof() ) { return sync(); }
    dumpBuffer();
    sputc(ch);
    return traits_type::to_int_type(ch);
}

std::streambuf::int_type ChannelBuffer::sync()
{
    if( dumpBuffer() )
    {
        for(std::vector<LogTarget*>::iterator i = targets_.begin(); i != targets_.end(); ++i) {
            (*i)->flush();
        }
        return 0;
    }
    else return -1;
}

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit
