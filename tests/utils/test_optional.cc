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
#include <iostream>
#include <memory>
#include <set>

#include "eckit/utils/Optional.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

Optional<long> none() {
    return Optional<long>();
}
Optional<long> number() {
    return Optional<long>(2);
}
Optional<std::vector<long>> array() {
    std::vector<long> out;
    for (int i = 0; i < 10; i++) {
        out.emplace_back(i);
    }
    return Optional<std::vector<long>>(std::move(out));
}

CASE("value") {
    {
        auto ret = none();
        ASSERT(!ret);
    }
    {
        auto ret = number();
        ASSERT(ret);

        long m = ret.value();
        ASSERT(m == 2);

        ret.value()  = 4;
        const long l = ret.value();
        ASSERT(l == 4);
    }
    {
        auto ret = array();
        ASSERT(ret);
        for (int i = 0; i < 10; i++) {
            ret()[i]++;
        }
        ASSERT(ret()[2] == 3);
    }
}

CASE("wrap") {
    {
        Optional<long> opt;
        ASSERT(!opt);

        opt = 3;
        ASSERT(opt);
        ASSERT(opt() == 3);

        auto ret = array();
        ASSERT(ret);
        for (int i = 0; i < 10; i++) {
            ret()[i]++;
        }
        Optional<std::vector<long>> out(std::move(ret()));
        ASSERT(out);
        ASSERT(out()[2] == 3);

        std::vector<long>& tmp = out();
        ASSERT(tmp[3] == 4);

        tmp[3] = 5;
        ASSERT(out()[3] == 5);

        std::vector<long> tmp2 = out();
        ASSERT(tmp2[3] == 5);

        tmp2[3] = 4;
        ASSERT(out()[3] == 5);
    }
}

CASE("copy and move assign") {
    {
        Optional<long> opt1{1};
        Optional<long> opt2{2};
        Optional<long> opt3{3};
        Optional<long> opt4;
        EXPECT(opt1.value() == 1);
        EXPECT(opt2.value() == 2);
        EXPECT(opt3.value() == 3);
        EXPECT(!opt4);

        opt1 = opt2;
        EXPECT(opt1.value() == 2);
        EXPECT(opt2);

        opt1 = std::move(opt3);
        EXPECT(opt1.value() == 3);

        opt1 = opt4;
        EXPECT(!opt1);
        EXPECT(opt3);
        opt3.reset();
        EXPECT(!opt3);

        // Self move-assignement
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif
        opt2 = std::move(opt2);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        EXPECT(opt2);
        EXPECT(opt2.value() == 2);

        // Self copy-assignement
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
        opt2 = opt2;
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        EXPECT(opt2);
        EXPECT(opt2.value() == 2);

        // Assign none
        opt2 = nullopt;
        EXPECT(!opt2);
    }
}

CASE("non-trivial object: std::string") {
    {
        Optional<std::string> opt;
        EXPECT(!opt);

        // Test copy assign value
        opt = std::string("test");
        EXPECT_EQUAL(opt.value(), std::string("test"));

        // Test move assign value
        std::string testMoveAssignValue("testMove");
        opt = std::move(testMoveAssignValue);
        EXPECT_EQUAL(opt.value(), std::string("testMove"));

        // Test copy assign some <- none
        Optional<std::string> optNone;
        opt = optNone;
        EXPECT(!opt);

        // Test copy assign none <- some
        Optional<std::string> optSome2("some2");
        opt = optSome2;
        EXPECT_EQUAL(opt.value(), std::string("some2"));

        // Test copy assign some <- some
        Optional<std::string> optSome3("some3");
        opt = optSome3;
        EXPECT_EQUAL(opt.value(), std::string("some3"));

        // Test move assign some <- some
        Optional<std::string> optTmp1("tmp1");
        opt = std::move(optTmp1);
        EXPECT(optTmp1);
        optTmp1.reset();
        EXPECT(!optTmp1);
        EXPECT_EQUAL(opt.value(), std::string("tmp1"));

        // Test move assign some <- none
        Optional<std::string> optTmp2{};
        opt = std::move(optTmp2);
        EXPECT(!optTmp2);
        optTmp2.reset();
        EXPECT(!optTmp2);
        EXPECT(!opt);

        // Test move assign none <- some
        Optional<std::string> optTmp3("tmp3");
        opt = std::move(optTmp3);
        EXPECT(optTmp3);
        optTmp3 = nullopt;
        EXPECT(!optTmp3);
        EXPECT_EQUAL(opt.value(), std::string("tmp3"));

        // Test move assign to self
        Optional<std::string> optTmp4("tmp4");
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif
        optTmp4 = std::move(optTmp4);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        EXPECT(optTmp4);
        EXPECT_EQUAL(optTmp4.value(), std::string("tmp4"));


        // Test copy construct some
        Optional<std::string> copyFromSome("copyFromSome");
        Optional<std::string> copyTo1(copyFromSome);
        EXPECT_EQUAL(copyTo1.value(), std::string("copyFromSome"));

        Optional<std::string> copyFromNone{};
        Optional<std::string> copyTo2(copyFromNone);
        EXPECT(!copyTo2);

        // Test move construct some
        Optional<std::string> moveFromSome("moveFromSome");
        Optional<std::string> moveTo1(std::move(moveFromSome));
        EXPECT(!moveFromSome);
        EXPECT_EQUAL(moveTo1.value(), std::string("moveFromSome"));

        Optional<std::string> moveFromNone{};
        Optional<std::string> moveTo2(std::move(moveFromNone));
        EXPECT(!moveFromNone);
        EXPECT(!moveTo2);
    }
}

