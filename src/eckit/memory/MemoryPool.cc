/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <unistd.h>
#include <pthread.h>

#include "eckit/log/Bytes.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/memory/MemoryPool.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

static pthread_mutex_t the_lock;

static void get_lock()
{
    //cout << pthread_self() << " get_lock" << std::endl;
	pthread_mutex_lock(&the_lock);
}

static void release_lock()
{
    //cout << pthread_self() << " release_lock" << std::endl;
	pthread_mutex_unlock(&the_lock);
}

// Don't use STL , Mutex, or Log to avoid re-entrance problems...

static pthread_once_t once = PTHREAD_ONCE_INIT;

static void _init(void)
{

    //cout << pthread_self() << " init" << std::endl;

#if defined(__GNUC__) && __GNUC__ < 3
#ifndef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP { PTHREAD_MUTEX_RECURSIVE_NP }
#endif
	pthread_mutexattr_t attr = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
#endif

	pthread_mutex_init(&the_lock,&attr);

	pthread_atfork(get_lock,
				   release_lock,
				   release_lock);
}

static void init()
{
	pthread_once(&once,_init);
}

//-----------------------------------------------------------------------------

union Align {
	char      char_;
	double    double_;
	long long longlong_;
	void*     voidstar_;
	float     float_;
};

//-----------------------------------------------------------------------------

struct MemBlk {

	static MemBlk* memList_;

	static unsigned long long allocateCount_;
	static unsigned long long releaseCount_;
	static unsigned long long reuseCount_;
	static unsigned long long newCount_;


	MemBlk        *left_;
	MemBlk        *right_;
	unsigned long size_;
	unsigned long reuse_;

	// Must be last member
	unsigned long check_;

	MemBlk(size_t s): left_(0),right_(0),size_(s),reuse_(0),check_(0) {}

	void* addr() { return ((char*)&check_ + sizeof(check_)); }

	static MemBlk* find(size_t s);

	bool check()        { return check_ == size_;  }
	bool inUse()        { return check_;           }
	void inUse(bool on) { check_ = on?size_:0;     }
	void reuse()        { reuse_++;                }

	void dump(std::ostream&,int depth = 0);
	void size(unsigned long long&,unsigned long long&);
};

//-----------------------------------------------------------------------------

MemBlk* MemBlk::memList_ = 0;
unsigned long long MemBlk::allocateCount_ = 0;
unsigned long long MemBlk::releaseCount_  = 0;
unsigned long long MemBlk::reuseCount_    = 0;
unsigned long long MemBlk::newCount_      = 0;

MemBlk* MemBlk::find(size_t size)
{
	MemBlk *m = memList_;
	MemBlk *p = 0;
	bool left = false;

	while(m)
	{
		if(size == m->size_ && !m->inUse())
		{
			reuseCount_++;
			m->inUse(true);
			m->reuse();
//			std::cout << "Reusing a " << size << " block" << std::endl;
			return m;
		}
		p = m;
		if(size <= m->size_)
		{
			left = true;
			m = m->left_;
		}
		else
		{
			left = false;
			m = m->right_;
		}
	}

//	std::cout << "Allocating a " << size << " block" << std::endl;

	newCount_++;

	void *blk = new char[size + sizeof(MemBlk)];
	m         = new(blk) MemBlk(size);

	if(p == 0)
		memList_ = m;
	else if(left)
		p->left_ = m;
	else
		p->right_ = m;

	m->inUse(true);

	return m;
}


void MemBlk::dump(std::ostream& s,int depth)
{
	if(left_)  left_->dump(s,depth+1);
	s << "size= " << size_ << " active= " << (check_?1:0) <<
	" reuse= " << reuse_ << " "<< Bytes(size_) << std::endl;
	if(right_) right_->dump(s,depth+1);
}

void MemBlk::size(unsigned long long& total,unsigned long long& inuse)
{
	if(left_)  left_->size(total,inuse);
	if(right_) right_->size(total,inuse);
	total += size_;
	if(check_) inuse += size_;
}

void* MemoryPool::largeAllocate(size_t size)
{
	init();

	MemBlk *m  = 0;
	void *addr = 0;

	get_lock();
	MemBlk::allocateCount_++;
	m = MemBlk::find(size>0?size:1);
	addr = m->addr();
	release_lock();

	return addr;
}

void MemoryPool::largeDeallocate(void* addr)
{
	init();

	get_lock();
	MemBlk::releaseCount_++;

	MemBlk *m = (MemBlk*)((char*)addr - sizeof(MemBlk));

	if(!(m->check() && m->inUse())) {
        std::cerr << "deallocating a bad block" << std::endl;
		abort();
	}

//	std::cout << "Releasing " << m->size_ << std::endl;
	m->inUse(false);

	release_lock();
}

//-----------------------------------------------------------------------------

const size_t WORD = sizeof(Align);

struct memblk {
	memblk *next_;
    size_t count_;
    size_t left_;
    size_t size_;
    char   buffer_[WORD];
};

const long HEADER_SIZE = (sizeof(memblk) - WORD);

//-----------------------------------------------------------------------------

eckit::MemPool eckit::MemPool::transientPool = {
	1, // 1 page
	0, // don't zero
};

