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
#include "eckit/sql/SQLSelect.h"
#include "eckit/sql/SQLSession.h"
#include "eckit/sql/SQLStatement.h"
#include "eckit/sql/expression/SQLExpressions.h"
#include "eckit/sql/type/SQLBitfield.h"
#include "eckit/testing/Test.h"

using namespace eckit::testing;

namespace {

//----------------------------------------------------------------------------------------------------------------------

static const std::vector<long> INTEGER_DATA{9999, 8888, 7777, 6666, 6666, 6666, 4444, 3333, 2222, 1111};
static const std::vector<double> REAL_DATA{99.9, 88.8, 77.7, 66.6, 66.6, 88.8, 44.4, 33.3, 22.2, 11.1};
static const std::vector<std::string> STRING_DATA{"cccc", "a-longer-string", "cccc", "cccc",
                                                  "cccc", "hijklmno", "aaaabbbb", "a-longer-string",
                                                  "another-string", "another-string2"};

static const std::vector<long> BITFIELD_DATA{0, 1, 2, 4, 8, 9, 10, 12, 13, 15};
static const std::vector<long> BF1_DATA{0, 1, 0, 0, 0, 1, 0, 0, 1, 1};
static const std::vector<long> BF2_DATA{0, 0, 1, 2, 0, 0, 1, 2, 2, 3};
static const std::vector<long> BF3_DATA{0, 0, 0, 0, 1, 1, 1, 1, 1, 1};


class TestTable : public eckit::sql::SQLTable {

public:
    TestTable(eckit::sql::SQLDatabase& db, const std::string& path, const std::string& name) :
        SQLTable(db, path, name) {
        addColumn("icol", 0, eckit::sql::type::SQLType::lookup("integer"), false, 0);
        addColumn("scol", 1, eckit::sql::type::SQLType::lookup("string", 1), false, 0);
        addColumn("rcol", 2, eckit::sql::type::SQLType::lookup("real"), false, 0);

        std::vector<std::string> bfNames = {"bf1", "bf2", "bf3"};
        std::vector<int32_t> bfSizes     = {1, 2, 1};
        std::string bfType               = eckit::sql::type::SQLBitfield::make("Bitfield", bfNames, bfSizes, "dummy");
        addColumn("bfcolumn", 3, eckit::sql::type::SQLType::lookup(bfType), false, 0, true, std::make_pair(bfNames, bfSizes));
        addColumn("bgcolumn@tbl1", 4, eckit::sql::type::SQLType::lookup(bfType), false, 0, true, std::make_pair(bfNames, bfSizes));
        addColumn("bgcolumn@tbl2", 5, eckit::sql::type::SQLType::lookup(bfType), false, 0, true, std::make_pair(bfNames, bfSizes));
        addColumn("preselected.bfcolumn", 6, eckit::sql::type::SQLType::lookup("integer"), false, 0);
    }

private:
    class TestTableIterator : public eckit::sql::SQLTableIterator {
    public:
        TestTableIterator(const TestTable& owner,
                          const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
                          std::function<void(eckit::sql::SQLTableIterator&)> updateCallback) :
            /* owner_(owner), */ idx_(0), data_(8), updateCallback_(updateCallback) {
            std::vector<size_t> offsets{0, 1, 2, 3, 4, 5, 6};
            std::vector<size_t> doublesSizes{1, 1, 1, 1, 1, 1, 1};
            for (const auto& col : columns) {
                columnIndexes_.push_back(col.get().index());
                offsets_.push_back(offsets[col.get().index()]);
                doublesSizes_.push_back(doublesSizes[col.get().index()]);
                hasMissing_.push_back(false);
                missingVals_.push_back(0);
            }
        }