struct StringWrap {
    std::string str;
    int uid;  // Unique id that gets generated even on movemment
    int id;   // Id that will also be moved - i.e. will be set to 0

    StringWrap(const char* v) :
        str(v), uid(++idCounter), id(uid) {}
    StringWrap(const std::string& v) :
        str(v), uid(++idCounter), id(uid) {}
    StringWrap() :
        str(), uid(++idCounter), id(uid) {}

    // Copy contructor
    StringWrap(const StringWrap& other) :
        str(other.str), uid(++idCounter), id(uid) {}
    // Move contructor
    StringWrap(StringWrap&& other) :
        str(std::move(other.str)), uid(++idCounter), id(other.id) {
        other.str = "";
        other.id  = 0;
    }

    StringWrap& operator=(const StringWrap& other) {
        str = other.str;
        return *this;
    }
    StringWrap& operator=(StringWrap&& other) {
        str      = std::move(other.str);
        id       = other.id;
        other.id = 0;
        return *this;
    }


    static int idCounter;
    static std::set<int> deletedIds;
    static std::set<int> deletedUIds;

    static bool idIsDeleted(int i) {
        return (deletedIds.find(i) != deletedIds.end());
    }
    static bool uidIsDeleted(int i) {
        return (deletedUIds.find(i) != deletedUIds.end());
    }
    static void printIdsDeleted() {
        std::cout << "Id deleted: ";
        for (int i : deletedIds) {
            std::cout << i << ", ";
        }
        std::cout << std::endl;
    }
    static void printUIdsDeleted() {
        std::cout << "UId deleted: ";
        for (int i : deletedUIds) {
            std::cout << i << ", ";
        }
        std::cout << std::endl;
    }

    ~StringWrap() {
        deletedUIds.insert(uid);
        if (id) {
            deletedIds.insert(id);
        }
    }
};
int StringWrap::idCounter = 0;
std::set<int> StringWrap::deletedIds;
std::set<int> StringWrap::deletedUIds;

CASE("test destructor on non-trivial object: copy assign value") {
    {
        Optional<StringWrap> opt;
        EXPECT(!opt);

        // Test copy assign value None <- Some
        StringWrap testCopyAssignValue1("testCopyAssignValue1");
        opt = testCopyAssignValue1;
        EXPECT_EQUAL(opt.value().str, std::string("testCopyAssignValue1"));
        int testCopyAssignValue1OptId  = opt.value().id;
        int testCopyAssignValue1OptUId = opt.value().uid;

        // Test copy assign value Some <- Some
        StringWrap testCopyAssignValue2("testCopyAssignValue2");
        int testCopyAssignValue2Id  = testCopyAssignValue2.id;
        int testCopyAssignValue2UId = testCopyAssignValue2.uid;
        opt                         = testCopyAssignValue2;
        EXPECT_EQUAL(opt.value().str, std::string("testCopyAssignValue2"));
        int testCopyAssignValue2OptId  = opt.value().id;
        int testCopyAssignValue2OptUId = opt.value().uid;
        EXPECT_NOT_EQUAL(testCopyAssignValue2OptId, testCopyAssignValue2Id);
        EXPECT_NOT_EQUAL(testCopyAssignValue2OptUId, testCopyAssignValue2UId);
        EXPECT_EQUAL(testCopyAssignValue2OptId, testCopyAssignValue1OptId);    // ID and UID is preserved on copy assignment
        EXPECT_EQUAL(testCopyAssignValue2OptUId, testCopyAssignValue1OptUId);  // ID and UID is preserved on copy assignment
        EXPECT(!StringWrap::idIsDeleted(testCopyAssignValue1OptId));           // ID and UID is preserved on copy assignment
        EXPECT(!StringWrap::uidIsDeleted(testCopyAssignValue1OptUId));         // ID and UID is preserved on copy assignment

        // Reset
        opt = Optional<StringWrap>{};
        EXPECT(!opt);
        // Reset should delete contained object
        EXPECT(StringWrap::idIsDeleted(testCopyAssignValue1OptId));
        EXPECT(StringWrap::uidIsDeleted(testCopyAssignValue1OptUId));
        EXPECT(StringWrap::idIsDeleted(testCopyAssignValue2OptId));    // Some as two lines above
        EXPECT(StringWrap::uidIsDeleted(testCopyAssignValue2OptUId));  // Some as two lines above
    }
}


