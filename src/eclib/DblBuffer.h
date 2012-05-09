/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File DblBuffer.h
// Baudouin Raoult - ECMWF Feb 97

#ifndef DblBuffer_H
#define DblBuffer_H

#include "eclib/DataHandle.h"
#include "eclib/Length.h"
#include "eclib/Mutex.h"
#include "eclib/TransferWatcher.h"

class DblBuffer {
public:

// -- Contructors

	DblBuffer(long count = 5, long size = 1024*1024, TransferWatcher& = TransferWatcher::dummy());

// -- Destructor

	~DblBuffer();

// -- Methods

	Length copy(DataHandle&,DataHandle&);

	bool   error();
	void   error(const string&);
	void   restart(RestartTransfer&);

private:

// No copy allowed

	DblBuffer(const DblBuffer&);
	DblBuffer& operator=(const DblBuffer&);

// -- Methods
	Length copy(DataHandle&,DataHandle&,const Length&);

// -- Members

	Mutex  mutex_;

	long   bufSize_;
	long   count_;

	Length inBytes_;
	Length outBytes_;

	bool   error_;
	string why_;

	bool restart_;
	Offset restartFrom_;
	TransferWatcher& watcher_;

// -- Friends

	friend class DblBufferTask;


};


#endif
