/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

//#include "eckit/log/Log.h"
#include "eckit/utils/Translator.h"

#include "odb_api/sql/SQLAST.h"
#include "odb_api/sql/SQLBitfield.h"
#include "odb_api/sql/SQLCreateTable.h"
#include "odb_api/sql/SQLDatabase.h"
#include "odb_api/sql/SQLSession.h"

#include "odb_api/Types.h"
#include "eckit/exception/Exceptions.h"
#include "odb_api/MDI.h"


using namespace eckit;

namespace eckit {
namespace sql {

SQLCreateTable::SQLCreateTable(std::string tableName, ColumnDefs &cols)
: tableName_(tableName), cols_(cols)
{}

SQLCreateTable::~SQLCreateTable() {}

void SQLCreateTable::print(std::ostream& s) const
{
}

unsigned long long SQLCreateTable::execute()
{
    SQLDatabase &db = SQLSession::current().currentDatabase();
    // SQLTable::SQLTable(SQLDatabase& owner,const PathName& path,const std::string& name,int no_rows,int index)
    // TODO:
    SQLTable *table = NULL; //new SQLTable(db, tableName_ /*?*/, tableName_, /*no_rows (columns, really..)*/ 0, /*index*/ 0);

    int index = 1;
    for (ColumnDefs::const_iterator i = cols_.begin(); i != cols_.end(); i++)
    {
        const std::string columnName (i->name()),
                     typeName (i->type());

		const long start (i->range().first),
                   end (i->range().second);
		bool isVector = ! (start == 0 && end == 0); // e.g: colname[1:3] pk1real
		BitfieldDef bitfieldDef;
		if (typeName == "@LINK")
		{
			ASSERT(! isVector);

			db.links()[tableName_].insert(columnName);

			const type::SQLType& intType = type::SQLType::lookup("pk5int");
			table->addColumn(columnName + ".offset", index++, intType, false, odb::MDI::integerMDI(), false, bitfieldDef);
			table->addColumn(columnName + ".length", index++, intType, false, odb::MDI::integerMDI(), false, bitfieldDef);
		}
		else
		{
			const type::SQLType& typ = type::SQLType::lookup(typeName);
			bool isBitmap = typ.getKind() == type::SQLType::bitmapType;
			if (isBitmap)
				//bitmap = static_cast<const type::SQLBitfield&>(typ).fields();
				bitfieldDef = static_cast<const type::SQLBitfield&>(typ).bitfieldDef();

			if (!isVector)
				// FIXME: no information about missing value in the CREATE TABLE syntax
				// TODO: choose a sensible default based upon type
				table->addColumn(columnName, index++, typ, true, odb::MDI::realMDI(), isBitmap, bitfieldDef);
			else
			{
				for (int i = start; i <= end; i++)
				{
					std::string expandedName = columnName + "_" + Translator<int,std::string>()(i);
					Log::debug() << "  === expandedName: " << expandedName << std::endl;
					// TODO: choose a sensible default based upon type
					table->addColumn(expandedName, index++, typ, true, odb::MDI::realMDI(), isBitmap, bitfieldDef);
				}
			}
		}
	}
	db.addTable(table);
	//db.setLinks(links);
	return 0;
}

} // namespace sql
} // namespace eckit

