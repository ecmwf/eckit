// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <atomic>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>
#include <thread>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"

namespace eckit::test {

namespace {

class Closeable {
public:

    enum class Failure : std::uint8_t {
        None,
        StdException,
        NonStdException
    };

    explicit Closeable(Failure failure = Failure::None) : failure_{failure} {}

    void close() {
        ++closeCalls_;
        switch (failure_) {
            case Failure::StdException:
                throw std::runtime_error{"close failed"};
            case Failure::NonStdException:
                throw 7;
            case Failure::None:
                break;
        }
    }

    int closeCalls() const { return closeCalls_; }

private:

    Failure failure_;
    int closeCalls_ = 0;
};

class ConstCloseable {
public:

    void close() const { ++closeCalls_; }

    int closeCalls() const { return closeCalls_; }

private:

    mutable int closeCalls_ = 0;
};

/// Releases the handle, then reports the failure.
class HandleFailingOnClose : public MemoryHandle {
public:

    HandleFailingOnClose() : MemoryHandle(64) {}

    void close() override {
        MemoryHandle::close();
        throw WriteError("close failed");
    }
};

/// Creates an AutoCloser in its destructor, which must not throw while another exception propagates.
class GuardInDestructor {
public:

    explicit GuardInDestructor(Closeable& resource) : resource_{resource} {}

    GuardInDestructor(const GuardInDestructor&)            = delete;
    GuardInDestructor& operator=(const GuardInDestructor&) = delete;
    GuardInDestructor(GuardInDestructor&&)                 = delete;
    GuardInDestructor& operator=(GuardInDestructor&&)      = delete;

    ~GuardInDestructor() { AutoCloser closer{resource_}; }

private:

