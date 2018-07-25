/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/testing/Test.h"
#include "eckit/sql/SQLDatabase.h"

using namespace eckit::testing;

namespace {
    
//----------------------------------------------------------------------------------------------------------------------

class TestTable : public eckit::sql::SQLTable {

public:

    using eckit::sql::SQLTable::SQLTable;

private:

    virtual eckit::sql::SQLTableIterator* iterator(const std::vector<std::reference_wrapper<eckit::sql::SQLColumn>>&) const {
        NOTIMP;
    }

    virtual eckit::sql::SQLColumn* createSQLColumn(const eckit::sql::type::SQLType& type,
                                                   const std::string& name,
                                                   int index,
                                                   bool hasMissingValue,
                                                   double missingValue,
                                                   const eckit::sql::BitfieldDef&) { NOTIMP; }

    virtual eckit::sql::SQLColumn* createSQLColumn(const eckit::sql::type::SQLType& type,
                                                   const std::string& name,
                                                   int index,
                                                   bool hasMissingValue,
                                                   double missingValue) { NOTIMP; }

};

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test SQL database setup" ) {

    eckit::sql::SQLDatabase db;

    db.addTable(new TestTable(db, "a/b/c.path", "table1"));
    db.addTable(new TestTable(db, "d/e/f.path", "table2"));
}

//----------------------------------------------------------------------------------------------------------------------

}


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
