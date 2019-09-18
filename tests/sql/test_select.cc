/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/sql/SQLColumn.h"
#include "eckit/sql/SQLDatabase.h"
#include "eckit/sql/SQLOutput.h"
#include "eckit/sql/SQLParser.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/testing/Test.h"

using namespace eckit::testing;

namespace {

//----------------------------------------------------------------------------------------------------------------------

static const std::vector<long> INTEGER_DATA{9999, 8888, 7777, 6666, 6666, 6666, 4444, 3333, 2222, 1111};
static const std::vector<double> REAL_DATA{99.9, 88.8, 77.7, 66.6, 66.6, 88.8, 44.4, 33.3, 22.2, 11.1};
static const std::vector<std::string> STRING_DATA{
    "a-longer-string", "cccc",           "cccc",           "cccc", "cccc", "hijklmno", "aaaabbbb",
    "a-longer-string", "another-string", "another-string2"};


class TestTable : public eckit::sql::SQLTable {

public:
    TestTable(eckit::sql::SQLDatabase& db, const std::string& path, const std::string& name) :
        SQLTable(db, path, name) {
        addColumn("icol", 0, eckit::sql::type::SQLType::lookup("integer"), false, 0);
        addColumn("scol", 1, eckit::sql::type::SQLType::lookup("string", 2), false, 0);
        addColumn("rcol", 2, eckit::sql::type::SQLType::lookup("real"), false, 0);
    }

private:
    class TestTableIterator : public eckit::sql::SQLTableIterator {
    public:
        TestTableIterator(const TestTable& owner,
                          const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns) :
            owner_(owner),
            idx_(0),
            data_(4) {
            std::vector<size_t> offsets{0, 1, 3};
            std::vector<size_t> doublesSizes{1, 2, 1};
            for (const auto& col : columns) {
                offsets_.push_back(offsets[col.get().index()]);
                doublesSizes_.push_back(doublesSizes[col.get().index()]);
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
        virtual std::vector<size_t> doublesDataSizes() const { return doublesSizes_; }
        virtual const double* data() const { return &data_[0]; }

        const TestTable& owner_;
        size_t idx_;
        std::vector<size_t> offsets_;
        std::vector<size_t> doublesSizes_;
        std::vector<double> data_;
    };

    virtual eckit::sql::SQLTableIterator* iterator(
        const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
        std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback) const {
        // TODO: Test callback
        return new TestTableIterator(*this, columns);
    }
};

//----------------------------------------------------------------------------------------------------------------------

class TestOutput : public eckit::sql::SQLOutput {

    virtual void prepare(eckit::sql::SQLSelect&) {}
    virtual void cleanup(eckit::sql::SQLSelect&) {}
    virtual void reset() {
        intOutput_.clear();
        floatOutput_.clear();
        strOutput_.clear();
    }
    virtual void flush() {
        std::swap(intOutput_, intOutput);
        std::swap(floatOutput_, floatOutput);
        std::swap(strOutput_, strOutput);
    }

    virtual bool output(const eckit::sql::expression::Expressions& results) {
        for (const auto& r : results) {
            r->output(*this);
        }
        //        eckit::Log::info() << "..." << intOutput_ << floatOutput_ << strOutput_ << std::endl;
        return true;
    }

    virtual void outputReal(double d, bool) { floatOutput_.push_back(d); }
    virtual void outputDouble(double d, bool) { floatOutput_.push_back(d); }
    virtual void outputInt(double d, bool) { intOutput_.push_back(d); }
    virtual void outputUnsignedInt(double d, bool) { intOutput_.push_back(d); }
    virtual void outputString(const char* s, size_t l, bool missing) {
        if (missing) {
            strOutput_.push_back(std::string());
        }
        else {
            ASSERT(s);
            strOutput_.push_back(std::string(s, l));
        }
    }
    virtual void outputBitfield(double d, bool) { intOutput_.push_back(d); }

    virtual unsigned long long count() {
        return std::max(intOutput.size(), std::max(floatOutput_.size(), strOutput_.size()));
    }

public:  // visible members
    std::vector<long> intOutput_;
    std::vector<double> floatOutput_;
    std::vector<std::string> strOutput_;

