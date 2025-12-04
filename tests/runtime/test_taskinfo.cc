/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <type_traits>
#include <vector>

#include "eckit/memory/Padded.h"
#include "eckit/runtime/Main.h"
#include "eckit/runtime/TaskInfo.h"
#include "eckit/testing/Test.h"


namespace eckit::test {

CASE("TaskInfo correctly initializes all members on placement new") {
    // Ensure we TaskInfo is a wrapper around Padded<Info, 4096> with no
    // additional fields so that we can test Padded<Info, 4096> as proxy of TaskInfo
    static_assert(sizeof(Padded<Info, 4096>) == sizeof(TaskInfo));
    static_assert(std::is_trivially_copyable_v<Padded<Info, 4096>>);

    const auto test_start_time = ::time(nullptr);

    auto getTimeOfDayNow = []() {
        ::timeval t{};
        ::gettimeofday(&t, nullptr);
        return t;
    };
    const auto test_start_time_of_day = getTimeOfDayNow();

    auto timevalCmp = [](const struct timeval& a, const struct timeval& b) {
        if (a.tv_sec < b.tv_sec)
            return -1;
        if (a.tv_sec > b.tv_sec)
            return 1;
        if (a.tv_usec < b.tv_usec)
            return -1;
        if (a.tv_usec > b.tv_usec)
            return 1;
        return 0;
    };

    alignas(TaskInfo) std::vector<uint8_t> buffer(sizeof(TaskInfo), 0xAF);
    // We *could* just reinterpret_cast TaskInfo into Padded<Info, 4096> but this is
    // undefined behaviour because Padded<Info, 4096> is not standart layout altough it
    // is the same layout. Hence we memcpy
    auto sut = new (buffer.data()) TaskInfo();
    Padded<Info, 4096> info{};
    ::memcpy(&info, sut, sizeof(info));
    EXPECT_EQUAL(info.busy_, true);
    EXPECT_EQUAL(info.thread_, pthread_self());
    EXPECT_EQUAL(info.pid_, getpid());
    // start is initialized to now on construction, once we reach this line
    // we no longer know when this was, so we just chcek for later than test start
    // but earler than now.
    EXPECT(info.start_ >= test_start_time);
    EXPECT(info.start_ <= ::time(nullptr));
    // last_ and check_ are initialized to now on construction, once we reach this line
    // we no longer know when this was, so we just chcek for later than test start
    // but earler than now.
    EXPECT(info.last_ >= test_start_time);
    EXPECT(info.last_ <= ::time(nullptr));
    EXPECT(info.check_ >= test_start_time);
    EXPECT(info.check_ <= ::time(nullptr));
    EXPECT(info.show_);
    EXPECT_EQUAL(info.late_, 0);
    EXPECT(std::all_of(info.buffer_, info.buffer_ + Info::size_, [](auto v) { return v == 0x00; }));
    EXPECT_EQUAL(info.pos_, 0);
    std::array<char, 80> expected_name{};
    ::strncpy(expected_name.begin(), Main::instance().displayName().c_str(), expected_name.size() - 1);
    EXPECT_EQUAL(::memcmp(info.name_, expected_name.begin(), expected_name.size()), 0);
    std::array<char, 80> expected_kind{};
    ::strncpy(expected_kind.begin(), Main::instance().name().c_str(), expected_kind.size() - 1);
    EXPECT_EQUAL(::memcmp(info.kind_, expected_kind.begin(), expected_kind.size()), 0);
    std::array<char, 256> expected_status{};
    ::strncpy(expected_status.begin(), "Starting", expected_status.size() - 1);
    EXPECT_EQUAL(::memcmp(info.status_, expected_status.begin(), expected_status.size()), 0);
    std::array<char, 80> expected_application{};
    ::strncpy(expected_application.begin(), Main::instance().name().c_str(), expected_application.size() - 1);
    EXPECT_EQUAL(::memcmp(info.application_, expected_application.begin(), expected_application.size()), 0);
    EXPECT_EQUAL(info.progress_.min_, 0);
    EXPECT_EQUAL(info.progress_.max_, 0);
    EXPECT_EQUAL(info.progress_.val_, 0);
    EXPECT(std::all_of(info.progress_.name_, info.progress_.name_ + sizeof(info.progress_.name_),
                       [](auto v) { return v == 0x00; }));
    EXPECT_EQUAL(info.progress_.rate_, 0);
    EXPECT_EQUAL(info.progress_.speed_, 0);
    // progress_.start and progress_.last_ are initialized to time of day now
    // on construction, once we reach this line we no longer know when this
    // was, so we just chcek for later than test start but earler than now.
    EXPECT(timevalCmp(info.progress_.start_, test_start_time_of_day) >= 0);
    EXPECT(timevalCmp(info.progress_.start_, getTimeOfDayNow()) <= 0);
    EXPECT(timevalCmp(info.progress_.last_, test_start_time_of_day) >= 0);
    EXPECT(timevalCmp(info.progress_.last_, getTimeOfDayNow()) <= 0);
    EXPECT_EQUAL(info.taskID_, 0);
    EXPECT_EQUAL(info.stop_, false);
    EXPECT_EQUAL(info.abort_, false);
    EXPECT_EQUAL(info.stoppable_, true);
    EXPECT_EQUAL(info.stopped_, false);
    EXPECT_EQUAL(info.canceled_, false);
    EXPECT_EQUAL(info.exception_, false);
    EXPECT(std::all_of(info.cancelMsg_, info.cancelMsg_ + sizeof(info.cancelMsg_), [](auto v) { return v == 0x00; }));
    EXPECT_EQUAL(info.config_, 0);
    EXPECT_EQUAL(info.resource_, 0);
    EXPECT_EQUAL(info.parent_, -1);
    EXPECT_EQUAL(info.depth_, 0);
    EXPECT_EQUAL(info.state_, ' ');
    EXPECT_EQUAL(info.port_, 0);
    EXPECT(std::all_of(info.host_, info.host_ + sizeof(info.host_), [](auto v) { return v == 0x00; }));
    EXPECT(std::all_of(info.message_, info.message_ + sizeof(info.message_), [](auto v) { return v == 0x00; }));
}

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
