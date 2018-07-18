
#include "eckit/sql/SQLDataTableIterator.h"
//#include "eckit/sql/SQLDataColumn.h"

namespace eckit {

SQLDataTableIterator::SQLDataTableIterator(const DataTable& table, double* data,
        const std::vector<odb::sql::SQLColumn*>& columns)
  : table_(table),
    it_(table.begin()),
    end_(table.end()), 
    columns_(columns),
    data_(data),
    firstRow_(true)
{
    if (it_ != end_)
    {
        copyRow();
    }
}

SQLDataTableIterator::~SQLDataTableIterator()
{}

void SQLDataTableIterator::rewind()
{}

bool SQLDataTableIterator::next()
{
    if (firstRow_) firstRow_ = false;
    else ++it_;

    if (it_ == end_)
        return false;

    copyRow();

    return true;
}

void SQLDataTableIterator::copyRow()
{
    std::copy(it_->data(), it_->data() + table_.columns().size(), data_);
}

} // namespace eckit