CASE("test destructor on non-trivial object: move assign value") {
    {
        Optional<StringWrap> opt;
        EXPECT(!opt);

        // Test move assign value None <- Some
        StringWrap testMoveAssignValue1("testMoveAssignValue1");
        int testMoveAssignValue1Id  = testMoveAssignValue1.id;
        int testMoveAssignValue1UId = testMoveAssignValue1.uid;
        opt                         = std::move(testMoveAssignValue1);
        EXPECT_EQUAL(opt.value().str, std::string("testMoveAssignValue1"));
        int testMoveAssignValue1OptId  = opt.value().id;
        int testMoveAssignValue1OptUId = opt.value().uid;
        EXPECT_EQUAL(testMoveAssignValue1Id, testMoveAssignValue1OptId);
        EXPECT_NOT_EQUAL(testMoveAssignValue1UId, testMoveAssignValue1OptUId);
        EXPECT(!StringWrap::uidIsDeleted(testMoveAssignValue1UId));

        // Test move assign value Some <- Some
        StringWrap testMoveAssignValue2("testMoveAssignValue2");
        int testMoveAssignValue2Id  = testMoveAssignValue2.id;
        int testMoveAssignValue2UId = testMoveAssignValue2.uid;
        opt                         = std::move(testMoveAssignValue2);
        EXPECT_EQUAL(opt.value().str, std::string("testMoveAssignValue2"));
        int testMoveAssignValue2OptId  = opt.value().id;
        int testMoveAssignValue2OptUId = opt.value().uid;
        EXPECT_EQUAL(testMoveAssignValue2OptId, testMoveAssignValue2Id);
        EXPECT_NOT_EQUAL(testMoveAssignValue2OptUId, testMoveAssignValue2UId);
        EXPECT_EQUAL(testMoveAssignValue2OptUId, testMoveAssignValue1OptUId);
        EXPECT(!StringWrap::idIsDeleted(testMoveAssignValue1OptId));
        EXPECT(!StringWrap::uidIsDeleted(testMoveAssignValue1OptUId));
        EXPECT(!StringWrap::uidIsDeleted(testMoveAssignValue2UId));


        Optional<StringWrap> testCopyAssignSomeNone;
        opt = testCopyAssignSomeNone;
        EXPECT(!opt);
        // Reset should delete previous object
        EXPECT(StringWrap::idIsDeleted(testMoveAssignValue2OptId));
        EXPECT(StringWrap::uidIsDeleted(testMoveAssignValue2OptUId));
    }
}