eckit::MemPool eckit::MemPool::permanentPool = {
	1, // 1 page
	0, // don't zero
};

//-----------------------------------------------------------------------------

void *MemoryPool::fastAllocate(size_t s,MemPool& pool)
{
#ifdef __hpux
	return new char[s];
#endif
	init();

	get_lock();

	memblk *m = pool.first_;

	/* align */

	s = ((s+WORD-1)/WORD)*WORD;

	while(m && (m->left_ < s))
		m = m->next_;

	if(m == 0)
	{
		static int page_size = getpagesize();

        size_t size = page_size*pool.pages_;

		if(s > size - HEADER_SIZE)
		{
//			std::cerr << "Object of " << s << " bytes is too big for " << __FUNCTION__ << std::endl;
//			std::cerr << "Block size is " << size - HEADER_SIZE << std::endl;
			size = ((s + HEADER_SIZE + (page_size-1)) / page_size) * page_size;
		}

		memblk* p = static_cast<memblk*>(largeAllocate(size));
		if(pool.clear_) ::memset(p,0,size);

		p->next_    = pool.first_;
		p->count_   = 0;
		p->size_    = p->left_ = size-HEADER_SIZE;
		m           = p;
		pool.first_ = p;
	}

	void *p   = &m->buffer_[m->size_ - m->left_];
	m->left_ -= s;
	m->count_++;

	release_lock();

	return p;
}

void MemoryPool::fastDeallocate(void *p,MemPool& pool)
{
#ifdef __hpux
	char* x = (char*)p;
	delete[] x;
	return;
#endif

	init();
	get_lock();

	memblk *m = pool.first_;
	memblk *n = 0;

	while(m)
	{
		if( ((char*)p >= (char*)&m->buffer_[0]) &&
		    ((char*)p < (char*)&m->buffer_[m->size_]))
		{
			m->count_--;
			if(m->count_ == 0)
			{
				if(n) n->next_   = m->next_;
				else pool.first_ = m->next_;
				largeDeallocate(m);
			}
			release_lock();
			return;
		}

		n = m;
		m = m->next_;
	}

    std::cerr << Here() << std::endl;
	abort();
}

//-----------------------------------------------------------------------------

inline size_t round(size_t n)
{
	const size_t rounding = WORD;
	return ((n + (rounding-1))/rounding)*rounding;
}

void MemoryPool::info(std::ostream& out)
{
    {
        init();

        get_lock();

#if 0

        static struct mallinfo l = { 0,};
        struct mallinfo m = mallinfo();
        out << "total " << m.arena
            <<  "  "  << m.arena - l.arena << std::endl;
        out << "used  " << m.uordblks << " (" << int(double(m.uordblks)/double(m.arena)*100 + 0.5) << "%)"
            <<  "  "  << m.uordblks - l.uordblks << std::endl;
        out << "free  " << m.fordblks << " (" << int(double(m.fordblks)/double(m.arena)*100 + 0.5) << "%)"
            <<  "  "  << m.fordblks - l.fordblks << std::endl;
        l = m;
        out << m.arena << " (total space in arena)" << std::endl;
        out << m.ordblks << " (number of ordinary blocks)" << std::endl;
        out << m.smblks << " (number of small blocks)" << std::endl;
        out << m.hblks << " (number of holding blocks)" << std::endl;
        out << m.hblkhd << " (space in holding block headers)" << std::endl;
        out << m.usmblks << " (space in small blocks in use)" << std::endl;
        out << m.fsmblks << " (space in free small blocks)" << std::endl;
        out << m.uordblks << " (space in ordinary blocks in use)" << std::endl;
        out << m.fordblks << " (space in free ordinary blocks)" << std::endl;
        out << m.keepcost << " (cost of enabling keep option)" << std::endl;
#ifdef SUNINFO
        out << m.mxfast << " (max size of small block)" << std::endl;
        out << m.nblks << " (number of small blocks in holding block)" << std::endl;
        out << m.grain << " (small block rounding factor)" << std::endl;
        out << m.uordbytes << " (space allocated in ordinary blocks)" << std::endl;
        out << m.allocated << " (number of ordinary blocks allocated)" << std::endl;
        out << m.treeoverhead << " (bytes used in maintaining in free tree)" << std::endl;
#endif
#endif
    }

	out << "---" << std::endl;
	out << "allocate " << MemBlk::allocateCount_ << std::endl;
	out << "release  " << MemBlk::releaseCount_  << std::endl;
	out << "reuse    " << MemBlk::reuseCount_ << std::endl;
	out << "new      " << MemBlk::newCount_ << std::endl;

	unsigned long long total = 0;
	unsigned long long left = 0;

	MemBlk::memList_->size(total,left);

	out << "total    " << total << std::endl;
	out << "left     " << left  << std::endl;

	MemBlk::memList_->dump(out,0);

	out << "Transient pool: " << std::endl;
	memblk *m = MemPool::transientPool.first_;

	while(m)
	{
		out << "Size = " << m->size_ << " left = " << m->left_
			 << " count = " << m->count_ << std::endl;
		m = m->next_;
	}

    release_lock();
}

//-----------------------------------------------------------------------------

} // namespace eckit