    private:
        ~TestTableIterator() override {}
        void rewind() override { idx_ = 0; }
        bool next() override {

            // After the first element, we resize things, so we can test the callback and type resizing
            // functionality

            if (idx_ == 0) {
                offsets_.clear();
                doublesSizes_.clear();
                std::vector<size_t> offsets{0, 1, 3, 4, 5, 6, 7};
                std::vector<size_t> doublesSizes{1, 2, 1, 1, 1, 1, 1};
                for (const auto& idx : columnIndexes_) {
                    offsets_.push_back(offsets[idx]);
                    doublesSizes_.push_back(doublesSizes[idx]);
                }
                updateCallback_(*this);
            }

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
            data_[4] = BITFIELD_DATA[idx_];
            data_[5] = BITFIELD_DATA[idx_];
            data_[6] = BITFIELD_DATA[idx_];
            data_[7] = INTEGER_DATA[idx_];
        }
        std::vector<size_t> columnOffsets() const override { return offsets_; }
        std::vector<size_t> doublesDataSizes() const override { return doublesSizes_; }
        std::vector<char> columnsHaveMissing() const override { return hasMissing_; }
        std::vector<double> missingValues() const override { return missingVals_; }
        const double* data() const override { return &data_[0]; }

        // const TestTable& owner_; // unused
        size_t idx_;
        std::vector<size_t> offsets_;
        std::vector<size_t> doublesSizes_;
        std::vector<size_t> columnIndexes_;
        std::vector<char> hasMissing_;
        std::vector<double> missingVals_;
        std::vector<double> data_;
        std::function<void(eckit::sql::SQLTableIterator&)> updateCallback_;
    };

    eckit::sql::SQLTableIterator* iterator(
        const std::vector<std::reference_wrapper<const eckit::sql::SQLColumn>>& columns,
        std::function<void(eckit::sql::SQLTableIterator&)> metadataUpdateCallback) const override {
        return new TestTableIterator(*this, columns, metadataUpdateCallback);
    }
};

//----------------------------------------------------------------------------------------------------------------------

class TestOutput : public eckit::sql::SQLOutput {

    void cleanup(eckit::sql::SQLSelect&) override {}
    void reset() override {
        intOutput_.clear();
        floatOutput_.clear();
        strOutput_.clear();
    }
    void flush() override {
        std::swap(intOutput_, intOutput);
        std::swap(floatOutput_, floatOutput);
        std::swap(strOutput_, strOutput);
    }

    bool output(const eckit::sql::expression::Expressions& results) override {
        for (const auto& r : results) {
            r->output(*this);
        }
        //        eckit::Log::info() << "..." << intOutput_ << floatOutput_ << strOutput_ << std::endl;
        return true;
    }

    void prepare(eckit::sql::SQLSelect& sql) override {
        columnNames.clear();
        const eckit::sql::expression::Expressions& columns(sql.output());
        for (const auto& col : columns) {
            columnNames.push_back(col->title());
        }
    }

    void outputReal(double d, bool) override { floatOutput_.push_back(d); }
    void outputDouble(double d, bool) override { floatOutput_.push_back(d); }
    void outputInt(double d, bool) override { intOutput_.push_back(d); }
    void outputUnsignedInt(double d, bool) override { intOutput_.push_back(d); }
    void outputString(const char* s, size_t l, bool missing) override {
        if (missing) {
            strOutput_.push_back(std::string());
        }
        else {
            ASSERT(s);
            strOutput_.push_back(std::string(s, l));
        }
    }
    void outputBitfield(double d, bool) override { intOutput_.push_back(d); }

    unsigned long long count() override {
        return std::max(intOutput.size(), std::max(floatOutput_.size(), strOutput_.size()));
    }

public:  // visible members
    std::vector<long> intOutput_;
    std::vector<double> floatOutput_;
    std::vector<std::string> strOutput_;

    std::vector<long> intOutput;
    std::vector<double> floatOutput;
    std::vector<std::string> strOutput;
    std::vector<std::string> columnNames;
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
        eckit::Log::info() << "Column names; " << o.columnNames << std::endl;
        EXPECT(o.columnNames == std::vector<std::string>({"scol", "icol"}));
    }


