/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/AutoLock.h"
#include "eclib/Dispatcher.h"
#include "eclib/JSON.h"
#include "eclib/Log.h"
#include "eclib/Monitor.h"
#include "eclib/Monitorable.h"
#include "eclib/Plural.h"
#include "eclib/thread/Thread.h"
#include "eclib/thread/ThreadControler.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template<class Traits> 
class DispatchTask : public Thread, public Monitorable {
public:

	typedef typename Traits::Handler Handler;
	typedef typename Traits::Request Request;

	DispatchTask(Dispatcher<Traits>&  owner,int id):
		Thread(false), // Don't delete object when stopped
		owner_(owner), id_(id) {}

private:

	Dispatcher<Traits>& owner_;
	int                 id_;
	vector<Request*>    pick_;
	Mutex               mutex_;

// -- Overridden methods

	// From Thread

	virtual void run();

	// From Monitorable

	virtual void status(ostream&) const;
	virtual void json(JSON&) const;


// No copy

	typedef class DispatchTask<Traits> DT;
	DispatchTask(const DT&);
	void operator=(const DT&);
};

template<class Traits> 
class DispatchInfo : public Thread {
	Dispatcher<Traits>& owner_;
	virtual void run();
public:
	DispatchInfo(Dispatcher<Traits>& owner):
		owner_(owner) {}
};

template<class Traits> 
void DispatchTask<Traits>::status(ostream& s) const
{
	AutoLock<Mutex> lock(((DispatchTask<Traits>*)this)->mutex_);
	s << "Picks for " <<  owner_.name() << " thread " << id_ << endl;
	for(int i = 0; i < pick_.size(); i++)
		if(pick_[i]) Handler::print(s,*pick_[i]);
}

template<class Traits> 
void DispatchTask<Traits>::json(JSON& s) const
{
	AutoLock<Mutex> lock(((DispatchTask<Traits>*)this)->mutex_);
    int n = 0;
	for(int i = 0; i < pick_.size(); i++)
		if(pick_[i]) n++;

    if(n) {
    s.startObject();
	s <<  "name" << owner_.name();
    s <<  "id"   << id_ ;
    s << "picks";
    s.startList();
	for(int i = 0; i < pick_.size(); i++)
		if(pick_[i]) Handler::json(s,*pick_[i]);
    s.endList();
    s.endObject();
    }
}

template<class Traits> 
void DispatchTask<Traits>::run()
{

	static const char *here = __FUNCTION__;

	Log::info() << "Start of " << owner_.name() << " thread " << id_ << endl;

	Monitor::name(owner_.name());

	Handler handler;



	while(!stopped())
	{

		bool stop = owner_.next(handler,pick_,mutex_);


		if(stop) // The T must stop
		{
			Log::info() << "Stopping thread " << id_ << endl;
			break;
		}

		owner_.running(1);
		try {
			if(pick_.size()) 
			{
				handler.handle(pick_);
				owner_.awake(); // Request is finished
			}
			else {
				handler.idle();
				owner_.sleep(); // Wait for something else to come...
			}
		}
		catch(exception& e)
		{
			Log::error() << "** " << e.what() << " Caught in " <<
				here <<  endl;
			Log::error() << "** Exception is ignored" << endl;
			owner_.awake();
		}
		owner_.running(-1);

		AutoLock<Mutex> lock(mutex_);

		for(typename vector<Request*>::iterator i = pick_.begin(); i != pick_.end(); ++i)
		{
			delete (*i);
			*i = 0;
		}
	}

	owner_.awake();
	Log::info() << "End of thread " << id_ << endl;


}

//=====================================================================
template<class Traits> 
void DispatchInfo<Traits>::run()
{
	Monitor::name(owner_.name());
	for(;;)
	{
		int n = owner_.size();
		Monitor::show(n>0);
		Log::status() << Plural(n,"request") << " queued" << endl;
		::sleep(1);
		/* owner_.sleep(3); */
		/* owner_.awake(); // Awake others */
	}
}
//=====================================================================

template<class Traits>
Dispatcher<Traits>::Dispatcher(const string& name):
	name_(name),
	maxTasks_(this,"numberOfThreads",1),
	count_(0),
	next_(0),
	running_(0),
	grow_(maxTasks_ == 0)
{
	// For some reason xlC require that
	typedef class DispatchInfo<Traits> DI;

	ThreadControler c(new DI(*this));
	c.start();

	changeThreadCount(maxTasks_);
}


template<class Traits>
Dispatcher<Traits>::~Dispatcher()
{
}

template<class Traits>
long Dispatcher<Traits>::running()
{
	if(grow_) {
		AutoLock<Mutex> lock(lock_);
		return running_;
	}
	return 0;
}

