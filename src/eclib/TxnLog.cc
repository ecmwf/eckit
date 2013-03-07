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
#include "eclib/FileStream.h"
#include "eclib/Monitor.h"
#include "eclib/PathName.h"
#include "eclib/Seconds.h"
#include "eclib/StrStream.h"
#include "eclib/thread/Thread.h"
#include "eclib/thread/ThreadControler.h"
#include "eclib/TimeStamp.h"
#include "eclib/Translator.h"
#include "eclib/TxnLog.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static PathName pathName(const string& name)
{
	PathName path = string("~/txn/") + name;
	path.mkdir();
	return path;
}

#if 0 // unused
static PathName lockName(const string& name)
{
	PathName path = string("~/txn/") + name + "/lock";
	path.touch();
	return path;
}
#endif

template<class T>
TxnLog<T>::TxnLog(const string& name):
	path_(pathName(name)),
	next_(path_ +  "/next"),
	nextID_(next_,1)
{
	AutoLock<MappedArray<TxnID> > lock(nextID_);
	Log::debug() << "TxnLog file is " << path_ << endl;

	PathName done = path_ + "/done";
	done.mkdir();
}

template<class T>
TxnLog<T>::~TxnLog()
{
}

template<class T>
PathName TxnLog<T>::name(T& event)
{
	StrStream s;
	s << setfill('0') << setw(10) << event.transactionID() << StrStream::ends;
	return path_ + "/" + string(s);
}


template<class T>
void TxnLog<T>::begin(T& event)
{
	AutoLock<MappedArray<TxnID> > lock(nextID_);

	if(event.transactionID() == 0)
		event.transactionID(++nextID_[0]);

	PathName path = name(event);
	ASSERT(!path.exists());

	FileStream log(path,"w");
	log << event;
}

template<class T>
void TxnLog<T>::end(T& event,bool backup)
{
	AutoLock<MappedArray<TxnID> > lock(nextID_);

	PathName path = name(event);

	if(backup)
	{
		StrStream s;
		s << path.dirName() << "/done/" << TimeStamp(time(0),"%Y%m%d") << StrStream::ends;

		// Append to current day's backup

		string t(s);
		FileStream log(t,"a");
		log << event;
	}

	// Remove file

	path.unlink();
}

template<class T>
class RecoverThread : public Thread {
	MappedArray<TxnID>& nextID_;
	TxnRecoverer<T>&    client_;
	vector<PathName>    result_;
	long                age_;
	time_t              now_;
	virtual void run();
public:
	RecoverThread(const PathName&,MappedArray<TxnID>&,
		TxnRecoverer<T>&,long);
	void recover();
};

template<class T>
RecoverThread<T>::RecoverThread(const PathName& path,
		MappedArray<TxnID>& nextID,
		TxnRecoverer<T>& client,
		long age):
    nextID_(nextID),
	client_(client),
	age_(age),
	now_(::time(0))
{
	AutoLock<MappedArray<TxnID> > lock(nextID_);
	PathName::match(path+"/[0-9]*",result_);
	
	// Sort by ID to preserve order
	std::sort(result_.begin(),result_.end());
	Log::info() << result_.size() << " task(s) found in log files" << endl;

	if(result_.size())
	{
		PathName last = result_[result_.size()-1];
		string name = last.baseName();
		TxnID id = Translator<string,unsigned long long>()(name);
		if(id >= nextID_[0])
			nextID_[0] = id + 1;
	}
}

template<class T>
void RecoverThread<T>::run()
{
	Monitor::name("recover");
	recover();
}

template<class T>
void RecoverThread<T>::recover()
{
	static const char *here = __FUNCTION__;
	for(Ordinal i = 0 ; i < result_.size() ; i++)
	{

		if(now_ - result_[i].created() < age_)
			Log::info() << "Skipping " << result_[i] << ", created " << 
				Seconds(now_ - result_[i].created()) << " ago." << endl;
		else
			try {
				FileStream log(result_[i],"r");
				T *task = Reanimator<T>::reanimate(log);
				if(task) {
					ASSERT(task->transactionID() < nextID_[0]);
					client_.push(task);
				}
			}
			catch(exception& e)
			{
				Log::error() << "** " << e.what() << " Caught in " <<
					here <<  endl;
				Log::error() << "** Exception is ignored" << endl;
			}
	}
}

template<class T>
void TxnLog<T>::recover(TxnRecoverer<T>& client,bool inThread,long age)
{
	if(inThread)
	{
		ThreadControler c(new RecoverThread<T>(path_,nextID_,client,age));
		c.start();
	}
	else
	{
		 RecoverThread<T> r(path_,nextID_,client,age);
		 r.recover();
	}
}

template <class T>
void TxnLog<T>::find(TxnFinder<T>& r)
{
	static const char *here = __FUNCTION__;

	PathName path = path_ + "/[0-9]*";
	vector<PathName>  active;

	PathName::match(path,active);

	for(Ordinal j = 0; (j < active.size()); ++j)
	{
		try {
			 FileStream log(active[j],"r");
			 auto_ptr<T> task(Reanimator<T>::reanimate(log));
			 if(task.get()) 
				if(!r.found(*task))
				return;
		}
		catch(Abort& e)
		{
			Log::error() << "** " << e.what() << " Caught in " <<
				here <<  endl;
			Log::error() << "** Exception is re-thrown" << endl;
			throw;
		}
		catch(exception& e)
		{
			Log::error() << "** " << e.what() << " Caught in " <<
							here <<  endl;
			Log::error() << "** Exception is ignored" << endl;
		}
	}


	if(r.old())
	{

		// Look for non active transactions

		path = path_ + "/done/[0-9]*";
		vector<PathName> dates;

		PathName::match(path,dates);

		// Sort by date in reverse order
		std::sort(dates.begin(),dates.end(),greater<PathName>());


		for(Ordinal k = 0 ; k < dates.size() ; k++)
		{
			Log::info() << "Searching " << dates[k] << endl;
			try {
				FileStream log(dates[k],"r");
				T *task = 0;
                while( (task = Reanimator<T>::reanimate(log)) )
				{
					if(!r.found(*(auto_ptr<T>(task))))
						return;
				}
			}
			catch(Abort& e)
			{
				Log::error() << "** " << e.what() << " Caught in " <<
					here <<  endl;
				Log::error() << "** Exception is re-thrown" << endl;
				throw;
			}
			catch(exception& e)
			{
				Log::error() << "** " << e.what() << " Caught in " <<
					here <<  endl;
				Log::error() << "** Exception is ignored" << endl;
			}
		}
	}
}

//-----------------------------------------------------------------------------

} // namespace eckit