CASE("test destructor on non-trivial object: copy & move assign optional") {
    {
        Optional<StringWrap> opt;
        EXPECT(!opt);

        // Test copy assign none <- some
        Optional<StringWrap> testCopyAssignNoneSome("testCopyAssignNoneSome");
        int testCopyAssignNoneSomeId  = testCopyAssignNoneSome.value().id;
        int testCopyAssignNoneSomeUId = testCopyAssignNoneSome.value().uid;
        opt                           = testCopyAssignNoneSome;
        EXPECT_EQUAL(opt.value().str, std::string("testCopyAssignNoneSome"));
        int testCopyAssignNoneSomeOptId  = opt.value().id;
        int testCopyAssignNoneSomeOptUId = opt.value().uid;
        EXPECT_NOT_EQUAL(testCopyAssignNoneSomeId, testCopyAssignNoneSomeOptId);
        EXPECT_NOT_EQUAL(testCopyAssignNoneSomeUId, testCopyAssignNoneSomeOptUId);
        EXPECT(!StringWrap::uidIsDeleted(testCopyAssignNoneSomeUId));
        EXPECT(!StringWrap::idIsDeleted(testCopyAssignNoneSomeId));

        // Test copy assign some <- some
        Optional<StringWrap> testCopyAssignSomeSome("testCopyAssignSomeSome");
        int testCopyAssignSomeSomeId  = testCopyAssignSomeSome.value().id;
        int testCopyAssignSomeSomeUId = testCopyAssignSomeSome.value().uid;
        opt                           = testCopyAssignSomeSome;
        EXPECT_EQUAL(opt.value().str, std::string("testCopyAssignSomeSome"));
        int testCopyAssignSomeSomeOptId  = opt.value().id;
        int testCopyAssignSomeSomeOptUId = opt.value().uid;
        EXPECT_EQUAL(testCopyAssignSomeSomeOptId, testCopyAssignNoneSomeOptId);
        EXPECT_EQUAL(testCopyAssignSomeSomeOptUId, testCopyAssignNoneSomeOptUId);
        EXPECT_NOT_EQUAL(testCopyAssignSomeSomeId, testCopyAssignNoneSomeOptId);
        EXPECT_NOT_EQUAL(testCopyAssignSomeSomeUId, testCopyAssignNoneSomeOptUId);
        EXPECT_NOT_EQUAL(testCopyAssignSomeSomeId, testCopyAssignSomeSomeOptId);
        EXPECT_NOT_EQUAL(testCopyAssignSomeSomeUId, testCopyAssignSomeSomeOptUId);
        EXPECT(!StringWrap::idIsDeleted(testCopyAssignNoneSomeOptId));
        EXPECT(!StringWrap::uidIsDeleted(testCopyAssignNoneSomeOptUId));
        EXPECT(!StringWrap::idIsDeleted(testCopyAssignSomeSomeId));
        EXPECT(!StringWrap::uidIsDeleted(testCopyAssignSomeSomeUId));

        // Test move assign some <- some
        Optional<StringWrap> testMoveAssignSomeSome("testMoveAssignSomeSome");
        int testMoveAssignSomeSomeId  = testMoveAssignSomeSome.value().id;
        int testMoveAssignSomeSomeUId = testMoveAssignSomeSome.value().uid;
        opt                           = std::move(testMoveAssignSomeSome);
        EXPECT(testMoveAssignSomeSome);
        testMoveAssignSomeSome.reset();
        EXPECT(!testMoveAssignSomeSome);
        EXPECT_EQUAL(opt.value().str, std::string("testMoveAssignSomeSome"));
        int testMoveAssignSomeSomeOptId  = opt.value().id;
        int testMoveAssignSomeSomeOptUId = opt.value().uid;
        EXPECT_NOT_EQUAL(testMoveAssignSomeSomeOptId, testCopyAssignSomeSomeOptId);  // Should have moved now
        EXPECT_EQUAL(testMoveAssignSomeSomeOptUId, testCopyAssignSomeSomeOptUId);    // Preserved
        EXPECT_NOT_EQUAL(testMoveAssignSomeSomeId, testCopyAssignNoneSomeOptId);
        EXPECT_NOT_EQUAL(testMoveAssignSomeSomeUId, testCopyAssignNoneSomeOptUId);
        EXPECT_EQUAL(testMoveAssignSomeSomeId, testMoveAssignSomeSomeOptId);
        EXPECT_NOT_EQUAL(testMoveAssignSomeSomeUId, testMoveAssignSomeSomeOptUId);
        EXPECT(!StringWrap::idIsDeleted(testCopyAssignSomeSomeOptId));
        EXPECT(!StringWrap::uidIsDeleted(testCopyAssignSomeSomeOptUId));
        EXPECT(!StringWrap::idIsDeleted(testMoveAssignSomeSomeId));
        EXPECT(StringWrap::uidIsDeleted(testMoveAssignSomeSomeUId));

        // Test move assign some <- none
        Optional<StringWrap> testMoveAssignSomeNone{};
        opt = std::move(testMoveAssignSomeNone);
        EXPECT(!testMoveAssignSomeNone);
        EXPECT(!opt);
        // Reset - kept object should have been deleted now
        EXPECT(StringWrap::idIsDeleted(testMoveAssignSomeSomeOptId));
        EXPECT(StringWrap::uidIsDeleted(testMoveAssignSomeSomeOptUId));

        // Test move assign none <- some
        Optional<StringWrap> testMoveAssignNoneSome("testMoveAssignNoneSome");
        int testMoveAssignNoneSomeId  = testMoveAssignNoneSome.value().id;
        int testMoveAssignNoneSomeUId = testMoveAssignNoneSome.value().uid;
        opt                           = std::move(testMoveAssignNoneSome);
        EXPECT(testMoveAssignNoneSome);
        testMoveAssignNoneSome.reset();
        EXPECT(!testMoveAssignNoneSome);
        EXPECT_EQUAL(opt.value().str, std::string("testMoveAssignNoneSome"));
        int testMoveAssignNoneSomeOptId  = opt.value().id;
        int testMoveAssignNoneSomeOptUId = opt.value().uid;
        EXPECT_EQUAL(testMoveAssignNoneSomeId, testMoveAssignNoneSomeOptId);
        EXPECT_NOT_EQUAL(testMoveAssignNoneSomeUId, testMoveAssignNoneSomeOptUId);
        EXPECT(!StringWrap::idIsDeleted(testMoveAssignNoneSomeId));
        EXPECT(StringWrap::uidIsDeleted(testMoveAssignNoneSomeUId));
    }
}

