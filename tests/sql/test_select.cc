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
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/expression/SQLExpressions.h"

using namespace eckit::testing;

namespace {
    
//----------------------------------------------------------------------------------------------------------------------

static const std::vector<long> INTEGER_DATA {9999, 8888, 7777, 6666, 5555, 4444, 3333, 2222, 1111};
static const std::vector<double> REAL_DATA {99.9, 88.8, 77.7, 66.6, 55.5, 44.4, 33.3, 22.2, 11.1};
static const std::vector<std::string> STRING_DATA {"aaaabbbb", "cccc", "dddd", "eeffg", "hijklmno",
                                                   "a-string", "a-longer-string", "another-string", ""};


class TestTable : public eckit::sql::SQLTable {

public:

    TestTable(eckit::sql::SQLDatabase& db, const std::string& path, const std::string& name) :
        SQLTable(db, path, name) {
        addColumn("icol", 0, eckit::sql::type::SQLType::lookup("integer"), false, 0);
        addColumn("scol", 1, eckit::sql::type::SQLType::lookup("string", 16), false, 0);
        addColumn("rcol", 2, eckit::sql::type::SQLType::lookup("real"), false, 0);
    }

private:

    class TestTableIterator : public eckit::sql::SQLTableIterator {
    public:
        TestTableIterator(const TestTable& owner,
                          const std::vector<std::reference_wrapper<eckit::sql::SQLColumn>>& columns) : owner_(owner), idx_(0), data_(4) {
            std::vector<size_t> offsets {0, 1, 3};
            for (const auto& col : columns) {
                offsets_.push_back(offsets[col.get().index()]);
            }
        }
    private:
        virtual ~TestTableIterator() {}
        virtual void rewind() { idx_ = 0; }
        virtual bool next() {
            if (idx_ < INTEGER_DATA.size()) {
                copyRow();
                idx_++;
                return true;
            }
            return false;
        }
        void copyRow() {
            data_[0] = INTEGER_DATA[idx_];
            ::strncpy(reinterpret_cast<char*>(&data_[1]), STRING_DATA[idx_].c_str(), 16);
            data_[3] = REAL_DATA[idx_];
        }
        virtual std::vector<size_t> columnOffsets() const { return offsets_; }
        virtual double* data() { return &data_[0]; }

        const TestTable& owner_;
        size_t idx_;
        std::vector<size_t> offsets_;
        std::vector<double> data_;
    };

    virtual eckit::sql::SQLTableIterator* iterator(const std::vector<std::reference_wrapper<eckit::sql::SQLColumn>>& columns) const {
        return new TestTableIterator(*this, columns);
    }
};

//----------------------------------------------------------------------------------------------------------------------

class TestOutput : public eckit::sql::SQLOutput {

    virtual void size(int) {}
    virtual void prepare(eckit::sql::SQLSelect&) {}
    virtual void cleanup(eckit::sql::SQLSelect&) {}
    virtual void reset() {}
    virtual void flush() {}

    virtual bool output(const eckit::sql::expression::Expressions& results) {
        for (const auto& r : results)
            r->output(*this);
        return true;
    };

    virtual void outputReal(double d, bool) { floatOutput.push_back(d); }
    virtual void outputDouble(double d, bool) { floatOutput.push_back(d); }
    virtual void outputInt(double d, bool) { intOutput.push_back(d); }
    virtual void outputUnsignedInt(double d, bool) { intOutput.push_back(d); }
    virtual void outputString(const char* s, size_t l, bool) { strOutput.push_back(std::string(s, l)); }
    virtual void outputBitfield(double d, bool) { intOutput.push_back(d); }

    virtual unsigned long long count() {
        return std::max(intOutput.size(), std::max(floatOutput.size(), strOutput.size()));
    };

public: // visible members

    std::vector<long> intOutput;
    std::vector<double> floatOutput;
    std::vector<std::string> strOutput;
};

//----------------------------------------------------------------------------------------------------------------------

CASE( "Test SQL database setup" ) {

    eckit::sql::SQLDatabase db;

    db.addTable(new TestTable(db, "a/b/c.path", "table1"));
    db.addTable(new TestTable(db, "d/e/f.path", "table2"));
}


CASE( "Test SQL select columns" ) {

    eckit::sql::SQLSession session(std::unique_ptr<TestOutput>(new TestOutput));
    eckit::sql::SQLDatabase& db(session.currentDatabase());

    db.addTable(new TestTable(db, "a/b/c.path", "table1"));
    db.addTable(new TestTable(db, "d/e/f.path", "table2"));

    std::string sql = "select scol,icol from table1";
    eckit::sql::SQLParser().parseString(session, sql);

    session.statement().execute();

    TestOutput& o(static_cast<TestOutput&>(session.output()));

    EXPECT(o.intOutput == INTEGER_DATA);
    EXPECT(o.floatOutput.empty());
    EXPECT(o.strOutput == STRING_DATA);
}


CASE( "Test SQL select all" ) {

    eckit::sql::SQLSession session(std::unique_ptr<TestOutput>(new TestOutput));
    eckit::sql::SQLDatabase& db(session.currentDatabase());

    db.addTable(new TestTable(db, "a/b/c.path", "table1"));
    db.addTable(new TestTable(db, "d/e/f.path", "table2"));

    std::string sql = "select * from table1";
    eckit::sql::SQLParser().parseString(session, sql);

    session.statement().execute();

    TestOutput& o(static_cast<TestOutput&>(session.output()));

    EXPECT(o.intOutput == INTEGER_DATA);
    EXPECT(o.floatOutput == REAL_DATA);
    EXPECT(o.strOutput == STRING_DATA);
}


CASE( "Test SQL select all" ) {

    eckit::sql::SQLSession session(std::unique_ptr<TestOutput>(new TestOutput));
    eckit::sql::SQLDatabase& db(session.currentDatabase());

    db.addTable(new TestTable(db, "a/b/c.path", "table1"));
    db.addTable(new TestTable(db, "d/e/f.path", "table2"));

    std::string sql = "select rcol from table1 where icol > 4000";
    eckit::sql::SQLParser().parseString(session, sql);

    session.statement().execute();

    TestOutput& o(static_cast<TestOutput&>(session.output()));

    EXPECT(o.intOutput == INTEGER_DATA);
    EXPECT(o.floatOutput == REAL_DATA);
    EXPECT(o.strOutput == STRING_DATA);
}


//----------------------------------------------------------------------------------------------------------------------

}


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