    Closeable& resource_;
};

void appendToString(void* data, const char* message) {
    static_cast<std::string*>(data)->append(message);
}

}  // namespace

//----------------------------------------------------------------------------------------------------------------------

CASE("AutoCloser: closes the object when the scope is left") {
    Closeable resource;
    {
        AutoCloser closer{resource};
        EXPECT_EQUAL(resource.closeCalls(), 0);
    }
    EXPECT_EQUAL(resource.closeCalls(), 1);
}

CASE("AutoCloser: closer() makes a guard that closes at the end of the scope") {
    Closeable resource;
    {
        auto guard = closer(resource);
        EXPECT_EQUAL(resource.closeCalls(), 0);
    }
    EXPECT_EQUAL(resource.closeCalls(), 1);
}

CASE("AutoCloser: a failed close reaches the caller when the scope is left normally") {
    Closeable resource{Closeable::Failure::StdException};
    EXPECT_THROWS_AS(([&] { AutoCloser closer{resource}; })(), std::runtime_error);
    EXPECT_EQUAL(resource.closeCalls(), 1);

    Closeable nonStdFailing{Closeable::Failure::NonStdException};
    int thrown = 0;
    try {
        AutoCloser closer{nonStdFailing};
    }
    catch (int value) {
        thrown = value;
    }
    EXPECT_EQUAL(thrown, 7);
}

CASE("AutoCloser: a failed close does not replace the exception that is propagating") {
    Closeable resource{Closeable::Failure::StdException};

    std::string message;
    try {
        AutoCloser closer{resource};
        throw std::logic_error{"original"};
    }
    catch (const std::logic_error& error) {
        message = error.what();
    }
    EXPECT_EQUAL(message, std::string{"original"});
    EXPECT_EQUAL(resource.closeCalls(), 1);
}

CASE("AutoCloser: a failed close and an exception from the scope may be of any type") {
    Closeable nonStdFailing{Closeable::Failure::NonStdException};
    int thrown = 0;
    try {
        AutoCloser closer{nonStdFailing};
        throw std::logic_error{"original"};
    }
    catch (const std::logic_error&) {
        thrown = -1;
    }
    EXPECT_EQUAL(thrown, -1);

    Closeable stdFailing{Closeable::Failure::StdException};
    try {
        AutoCloser closer{stdFailing};
        throw 42;
    }
    catch (int value) {
        thrown = value;
    }
    EXPECT_EQUAL(thrown, 42);
}

CASE("AutoCloser: a successful close leaves the exception that is propagating untouched") {
    Closeable resource;
    EXPECT_THROWS_AS(([&] {
                         AutoCloser closer{resource};
                         throw std::logic_error{"original"};
                     })(),
                     std::logic_error);
    EXPECT_EQUAL(resource.closeCalls(), 1);
}

CASE("AutoCloser: a failed close inside a catch block reaches the caller") {
    Closeable resource{Closeable::Failure::StdException};

    try {
        throw SeriousBug{"caught"};
    }
    catch (const Exception&) {
        EXPECT_THROWS_AS(([&] { AutoCloser closer{resource}; })(), std::runtime_error);
    }
    EXPECT_EQUAL(resource.closeCalls(), 1);
}

CASE("AutoCloser: works on a const object") {
    const ConstCloseable resource;
    { AutoCloser closer{resource}; }
    EXPECT_EQUAL(resource.closeCalls(), 1);
}

CASE("AutoCloser: closes a DataHandle") {
    MemoryHandle handle{64};
    handle.openForWrite(Length{64});
    {
        AutoCloser closer{handle};
        EXPECT_EQUAL(handle.write("abc", 3), long{3});
    }
    EXPECT_THROWS_AS(handle.seek(0), AssertionFailed);  // not open any more
}

CASE("AutoCloser: a failed close of a DataHandle reaches the caller, and the handle is released") {
    HandleFailingOnClose handle;
    handle.openForWrite(Length{64});
    EXPECT_THROWS_AS(([&] { AutoCloser closer{handle}; })(), WriteError);
    EXPECT_THROWS_AS(handle.seek(0), AssertionFailed);

    HandleFailingOnClose another;
    another.openForWrite(Length{64});
    std::string message;
    try {
        AutoCloser closer{another};
        throw std::logic_error{"original"};
    }
    catch (const std::logic_error& error) {
        message = error.what();
    }
    EXPECT_EQUAL(message, std::string{"original"});
}

CASE("AutoCloser: a guard created in a destructor run by unwinding does not throw") {
    Closeable resource{Closeable::Failure::StdException};
    std::string message;
    try {
        GuardInDestructor guard{resource};
        throw std::logic_error{"original"};
    }
    catch (const std::logic_error& error) {
        message = error.what();
    }
    EXPECT_EQUAL(message, std::string{"original"});
    EXPECT_EQUAL(resource.closeCalls(), 1);
}

CASE("AutoCloser: a failed close is logged while an exception propagates, and not on a normal exit") {
    Closeable resource{Closeable::Failure::StdException};
    std::string logged;

    Log::error().setCallback(&appendToString, &logged);
    try {
        AutoCloser closer{resource};
        throw std::logic_error{"original"};
    }
    catch (const std::logic_error&) {
    }
    const std::string duringUnwinding = logged;

    logged.clear();
    try {
        AutoCloser closer{resource};
    }
    catch (const std::runtime_error&) {
    }
    const std::string onNormalExit = logged;
    Log::error().reset();

    EXPECT(duringUnwinding.find("ignored: close failed") != std::string::npos);
    EXPECT(onNormalExit.empty());
}

CASE("AutoCloser: a failed close reaches the caller after the thread's exception was released on another thread") {
    std::exception_ptr held;
    std::atomic<int> stage{0};
    bool reported = false;

    std::thread worker{[&] {
        try {
            throw SeriousBug{"transported"};
        }
        catch (...) {
            held = std::current_exception();
        }
        stage.store(1, std::memory_order_release);
        while (stage.load(std::memory_order_acquire) != 2) {
            std::this_thread::yield();
        }

        Closeable resource{Closeable::Failure::StdException};
        try {
            AutoCloser closer{resource};
        }
        catch (const std::runtime_error&) {
            reported = true;
        }
    }};

    while (stage.load(std::memory_order_acquire) != 1) {
        std::this_thread::yield();
    }
    held = nullptr;  // the last reference is released on another thread
    stage.store(2, std::memory_order_release);
    worker.join();

    EXPECT(reported);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
