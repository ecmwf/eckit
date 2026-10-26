// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @file   FaultInjection.h
///
/// Helpers for tests of failure paths: run code in a forked child and report how it ended, count and close the
/// process's file descriptors, and detect a descriptor being closed twice.
///
/// Linux only: the descriptor helpers read /proc/self. Fork-based helpers must run before any other thread exists.

#pragma once

#if defined(__linux__)

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

namespace eckit::testing {

//----------------------------------------------------------------------------------------------------------------------

namespace detail {

inline std::string errnoMessage(const std::string& what) {
    return what + ": " + std::strerror(errno);
}

/// Calls @p visit with the name of every entry of @p dir, except "." and "..".
template <typename Visitor>
void forEachEntry(const char* dir, Visitor&& visit) {
    DIR* d = ::opendir(dir);
    if (d == nullptr) {
        throw std::runtime_error(errnoMessage(std::string("opendir ") + dir));
    }
    while (const dirent* e = ::readdir(d)) {
        if (std::strcmp(e->d_name, ".") != 0 && std::strcmp(e->d_name, "..") != 0) {
            visit(e->d_name);
        }
    }
    ::closedir(d);
}

}  // namespace detail

//----------------------------------------------------------------------------------------------------------------------

/// Number of threads in this process.
inline int threadCount() {
    int n = 0;
    detail::forEachEntry("/proc/self/task", [&](const char*) { ++n; });
    return n;
}

/// Number of file descriptors open in this process.
inline int openDescriptorCount() {
    int n = 0;
    detail::forEachEntry("/proc/self/fd", [&](const char*) { ++n; });
    return n - 1;  // the descriptor of the directory being read
}

/// Closes @p fd behind the back of the object that owns it, so that the owner's next close fails with EBADF.
inline void closeDescriptor(int fd) {
    if (::close(fd) != 0) {
        throw std::runtime_error(detail::errnoMessage("close " + std::to_string(fd)));
    }
}

/// Closes the only descriptor of this process that refers to @p path, and returns its number.
/// Throws if no descriptor or more than one refers to it.
inline int closeDescriptorOf(const std::string& path) {
    char resolved[PATH_MAX];
    if (::realpath(path.c_str(), resolved) == nullptr) {
        throw std::runtime_error(detail::errnoMessage("realpath " + path));
    }

    int found   = -1;
    int matches = 0;
    detail::forEachEntry("/proc/self/fd", [&](const char* name) {
        char target[PATH_MAX];
        const std::string link = std::string("/proc/self/fd/") + name;
        const ssize_t len      = ::readlink(link.c_str(), target, sizeof(target) - 1);
        if (len > 0) {
            target[len] = '\0';
            if (std::strcmp(target, resolved) == 0) {
                found = std::atoi(name);
                ++matches;
            }
        }
    });

    if (matches != 1) {
        throw std::runtime_error(std::to_string(matches) + " descriptors refer to " + path + ", expected 1");
    }
    closeDescriptor(found);
    return found;
}

//----------------------------------------------------------------------------------------------------------------------

/// Occupies a free descriptor number with a file of its own, to detect code that closes that number again.
///
/// Typical use: make an object's close() fail, call close() once, then create the sentinel on the descriptor the
/// object held, then call close() again. stillOpen() is false if the second call closed the descriptor.
/// The sentinel must be created after the first close(): before it, the number is still the object's.
class DescriptorSentinel {
public:

    explicit DescriptorSentinel(int fd) : fd_(fd) {
        if (::fcntl(fd_, F_GETFD) != -1) {
            throw std::runtime_error("descriptor " + std::to_string(fd_) + " is still open");
        }
        const int tmp = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        if (tmp < 0) {
            throw std::runtime_error(detail::errnoMessage("open /dev/null"));
        }
        if (tmp != fd_) {
            const int dup = ::dup2(tmp, fd_);
            ::close(tmp);
            if (dup != fd_) {
                throw std::runtime_error(detail::errnoMessage("dup2 to " + std::to_string(fd_)));
            }
        }
        struct stat st{};
        ::fstat(fd_, &st);
        dev_ = st.st_dev;
        ino_ = st.st_ino;
    }

    DescriptorSentinel(const DescriptorSentinel&)            = delete;
    DescriptorSentinel& operator=(const DescriptorSentinel&) = delete;