    SECTION("Test SQL select all") {

        std::string sql = "select * from table1";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        for (int row = 0; row < INTEGER_DATA.size(); ++row) {
            EXPECT(o.intOutput[5 * row] == INTEGER_DATA[row]);
            for (int j = 0; j < 3; ++j) {
                EXPECT(o.intOutput[5 * row + 1 + j] == BITFIELD_DATA[row]);
            }
            EXPECT(o.intOutput[5 * row + 4] == INTEGER_DATA[row]);
        }
        EXPECT(o.floatOutput == REAL_DATA);
        EXPECT(o.strOutput == STRING_DATA);
        EXPECT(o.columnNames == std::vector<std::string>({"icol", "scol", "rcol", "bfcolumn", "bgcolumn@tbl1", "bgcolumn@tbl2", "preselected.bfcolumn"}));
    }


    SECTION("Test SQL select where") {

        std::string sql = "select rcol from table1 where icol > 4000";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        EXPECT(o.intOutput.empty());
        std::vector<double> expected{99.9, 88.8, 77.7, 66.6, 66.6, 88.8, 44.4};
        EXPECT(o.floatOutput == expected);
        EXPECT(o.strOutput.empty());
        EXPECT(o.columnNames == std::vector<std::string>({"rcol"}));
    }


    SECTION("Test SQL select where long string") {

        std::string sql = "select rcol from table1 where scol == \"another-string\"";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        EXPECT(o.intOutput.empty());
        std::vector<double> expected{22.2};
        EXPECT(o.floatOutput == expected);
        EXPECT(o.strOutput.empty());
        EXPECT(o.columnNames == std::vector<std::string>({"rcol"}));
    }

    SECTION("Test SQL select distinct") {

        std::vector<std::string> queries = {"select distinct icol from table1",
                                            "select distinct rcol from table1",
                                            "select distinct scol from table1",
                                            "select distinct icol,rcol from table1"};

        for (size_t i = 0; i < queries.size(); i++) {

            eckit::sql::SQLParser().parseString(session, queries[i]);
            session.statement().execute();

            if (i == 0) {
                EXPECT(o.intOutput == std::vector<long>({9999, 8888, 7777, 6666, 4444, 3333, 2222, 1111}));
                EXPECT(o.floatOutput.empty() && o.strOutput.empty());
                EXPECT(o.columnNames == std::vector<std::string>({"icol"}));
            }
            else if (i == 1) {
                EXPECT(o.floatOutput == std::vector<double>({99.9, 88.8, 77.7, 66.6, 44.4, 33.3, 22.2, 11.1}));
                EXPECT(o.intOutput.empty() && o.strOutput.empty());
                EXPECT(o.columnNames == std::vector<std::string>({"rcol"}));
            }
            else if (i == 2) {
                EXPECT(o.strOutput == std::vector<std::string>({"cccc", "a-longer-string", "hijklmno", "aaaabbbb", "another-string", "another-string2"}));
                EXPECT(o.intOutput.empty() && o.floatOutput.empty());
                EXPECT(o.columnNames == std::vector<std::string>({"scol"}));
            }
            else {
                // Test that it is finding unique rows, not values (n.b. 6666/88.8)
                EXPECT(o.intOutput == std::vector<long>({9999, 8888, 7777, 6666, 6666, 4444, 3333, 2222, 1111}));
                EXPECT(o.floatOutput == std::vector<double>({99.9, 88.8, 77.7, 66.6, 88.8, 44.4, 33.3, 22.2, 11.1}));
                EXPECT(o.strOutput.empty());
                EXPECT(o.columnNames == std::vector<std::string>({"icol", "rcol"}));
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
                EXPECT(o.columnNames == std::vector<std::string>({"icol"}));
            }
            else {
                EXPECT(o.intOutput.empty());
                EXPECT(o.floatOutput == std::vector<double>({88.8, 66.6, 77.7, 99.9, 11.1, 22.2, 44.4, 33.3, 88.8}));
                EXPECT(o.strOutput == std::vector<std::string>({"hijklmno", "cccc", "cccc", "cccc", "another-string2", "another-string", "aaaabbbb", "a-longer-string", "a-longer-string"}));
                EXPECT(o.columnNames == std::vector<std::string>({"rcol", "scol"}));
            }
        }
    }

