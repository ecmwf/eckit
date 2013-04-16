/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/FileHandle.h"
#include "eclib/Log.h"
#include "eclib/filesystem/MarsFSHandle.h"
#include "eclib/StrStream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ClassSpec MarsFSHandle::classSpec_ = {&DataHandle::classSpec(),"MarsFSHandle",};
Reanimator<MarsFSHandle> MarsFSHandle::reanimator_;

void MarsFSHandle::print(ostream& s) const
{
	s << "MarsFSHandle[file=" << path_ << ']';
}

void MarsFSHandle::encode(Stream& s) const
{
	DataHandle::encode(s);
	s << path_;
	s << overwrite_;
}

MarsFSHandle::MarsFSHandle(Stream& s):
	DataHandle(s),
    path_(s),
    position_(0)
{
	s >> overwrite_;
}

MarsFSHandle::MarsFSHandle(const MarsFSPath& path, bool overwrite):
	path_(path),
	read_(false),
    position_(0),
    overwrite_(overwrite)    
{
	//Log::info() << "CREATE " << *this << endl;
}

MarsFSHandle::~MarsFSHandle()
{
	//Log::info() << "DESTROY " << *this << endl;
}


Length MarsFSHandle::openForRead()
{
	read_   = true;
    position_ = 0;
    if(!file_.get())
    	file_ = auto_ptr<MarsFSFile>(new MarsFSFile(path_));
    length_ = file_->open("r");
    return length_;
}

void MarsFSHandle::openForWrite(const Length& length)
{
	read_   = false;
    length_ = length;
    position_ = 0;
    if(!file_.get())
    	file_ = auto_ptr<MarsFSFile>(new MarsFSFile(path_));
    file_->open("w", overwrite_);
}

void MarsFSHandle::openForAppend(const Length& length)
{
    NOTIMP;
}

long MarsFSHandle::read(void* buffer,long length)
{
	ASSERT(file_.get());
    long len = file_->read(buffer, length);
    if(len > 0) position_ += len;
    return len;
}

long MarsFSHandle::write(const void* buffer,long length)
{
	ASSERT(file_.get());
    long len = file_->write(buffer, length);
    if(len > 0) position_ += len;
    return len;
}

Offset MarsFSHandle::position() 
{
    return position_;
}

void MarsFSHandle::close()
{
	if(file_.get()) {
		file_->close();
		file_ = auto_ptr<MarsFSFile>(0);
	}
}

void MarsFSHandle::rewind()
{
	ASSERT(file_.get());
	file_->seek(0);
}

void MarsFSHandle::skip(size_t n)
{
	ASSERT(file_.get());
	file_->skip(n);
    position_ += n;
}

Length MarsFSHandle::estimate()
{
	return MarsFSClient(path_).size(path_.path());
}

bool MarsFSHandle::isEmpty() const
{
	if(!MarsFSClient(path_).exists(path_.path()))
		return false;
    return  MarsFSClient(path_).size(path_.path()) == Length(0);
}

void MarsFSHandle::restartReadFrom(const Offset& from)
{
    ASSERT(read_);
    ASSERT(file_.get());
    Log::warning() << *this << " restart read from " << from << endl;
    ASSERT(file_->seek(from) == from);
}

void MarsFSHandle::restartWriteFrom(const Offset& from)
{
    ASSERT(!read_);
    ASSERT(file_.get());
    Log::warning() << *this << " restart write from " << from << endl;
    ASSERT(file_->seek(from) == from);
}

void MarsFSHandle::toLocal(Stream& s) const
{
    // TODO: Return FileHandle if local
    if(path_.isLocal()) {
        FileHandle fh(path_.path());
        s << fh;
        return;
    }
    s << *this;
}

DataHandle* MarsFSHandle::toLocal()
{
    // TODO: Return FileHandle if local
    if(path_.isLocal()) {
        return new FileHandle(path_.path());
    }
    return this;
}

void MarsFSHandle::cost(map<string,Length>& c, bool read) const
{
    if(read) {
        c[path_.node()] += const_cast<MarsFSHandle*>(this)->estimate();
    }
    else
    {
        // Just mark the node as being a candidate
        c[path_.node()] += 0;
    }
}

string MarsFSHandle::title() const
{
	StrStream os;
	os << "marsfs:/" << path_.node() << "/" << PathName::shorten(path_.path()) << StrStream::ends;
	return string(os);
}

//-----------------------------------------------------------------------------

} // namespace eckit