    ~DescriptorSentinel() {
        if (stillOpen()) {
            ::close(fd_);
        }
    }

    /// True if the descriptor is still open on the sentinel's own file.
    bool stillOpen() const {
        struct stat st{};
        return ::fstat(fd_, &st) == 0 && st.st_dev == dev_ && st.st_ino == ino_;
    }

    int fd() const { return fd_; }

private:

    int fd_;
    dev_t dev_{};
    ino_t ino_{};
};

//----------------------------------------------------------------------------------------------------------------------

/// True if /dev/full can be opened for writing. A buffered write to it fails with ENOSPC when flushed.
inline bool devFullAvailable() {
    return ::access("/dev/full", W_OK) == 0;
}

//----------------------------------------------------------------------------------------------------------------------

/// How a child started by runInChild() ended.
enum class ChildOutcome {
    Completed,   ///< the function returned
    Threw,       ///< an exception left the function
    Terminated,  ///< std::terminate was called
    Aborted,     ///< the child was killed by SIGABRT (std::abort)
    Other,       ///< any other exit status or signal
};

inline std::ostream& operator<<(std::ostream& out, ChildOutcome outcome) {
    switch (outcome) {
        case ChildOutcome::Completed:
            return out << "Completed";
        case ChildOutcome::Threw:
            return out << "Threw";
        case ChildOutcome::Terminated:
            return out << "Terminated";
        case ChildOutcome::Aborted:
            return out << "Aborted";
        case ChildOutcome::Other:
            return out << "Other";
    }
    return out;
}

struct ChildResult {
    ChildOutcome outcome;
    int status;          ///< raw status from waitpid
    std::string output;  ///< what the child wrote to stderr
};

namespace detail {

constexpr int terminatedExitCode = 86;
constexpr int threwExitCode      = 87;

[[noreturn]] inline void exitTerminated() {
    ::_exit(terminatedExitCode);
}

}  // namespace detail

/// Runs @p fn in a forked child and reports how it ended, with the child's stderr.
///
/// The child ends with _exit, so no static destructor and no leak check runs in it: check leaks in the parent.
/// Throws std::logic_error if the process has more than one thread, as fork() copies only the calling thread.
inline ChildResult runInChild(const std::function<void()>& fn) {
    if (const int threads = threadCount(); threads != 1) {
        throw std::logic_error("runInChild needs a single-threaded process, found " + std::to_string(threads) +
                               " threads");
    }

    int fds[2];
    if (::pipe(fds) != 0) {
        throw std::runtime_error(detail::errnoMessage("pipe"));
    }

    std::cout.flush();
    std::cerr.flush();
    std::fflush(nullptr);

    const pid_t pid = ::fork();
    if (pid < 0) {
        ::close(fds[0]);
        ::close(fds[1]);
        throw std::runtime_error(detail::errnoMessage("fork"));
    }

    if (pid == 0) {
        ::close(fds[0]);
        ::dup2(fds[1], STDERR_FILENO);
        ::close(fds[1]);
        std::set_terminate(&detail::exitTerminated);
        try {
            fn();
        }
        catch (...) {
            std::cerr.flush();
            ::_exit(detail::threwExitCode);
        }
        std::cerr.flush();
        ::_exit(0);
    }

    ::close(fds[1]);
    std::string output;
    char buffer[4096];
    for (;;) {
        const ssize_t n = ::read(fds[0], buffer, sizeof(buffer));
        if (n > 0) {
            output.append(buffer, static_cast<size_t>(n));
        }
        else if (n == 0 || errno != EINTR) {
            break;
        }
    }
    ::close(fds[0]);

    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR) {
            throw std::runtime_error(detail::errnoMessage("waitpid"));
        }
    }

    ChildOutcome outcome = ChildOutcome::Other;
    if (WIFEXITED(status)) {
        switch (WEXITSTATUS(status)) {
            case 0:
                outcome = ChildOutcome::Completed;
                break;
            case detail::terminatedExitCode:
                outcome = ChildOutcome::Terminated;
                break;
            case detail::threwExitCode:
                outcome = ChildOutcome::Threw;
                break;
            default:
                break;
        }
    }
    else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT) {
        outcome = ChildOutcome::Aborted;
    }

    return {outcome, status, output};
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::testing

#endif  // __linux__
