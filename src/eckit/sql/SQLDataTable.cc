//#include "eckit/sql/SQLDataTable.h"

//#include <map>

#include "eckit/sql/SQLDataTable.h"
#include "odb_api/DataTable.h"

#include "eckit/sql/SQLDataColumn.h"
#include "eckit/sql/SQLDataTableIterator.h"
#include "eckit/sql/type/SQLBitfield.h"


using namespace std;
using namespace eckit;
using namespace eckit::sql;

using namespace eckit;

namespace eckit {

SQLDataTable::SQLDataTable(odb::sql::SQLDatabase& db, const DataTable& table)
  : SQLTable(db, PathName("<>"), table.name()),
    table_(table),
    data_(0)
{
    populateColumns();
}

SQLDataTable::~SQLDataTable()
{
    delete [] data_;
}

SQLColumn* SQLDataTable::column(const std::string& name)
{
    SQLColumn* column = 0;
    std::map<std::string, SQLColumn*>::iterator it = columnsByName_.begin();

    for (; it != columnsByName_.end(); ++it)
    {
        std::string s = it->first;

        if (s.find(name + "@") == 0)
        {
            if (column)
                throw eckit::UserError(std::string("SQLDataTable::column: name \"") + name + "\" is ambiguous");
            else 
                column = it->second;
        }
    }

    if (column)
        return column;

    return SQLTable::column(name);
}

bool SQLDataTable::hasColumn(const std::string& name, std::string* fullName)
{
    if (SQLTable::hasColumn(name))
    {
        if (fullName)
            *fullName = name;

        return true;
    }

    int n = 0;
    std::map<std::string,SQLColumn*>::iterator it = columnsByName_.begin();

    for (; it != columnsByName_.end(); ++it)
    {
        if (it->first.find(name + "@") == 0)
        {
            n++;

            if (fullName)
                *fullName = it->first;
        }
    }

    if (n == 0) return false;
    if (n == 1) return true;

    throw eckit::UserError(std::string("SQLDataTable:hasColumn(\"") + name + "\"): ambiguous name");
    return false;
}

SQLTableIterator* SQLDataTable::iterator(const std::vector<SQLColumn*>& columns) const
{
    return new SQLDataTableIterator(table_, const_cast<double*>(data_), columns);
}

SQLColumn* SQLDataTable::createSQLColumn(const type::SQLType& type, const std::string& name,
    int index, bool hasMissingValue, double missingValue, 
    const BitfieldDef& bitfieldDef)
{
    return new SQLDataColumn(type, *this, name, index, hasMissingValue, missingValue,
        bitfieldDef, &data_[index]);
}

SQLColumn* SQLDataTable::createSQLColumn(const type::SQLType& type, const std::string& name,
    int index, bool hasMissingValue, double missingValue)
{
    return new SQLDataColumn(type, *this, name, index, hasMissingValue, missingValue,
        &data_[index]);
}

void SQLDataTable::populateColumns()
{
    size_t count = table_.columns().size();

    data_ = new double[count]; ASSERT(data_);

    for (size_t index = 0; index < count; ++index)
    {
        const DataColumn& column = table_.columns()[index];

        const std::string name = column.name();
        bool hasMissingValue = true; // TODO: implement DataColumn::hasMissing() method
        double missingValue = column.missingValue();
        odb::BitfieldDef bitfieldDef = column.bitfieldDef();
    
        std::string sqlType;

        switch (column.type())
        {
            case INTEGER: sqlType = "integer"; break;
            case REAL:    sqlType = "real";    break;
            case DOUBLE:  sqlType = "double";  break;
            case STRING:  sqlType = "string";  break;

            case BITFIELD:
            {
                std::string signature = type::SQLBitfield::make("Bitfield",
                    bitfieldDef.first, bitfieldDef.second, "DummyTypeAlias");
                addColumn(name, index, type::SQLType::lookup(signature),
                    hasMissingValue, missingValue, true, bitfieldDef);
                continue;
            } break;

            default:
                ASSERT(!"Unknown type");
                break;
        }

        SQLColumn *c = column.type() == BITFIELD 
                    ? new SQLDataColumn(type::SQLType::lookup(sqlType), *this, name, index, hasMissingValue, missingValue, bitfieldDef, &data_[index])
                    : new SQLDataColumn(type::SQLType::lookup(sqlType), *this, name, index, hasMissingValue, missingValue, &data_[index]);

        addColumn(c, name, index);
    }
}

} // namespace eckit
