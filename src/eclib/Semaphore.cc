/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstdio>
#include <errno.h>

#include <sys/sem.h>

#include "eclib/Semaphore.h"

struct sembuf _lock[] = {
	{ 0, 0,  SEM_UNDO }, /* test */
	{ 0, 1,  SEM_UNDO }, /* lock */
};

struct sembuf _unlock[] = {
	{ 0, -1, SEM_UNDO }, /* ulck */
};

Semaphore::Semaphore(const PathName& name,int count):
    semaphore_(-1),
	count_(count),
	level_(0)
{
	key_t key = ftok(name.localPath(),1);

	if(key == key_t(-1) && errno == ENOENT)
	{
		name.touch();
		key = ftok(name.localPath(),1);
	}

	if(key == key_t(-1))
		throw FailedSystemCall(string("ftok(") + name + string(")"));

    /// @note cannot use Log::debug() of SYSCALL here, because Log is not yet initialized
	// std::cout << "Creating semaphore path=" << name << ", count=" << count << ", key=" << hex << key << dec << std::endl; 
	if( (semaphore_ = semget(key, count_, 0666 | IPC_CREAT)) < 0 )
        perror("semget failed"), throw FailedSystemCall("semget");
}

Semaphore::~Semaphore()
{
	ASSERT(level_ == 0);
}

void Semaphore::lock(void) 
{
	mutex_.lock();
	if(++level_ == 1)
		while(semop(semaphore_,_lock,NUMBER(_lock)) < 0)
		{
			if(errno != EINTR)
				throw FailedSystemCall("semop lock");
		}
}

void Semaphore::unlock(void) 
{
	ASSERT(level_ > 0);

	if(--level_ == 0)
		while(semop(semaphore_,_unlock,NUMBER(_unlock)) < 0)
		{
			if(errno != EINTR)
				throw FailedSystemCall("semop unlock");
		}

	mutex_.unlock();
}

bool Semaphore::test(int n)
{
	struct sembuf test = { n, 0 , IPC_NOWAIT};

	if(semop(semaphore_,&test,1) == 0)
		return false;  // Free

	if(errno == EAGAIN)
		return true;  // in use

	throw FailedSystemCall("semop test");

	return false;
}

pid_t  Semaphore::getpid() const
{
    int val;
    SYSCALL(val = semctl(semaphore_,0,GETPID));
    return val;
}

void Semaphore::set(int val,int n)
{
	SYSCALL(semctl(semaphore_,n,SETVAL,val));
}


int Semaphore::get(int n) const
{
	int val;
	SYSCALL(val = semctl(semaphore_,n,GETVAL,0));
	return val;
}

void Semaphore::raise(int n)
{
	struct sembuf op = { n,1,SEM_UNDO};
	SYSCALL(semop(semaphore_,&op,1));
}

void Semaphore::lower(int n)
{
	struct sembuf op = { n,-1,SEM_UNDO};
	SYSCALL(semop(semaphore_,&op,1));
}
