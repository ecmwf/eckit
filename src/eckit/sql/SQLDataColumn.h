#ifndef SQLDATACOLUMN_H_
#define SQLDATACOLUMN_H_

//#include <string>

#include "eckit/sql/SQLColumn.h"

namespace eckit {

class SQLDataColumn : public odb::sql::SQLColumn
{
public:
	SQLDataColumn(const odb::sql::type::SQLType& type, odb::sql::SQLTable& table,
            const std::string& name, int index, bool hasMissingValue,
            double missingValue, const odb::BitfieldDef& bitfieldDef,
            double* value);

	SQLDataColumn(const odb::sql::type::SQLType& type, odb::sql::SQLTable& table,
            const std::string& name, int index, bool hasMissingValue,
            double missingValue, double* value);

    ~SQLDataColumn();

    void value(double* p) { value_ = p; }
    double * value() const { return value_; }

private:
    SQLDataColumn(const SQLDataColumn&);
    SQLDataColumn& operator=(const SQLDataColumn&);

    virtual void rewind();
    virtual double next(bool& missing);
    virtual void advance(unsigned long n);

    double* value_;
    double missing_;
};

} // namespace eckit

#endif // SQLDATACOLUMN_H_
