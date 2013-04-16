/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File filesystem/MoverHandle.cc
// Baudouin Raoult - (c) ECMWF Jun 11


#include "eclib/AutoLock.h"
#include "eclib/Monitor.h"
#include "eckit/filesystem/MoverHandle.h"
#include "eclib/MoverTransfer.h"
#include "eclib/NodeInfo.h"
#include "eckit/filesystem/TCPHandle.h"
#include "eclib/TCPServer.h"
#include "eclib/thread/Thread.h"
#include "eclib/thread/ThreadControler.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class MoverHandleThread : public Thread {

    MoverHandle& owner_;
	TCPServer server_;
	TCPHandle local_;
    bool      read_;
    bool      fail_;
	Mutex     mutex_;

	public:

	MoverHandleThread(MoverHandle& owner, bool read):
        owner_(owner),
		local_(NodeInfo::thisNode().host(), server_.localPort()),
		read_(read),
		fail_(false)
	{
	}

	void run() 
	{
		static const char* here = __FUNCTION__;

		{
			AutoLock<Mutex> lock(mutex_);
			if(fail_)
				return;
		}

		try
		{
			DataHandle& remote = owner_.handle();
			Monitor::name("mover");
			Log::status() << "Using mover" << endl;
			MoverTransfer mover;
			if(read_) {
				mover.transfer(remote, local_);
			}
			else
			{
				mover.transfer(local_, remote);
			}
		}
		catch(TimeOut& e)
		{
			Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
			Log::error() << "** Exception is handled" << endl;
			owner_.fail(Retry(e.what()).what());
		}
		catch(exception& e)
		{
			Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
			Log::error() << "** Exception is handled" << endl;
			owner_.fail(e.what());
		}

	}

	TCPSocket& accept() {

		static const char* here = __FUNCTION__;

		try {
			return server_.accept("MoverHandle waiting for connection", 60);
		}
		catch(TimeOut& e)
		{
			AutoLock<Mutex> lock(mutex_);
			fail_ = true;
			Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
			Log::error() << "** Exception is handled" << endl;
			throw Retry(e.what());
		}
		catch(...)
		{
			AutoLock<Mutex> lock(mutex_);
			fail_ = true;
			throw;
		}
	}
};

//-----------------------------------------------------------------------------

MoverHandle::MoverHandle(DataHandle* handle):
	handle_(handle),
	fail_(false)
{
}

MoverHandle::~MoverHandle()
{
}

void MoverHandle::fail(const string& error)
{
	AutoLock<Mutex> lock(mutex_);
	error_ = error;
	fail_  = true;
}

Length MoverHandle::openForRead() 
{

	MoverHandleThread* t = new MoverHandleThread(*this, true);
	ThreadControler tc(t);
	tc.start();
	data_ = t->accept();
	Log::info() << "Connected" << endl;

	return handle_->estimate();
}

void MoverHandle::openForWrite(const Length&)
{
	static const char* here = __FUNCTION__;

	MoverHandleThread* t = new MoverHandleThread(*this, false);
	ThreadControler tc(t);
	tc.start();

	try {
		data_ = t->accept();
	}
	catch(TimeOut& e)
	{
		// TODO: Check if the thread disapear
		Log::error() << "** " << e.what() << " Caught in " << here <<  endl;
		Log::error() << "** Exception is handled" << endl;
		throw Retry(e.what());
	}

	Log::info() << "Connected" << endl;
}


void MoverHandle::openForAppend(const Length&)
{
	NOTIMP;
}

void MoverHandle::close()
{
	AutoLock<Mutex> lock(mutex_);
	data_.close();
	if(fail_) throw Exception(string("MoverHandle::close: ") + error_);
}

long MoverHandle::read(void* buf, long len)
{
	AutoLock<Mutex> lock(mutex_);
	if(fail_) throw Exception(string("MoverHandle::read: ") + error_);
	return data_.read(buf, len);
}

long MoverHandle::write(const void* buf, long len)
{
	AutoLock<Mutex> lock(mutex_);
	if(fail_) throw Exception(string("MoverHandle::write: ") + error_);
	return data_.write(buf, len);
}

void MoverHandle::print(ostream& s) const
{
	s << "MoverHandle[";
	handle_->print(s);
	s << "]";
}


string MoverHandle::title() const
{
	return string("->") + handle_->title();
}

//-----------------------------------------------------------------------------

} // namespace eckit

