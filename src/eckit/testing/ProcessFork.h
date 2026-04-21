/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file   ProcessFork.h
/// @author Metin Cakircali
/// @date   Apr 2025

#pragma once

#include <sys/wait.h>
#include <unistd.h>

#include <exception>
#include <vector>

namespace eckit::testing {

//----------------------------------------------------------------------------------------------------------------------

/// Fork @p n child processes. Each child waits on a barrier, then runs @p fn(child_index).
/// Parent waits for all children to exit.
/// Returns true if every child exited with status 0.
template <typename Fn>
bool fork_and_run(int n, Fn&& fn) {
    int barrier[2];
    if (::pipe(barrier) != 0) {
        return false;
    }

    std::vector<pid_t> pids;
    pids.reserve(n);

    for (int i = 0; i < n; ++i) {
        const pid_t pid = ::fork();
        if (pid < 0) {
            for (auto p : pids) {
                ::kill(p, SIGTERM);
                ::waitpid(p, nullptr, 0);
            }
            ::close(barrier[0]);
            ::close(barrier[1]);
            return false;
        }
        if (pid == 0) {
            std::set_terminate([]() { ::_exit(1); });
            ::close(barrier[1]);
            char buf;
            static_cast<void>(::read(barrier[0], &buf, 1));
            ::close(barrier[0]);
            try {
                fn(i);
                ::_exit(0);
            }
            catch (...) {
                ::_exit(1);
            }
        }
        pids.push_back(pid);
    }

    ::close(barrier[0]);
    ::close(barrier[1]);

    bool all_ok = true;
    for (auto pid : pids) {
        int status = 0;
        if (::waitpid(pid, &status, 0) < 0) {
            all_ok = false;
        }
        else if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            all_ok = false;
        }
    }
    return all_ok;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::testing
