/// @file   SQLDataSet.h
/// @author Tomas Kral

#ifndef SQLDATASET_H_
#define SQLDATASET_H_

#include "eckit/sql/SQLDatabase.h"

namespace eckit {
namespace sql {

class SQLDataSet : public SQLDatabase
{
public:
    SQLDataSet();
    SQLDataSet(const std::string& name);
    ~SQLDataSet();

private:
    SQLDataSet(const SQLDataSet&);
    SQLDataSet& operator=(const SQLDataSet&);

    virtual void open();
    virtual void close();

    virtual odb::sql::SQLTable* openDataHandle(eckit::DataHandle&, DataFormat = ODA);
    virtual odb::sql::SQLTable* openDataStream(std::istream&, const std::string& delimiter, DataFormat = CSV);
};

} // namespace sql
} // namespace eckit

#endif // SQLDATASET_H_