CASE("test destructor on non-trivial object: copy construct") {
    {
        // Test copy construct some
        Optional<StringWrap> copyFromSome("copyFromSome");
        Optional<StringWrap> copyTo1(copyFromSome);
        EXPECT_EQUAL(copyTo1.value().str, std::string("copyFromSome"));
        EXPECT_NOT_EQUAL(copyTo1.value().id, copyFromSome.value().id);
        EXPECT_NOT_EQUAL(copyTo1.value().uid, copyFromSome.value().uid);
        EXPECT(!StringWrap::idIsDeleted(copyTo1.value().id));
        EXPECT(!StringWrap::uidIsDeleted(copyTo1.value().uid));
        EXPECT(!StringWrap::idIsDeleted(copyFromSome.value().id));
        EXPECT(!StringWrap::uidIsDeleted(copyFromSome.value().uid));

        Optional<StringWrap> copyFromNone{};
        Optional<StringWrap> copyTo2(copyFromNone);
        EXPECT(!copyFromNone);
        EXPECT(!copyTo2);
    }
}

CASE("test destructor on non-trivial object: move construct") {
    {
        // Test move construct some
        Optional<StringWrap> moveFromSome("moveFromSome");
        int moveFromSomeId  = moveFromSome.value().id;
        int moveFromSomeUId = moveFromSome.value().uid;
        Optional<StringWrap> moveTo1(std::move(moveFromSome));
        EXPECT(!moveFromSome);
        EXPECT_EQUAL(moveTo1.value().str, std::string("moveFromSome"));
        EXPECT_EQUAL(moveTo1.value().id, moveFromSomeId);
        EXPECT_NOT_EQUAL(moveTo1.value().uid, moveFromSomeUId);
        EXPECT(!StringWrap::idIsDeleted(moveTo1.value().id));
        EXPECT(!StringWrap::uidIsDeleted(moveTo1.value().uid));
        EXPECT(!StringWrap::idIsDeleted(moveFromSomeId));
        EXPECT(!StringWrap::uidIsDeleted(moveFromSomeUId));

        Optional<StringWrap> moveFromNone{};
        Optional<StringWrap> moveTo2(std::move(moveFromNone));
        EXPECT(!moveFromNone);
        EXPECT(!moveTo2);
    }
}

CASE("test destructor on non-trivial object: destruct") {
    {
        int testDestructorID  = 0;
        int testDestructorUID = 0;
        {
            Optional<StringWrap> testDestructor("abuse me");
            testDestructorID  = testDestructor().id;
            testDestructorUID = testDestructor().uid;
        }
        EXPECT_NOT_EQUAL(testDestructorID, 0);
        EXPECT_NOT_EQUAL(testDestructorUID, 0);
        EXPECT(StringWrap::idIsDeleted(testDestructorID));
        EXPECT(StringWrap::uidIsDeleted(testDestructorUID));
    }
}

struct TestOptionalConstexprPOD {
    int a;
    int b;
};

// Constexpr copy is not possible in C++11 (requires c++14 anyway)
// * requires conditial branching with member values
// * allow constexpr construct values with new
CASE("constexpr") {
    {
        constexpr Optional<int> none{};
        static_assert(!none.has_value(), "Empty optional constexpr");
        constexpr Optional<int> v1(1);
        constexpr Optional<int> v2(*v1 + 4);
        static_assert(*v2 == 5, "Valued optional constexpr");

        constexpr Optional<TestOptionalConstexprPOD> pod(TestOptionalConstexprPOD{1, 2});
        static_assert(pod->b == 2, "Trivial optional constexpr");

        // Composing not possible with C++11 limitations
        // constexpr Optional<TestOptionalConstexprPOD> pod2 = bool(pod) ? Optional<TestOptionalConstexprPOD>(TestOptionalConstexprPOD{pod().a+1,2}) : Optional<TestOptionalConstexprPOD>{};

        // // Constexpr copy is not possible in C++11 (requires c++14 anyway)
        // // constexpr Optional<int> v3(v2);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