    std::vector<long> intOutput;
    std::vector<double> floatOutput;
    std::vector<std::string> strOutput;
};

//----------------------------------------------------------------------------------------------------------------------

// TODO: Test explicit vs implicit table
// TODO: Test if database has a table

CASE("Test SQL database setup") {

    eckit::sql::SQLDatabase db;

    db.addTable(new TestTable(db, "a/b/c.path", "table1"));
    db.addTable(new TestTable(db, "d/e/f.path", "table2"));
}


CASE("Select from constructed table") {

    eckit::sql::SQLSession session(std::unique_ptr<TestOutput>(new TestOutput));
    eckit::sql::SQLDatabase& db(session.currentDatabase());

    db.addTable(new TestTable(db, "a/b/c.path", "table1"));

    TestOutput& o(static_cast<TestOutput&>(session.output()));

    // All of the different types of select from this standard table.

    SECTION("Test SQL select columns") {

        std::string sql = "select scol,icol from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        EXPECT(o.intOutput == INTEGER_DATA);
        EXPECT(o.floatOutput.empty());
        EXPECT(o.strOutput == STRING_DATA);
    }


    SECTION("Test SQL select all") {

        std::string sql = "select * from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        EXPECT(o.intOutput == INTEGER_DATA);
        EXPECT(o.floatOutput == REAL_DATA);
        EXPECT(o.strOutput == STRING_DATA);
    }


    SECTION("Test SQL select where") {

        std::string sql = "select rcol from table1 where icol > 4000";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        EXPECT(o.intOutput.empty());
        std::vector<double> expected{99.9, 88.8, 77.7, 66.6, 66.6, 88.8, 44.4};
        EXPECT(o.floatOutput == expected);
        EXPECT(o.strOutput.empty());
    }


    SECTION("Test SQL select where long string") {

        std::string sql = "select rcol from table1 where scol == \"another-string\"";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        EXPECT(o.intOutput.empty());
        std::vector<double> expected{22.2};
        EXPECT(o.floatOutput == expected);
        EXPECT(o.strOutput.empty());
    }

    SECTION("Test SQL select distinct") {

        std::vector<std::string> queries = {"select distinct icol from table1", "select distinct rcol from table1",
                                            "select distinct scol from table1",
                                            "select distinct icol,rcol from table1"};

        for (size_t i = 0; i < queries.size(); i++) {

            eckit::sql::SQLParser().parseString(session, queries[i]);
            session.statement().execute();

            if (i == 0) {
                EXPECT(o.intOutput == std::vector<long>({9999, 8888, 7777, 6666, 4444, 3333, 2222, 1111}));
                EXPECT(o.floatOutput.empty() && o.strOutput.empty());
            }
            else if (i == 1) {
                EXPECT(o.floatOutput == std::vector<double>({99.9, 88.8, 77.7, 66.6, 44.4, 33.3, 22.2, 11.1}));
                EXPECT(o.intOutput.empty() && o.strOutput.empty());
            }
            else if (i == 2) {
                EXPECT(o.strOutput == std::vector<std::string>({"a-longer-string", "cccc", "hijklmno", "aaaabbbb",
                                                                "another-string", "another-string2"}));
                EXPECT(o.intOutput.empty() && o.floatOutput.empty());
            }
            else {
                // Test that it is finding unique rows, not values (n.b. 6666/88.8)
                EXPECT(o.intOutput == std::vector<long>({9999, 8888, 7777, 6666, 6666, 4444, 3333, 2222, 1111}));
                EXPECT(o.floatOutput == std::vector<double>({99.9, 88.8, 77.7, 66.6, 88.8, 44.4, 33.3, 22.2, 11.1}));
                EXPECT(o.strOutput.empty());
            }
        }
    }

    SECTION("Test SQL select order_by") {

        std::vector<std::string> queries = {
            "select icol from table1 order by icol ASC",
            "select icol from table1 order by icol",
            "select icol from table1 order by icol DESC",
            "select distinct icol from table1 order by icol",
            "select distinct rcol,scol from table1 order by scol DESC, icol ASC",  // n.b. different order and result
                                                                                   // contents
        };

        std::vector<std::vector<long>> vals = {{1111, 2222, 3333, 4444, 6666, 6666, 6666, 7777, 8888, 9999},
                                               {1111, 2222, 3333, 4444, 6666, 6666, 6666, 7777, 8888, 9999},
                                               {9999, 8888, 7777, 6666, 6666, 6666, 4444, 3333, 2222, 1111},
                                               {1111, 2222, 3333, 4444, 6666, 7777, 8888, 9999}};

        for (size_t i = 0; i < queries.size(); i++) {

            eckit::sql::SQLParser().parseString(session, queries[i]);
            session.statement().execute();

            if (i < 4) {
                EXPECT(o.intOutput == vals[i]);
                EXPECT(o.floatOutput.empty() && o.strOutput.empty());
            }
            else {
                EXPECT(o.intOutput.empty());
                EXPECT(o.floatOutput == std::vector<double>({88.8, 66.6, 77.7, 88.8, 11.1, 22.2, 44.4, 33.3, 99.9}));
                EXPECT(o.strOutput ==
                       std::vector<std::string>({"hijklmno", "cccc", "cccc", "cccc", "another-string2",
                                                 "another-string", "aaaabbbb", "a-longer-string", "a-longer-string"}));
            }
        }
    }

}  // Testing SQL select from standard table


CASE("Test with implicit tables") {

    eckit::sql::SQLSession session(std::unique_ptr<TestOutput>(new TestOutput));
    eckit::sql::SQLDatabase& db(session.currentDatabase());

    db.addImplicitTable(new TestTable(db, "a/b/c.path", "table1"));

    TestOutput& o(static_cast<TestOutput&>(session.output()));

    // All of the different types of select from this standard table.

    SECTION("Test SQL select columns") {

        std::string sql = "select scol,icol";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        EXPECT(o.intOutput == INTEGER_DATA);
        EXPECT(o.floatOutput.empty());
        EXPECT(o.strOutput == STRING_DATA);
    }


    SECTION("Test SQL select all") {

        std::string sql = "select *";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        EXPECT(o.intOutput == INTEGER_DATA);
        EXPECT(o.floatOutput == REAL_DATA);
        EXPECT(o.strOutput == STRING_DATA);
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
