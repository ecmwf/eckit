/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "eckit/log/Timer.h"

#include "eckit/sql/SQLIndex.h"
//#include "eckit/sql/SQLColumn.h"
//#include "eckit/sql/SQLTable.h"

namespace eckit {
namespace sql {

struct _data {
	unsigned long offset_;
	unsigned long pool_;
};


SQLIndex::SQLIndex(SQLColumn& owner):
	owner_(owner),
	value_(0)
{} 

SQLIndex::~SQLIndex() {}

void SQLIndex::update()
{
#if 0
	PathName path  = owner_.indexPath();


	Log::info() << "Loading index " << path << std::endl;

	owner_.rewind();
    bool create = !path.exists();


	if(create)
	{
		BeDB db(path,true);

		_data d;
		Timer timer("Create index");

		unsigned long long rows = owner_.noRows();

		for(unsigned long long i = 0; i < rows; i++)
		{
			bool missing = false;
			d.offset_    = owner_.offset();
			d.pool_      = owner_.currentPool();
			double value = owner_.next(missing);

			db.add(&value,sizeof(value),&d,sizeof(d));

		}
	}

	auto_ptr<BeDB> db(new BeDB(path,false));
	db_ = db;
#endif
}

void SQLIndex::rewind(double* value)
{
#if 0
	value_ = value;
	db_->rewind();
//	std::cout << "SQLIndex::rewind " << *value << std::endl;
#endif
}

bool SQLIndex::next(unsigned long& pool,unsigned long& offset)
{
#if 0
	_data d;
	size_t size = sizeof(d);

//	std::cout << "SQLIndex::next " << value_ << std::endl;

//	for(;;)
	{
		if(db_->get(value_,sizeof(*value_),&d,size))
		{
//			if(pool_ < 0 || (pool_ == d.pool_ && d.offset_ >= offset1_ && d.offset_ <= offset2_))
			{
//				std::cout << "SQLIndex::next " << d.pool_ << " " << d.offset_ << std::endl;
				pool   = d.pool_;
				offset = d.offset_;
				return true;
			}
		}
		else
			return false;
	}

#endif
return false;
}

#if 0
void SQLIndex::restrict(unsigned long pool,unsigned long offset,unsigned long length)
{
	pool_    = pool;
	offset1_ = offset;
	offset2_ = offset + length - 1;
	std::cout << "Restrict " << pool_ << " " << offset1_ << " " << offset2_ << std::endl;
}
#endif

} // namespace sql
} // namespace eckit