template<class Traits>
void Dispatcher<Traits>::running(long delta)
{
	if(grow_) {
		AutoLock<Mutex> lock(lock_);
		running_ += delta;
		ASSERT(running_ >= 0);
	}
}

template<class Traits>
void Dispatcher<Traits>::push(Request* r)
{
	if(!r) return;

	if(grow_)
	{
		int cnt = running();
		if(cnt == count_) changeThreadCount(1);
	}

	_push(r);
}

template<class Traits>
void Dispatcher<Traits>::_push(Request* r)
{

	{
		AutoLock<MutexCond> lock(ready_);
		queue_.push_back(r); // enqueue Request
		ready_.signal();
	}
	awake();
}

template<class Traits>
void Dispatcher<Traits>::sleep()
{
	//Log::debug() << "Sleeping..." << endl;
	AutoLock<MutexCond> lock(wait_);
	wait_.wait();
	//Log::debug() << "Awaken..." << endl;
}

template<class Traits>
void Dispatcher<Traits>::sleep(int n)
{
	AutoLock<MutexCond> lock(wait_);
	wait_.wait(n);
}

template<class Traits>
void Dispatcher<Traits>::awake()
{
	Log::debug() << "Awake ..." << endl;
	AutoLock<MutexCond> lock(wait_);
	wait_.broadcast();
}

template<class Traits>
int Dispatcher<Traits>::size()
{
	AutoLock<MutexCond> lock(ready_);
	return queue_.size();
}

template<class Traits>
void Dispatcher<Traits>::print(ostream& s) const
{
	AutoLock<MutexCond> lock(const_cast<Dispatcher<Traits>*>(this)->ready_);
    for(typename list<Request*>::const_iterator i = queue_.begin() ; 
		i != queue_.end(); ++i) 
			if(*i)
			Handler::print(s,*(*i));
}

template<class Traits>
void Dispatcher<Traits>::json(JSON& s) const
{
	AutoLock<MutexCond> lock(const_cast<Dispatcher<Traits>*>(this)->ready_);
    s.startList();
    for(typename list<Request*>::const_iterator i = queue_.begin() ; 
		i != queue_.end(); ++i) 
			if(*i)
			Handler::json(s,*(*i));
    s.endList();
}

template<class Traits>
bool Dispatcher<Traits>::next(Handler& handler,
	vector<Request*>& result,Mutex& mutex)
{

	Log::status() << "-" << endl;

	AutoLock<MutexCond> lock(ready_);

	bool stop = false;

	while(queue_.empty())
		ready_.wait();

	AutoLock<Mutex> lock2(mutex); // Lock vector

	result.clear();

	// Check for null requests

	typename list<Request*>::iterator i = std::find(queue_.begin(),queue_.end(),
		(Request*)0);

	if(i == queue_.end())
		handler.pick(queue_,result);
	else {
		// thread will be stopped
		queue_.erase(i);
		stop = true;
	}

	Log::debug() << "Got " << result.size() << " requests from the queue" 
		<< endl;

	Log::debug() << "Left " << queue_.size() << " requests in the queue" 
		<< endl;

	ready_.signal(); // Let other get some more

	if(result.size())
		Log::status() << "Processing request" << endl;

	return stop;
}

template<class Traits>
void Dispatcher<Traits>::dequeue(DequeuePicker<Request>& p)
{
	AutoLock<MutexCond> lock(ready_);
	p.pick(queue_);
	ready_.signal();
	
}

template<class Traits>
void Dispatcher<Traits>::changeThreadCount(int delta)
{
	// We do not take a lock, to aboid dead-locks
	typedef class DispatchTask<Traits> DT;

	if(delta > 0) 
	{
		for(int i = 0; i < delta ; i++) 
		{
			ThreadControler c(new DT(*this,next_++));
			c.start();
		}
	}

	if(delta < 0) 
	{
		for(int i = 0; i < -delta ; i++)
		{
			_push(0);
		}
	}

	if(delta) {
		AutoLock<Mutex> lock(lock_);
		count_   += delta;
	}
}

template<class Traits>
void Dispatcher<Traits>::reconfigure()
{
	Log::info() << "Max is now : " << maxTasks_ << endl;
	changeThreadCount(maxTasks_ - count_);
	awake();
}

template<class Request> 
void DefaultHandler<Request>::pick(list<Request*>& queue,
	vector<Request*>& result)
{
	Request *r = queue.front();
	queue.pop_front();
	result.push_back(r);
}

template<class Request> 
void DefaultHandler<Request>::print(ostream& s,const Request& r)
{
	s << r << endl;
}

template<class Request> 
void DefaultHandler<Request>::json(JSON& s,const Request& r)
{
	r.json(s);
}

//-----------------------------------------------------------------------------

} // namespace eckit