    SECTION("Test selection of bitfield bit columns") {

        // n.b. ensure that we check the ability to:
        // i) Find fields in a bitfield column
        // ii) Find fields in a bitfield column (plus table name)
        // iii) Find fields in a bitfield column (auto table name)

        std::string sql = "select bfcolumn.bf1, bgcolumn.bf3@tbl1, bfcolumn.bf2, bgcolumn.bf1@tbl2 from table1";
        eckit::sql::SQLParser().parseString(session, sql);
        session.statement().execute();

        EXPECT(o.columnNames == std::vector<std::string>({"bfcolumn.bf1", "bgcolumn.bf3@tbl1", "bfcolumn.bf2", "bgcolumn.bf1@tbl2"}));
        for (int row = 0; row < INTEGER_DATA.size(); ++row) {
            EXPECT(o.intOutput[4 * row] == BF1_DATA[row]);
            EXPECT(o.intOutput[4 * row + 1] == BF3_DATA[row]);
            EXPECT(o.intOutput[4 * row + 2] == BF2_DATA[row]);
            EXPECT(o.intOutput[4 * row + 3] == BF1_DATA[row]);
        }
    }

    SECTION("Test selection of a column that looks-like a bitfield") {

        std::string sql = "select preselected.bfcolumn from table1";
        eckit::sql::SQLParser().parseString(session, sql);
        session.statement().execute();

        EXPECT(o.columnNames == std::vector<std::string>({"preselected.bfcolumn"}));
        for (int row = 0; row < INTEGER_DATA.size(); ++row) {
            EXPECT(o.intOutput[row] == INTEGER_DATA[row]);
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
        EXPECT(o.columnNames == std::vector<std::string>({"scol", "icol"}));
    }


    SECTION("Test SQL select all") {

        std::string sql = "select *";
        eckit::sql::SQLParser().parseString(session, sql);

        session.statement().execute();

        for (int row = 0; row < INTEGER_DATA.size(); ++row) {
            EXPECT(o.intOutput[5 * row] == INTEGER_DATA[row]);
            for (int j = 0; j < 3; ++j) {
                EXPECT(o.intOutput[5 * row + 1 + j] == BITFIELD_DATA[row]);
            }
            EXPECT(o.intOutput[5 * row + 4] == INTEGER_DATA[row]);
        }
        EXPECT(o.floatOutput == REAL_DATA);
        EXPECT(o.strOutput == STRING_DATA);
        EXPECT(o.columnNames == std::vector<std::string>({"icol", "scol", "rcol", "bfcolumn", "bgcolumn@tbl1", "bgcolumn@tbl2", "preselected.bfcolumn"}));
    }

    SECTION("Test selection of bitfield bit columns") {

        std::string sql = "select bfcolumn.bf1, bgcolumn.bf3@tbl1, bfcolumn.bf2, bgcolumn.bf1@tbl2";
        eckit::sql::SQLParser().parseString(session, sql);
        session.statement().execute();

        EXPECT(o.columnNames == std::vector<std::string>({"bfcolumn.bf1", "bgcolumn.bf3@tbl1", "bfcolumn.bf2", "bgcolumn.bf1@tbl2"}));
        for (int row = 0; row < INTEGER_DATA.size(); ++row) {
            EXPECT(o.intOutput[4 * row] == BF1_DATA[row]);
            EXPECT(o.intOutput[4 * row + 1] == BF3_DATA[row]);
            EXPECT(o.intOutput[4 * row + 2] == BF2_DATA[row]);
            EXPECT(o.intOutput[4 * row + 3] == BF1_DATA[row]);
        }
    }

    SECTION("Test selection of a column that looks-like a bitfield") {

        std::string sql = "select preselected.bfcolumn";
        eckit::sql::SQLParser().parseString(session, sql);
        session.statement().execute();

        EXPECT(o.columnNames == std::vector<std::string>({"preselected.bfcolumn"}));
        for (int row = 0; row < INTEGER_DATA.size(); ++row) {
            EXPECT(o.intOutput[row] == INTEGER_DATA[row]);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace


int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
