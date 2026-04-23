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

#include <cstdlib>
#include <string>
#include <vector>

namespace eckit::testing {

//----------------------------------------------------------------------------------------------------------------------

/// Fork @p n children, each re-exec-ing the current binary as a fresh process.
///
/// This avoids inheriting in-process state (gRPC channels, libfabric contexts, etc.)
/// that is not fork-safe.  Each child gets a completely clean address space.
///
/// @p args  Extra arguments passed to each child on the command line.
///          "--worker-id=<0..n-1>" is prepended automatically.
///
/// The binary's main() should call parse_worker_args() to detect whether it was
/// launched as a child worker and retrieve the argument map.
///
/// Returns true if every child exited with status 0.
inline bool fork_and_exec(int n, const std::vector<std::string>& args = {}) {
    // Resolve path to current executable
    char exe[4096];
    const ssize_t len = ::readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (len <= 0) {
        return false;
    }
    exe[len] = '\0';

    std::vector<pid_t> pids;
    pids.reserve(n);

    for (int i = 0; i < n; ++i) {
        const pid_t pid = ::fork();
        if (pid < 0) {
            for (auto p : pids) {
                ::kill(p, SIGTERM);
                ::waitpid(p, nullptr, 0);
            }
            return false;
        }
        if (pid == 0) {
            // Build argv: exe --worker-id=i [extra args...]
            std::vector<std::string> storage;
            storage.reserve(2 + args.size());
            storage.push_back(exe);
            storage.push_back("--worker-id=" + std::to_string(i));
            for (const auto& a : args) {
                storage.push_back(a);
            }

            std::vector<char*> argv;
            argv.reserve(storage.size() + 1);
            for (auto& s : storage) {
                argv.push_back(s.data());
            }
            argv.push_back(nullptr);

            ::execv(exe, argv.data());
            ::_exit(127);  // exec failed
        }
        pids.push_back(pid);
    }

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

/// Parse argc/argv for child worker arguments set by fork_and_exec.
/// Returns a map of "--key=value" arguments if "--worker-id=N" is present,
/// or an empty map if this process was not launched as a child worker.
/// Keys are stored without the "--" prefix.
inline std::vector<std::pair<std::string, std::string>> parse_worker_args(int argc, char** argv) {
    std::vector<std::pair<std::string, std::string>> result;
    bool is_worker = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) != "--") {
            continue;
        }
        auto eq = arg.find('=', 2);
        if (eq == std::string::npos) {
            continue;
        }
        auto key   = arg.substr(2, eq - 2);
        auto value = arg.substr(eq + 1);
        if (key == "worker-id") {
            is_worker = true;
        }
        result.emplace_back(std::move(key), std::move(value));
    }

    if (!is_worker) {
        result.clear();
    }
    return result;
}

/// Convenience: extract a named argument from the child worker args map.
inline const std::string& get_worker_arg(const std::vector<std::pair<std::string, std::string>>& args,
                                           const std::string& key) {
    for (const auto& kv : args) {
        if (kv.first == key) {
            return kv.second;
        }
    }
    static const std::string empty;
    return empty;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::testing
