/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <algorithm>
#include <cctype>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "eckit/log/Log.h"
#include "eckit/option/CmdArgs.h"
#include "eckit/option/EckitTool.h"
#include "eckit/option/SimpleOption.h"
#include "eckit/runtime/Monitor.h"


namespace eckit::tools {

namespace {

struct TaskEntry {
    unsigned long slot;
    const TaskInfo* task;
};

std::string fixedString(const char* data, size_t size) {
    size_t len = 0;
    while (len < size && data[len] != '\0') {
        ++len;
    }
    return std::string(data, len);
}

void appendSortKey(const Monitor::TaskArray& tasks, unsigned long slot, std::vector<std::string>& key,
                   std::vector<unsigned long>& seen) {
    if (slot >= tasks.cend() - tasks.cbegin() || std::find(seen.begin(), seen.end(), slot) != seen.end()) {
        return;
    }

    seen.push_back(slot);

    const Info& info = tasks.cbegin()[slot].rawInfo();
    if (info.parent_ >= 0) {
        appendSortKey(tasks, static_cast<unsigned long>(info.parent_), key, seen);
    }

    key.push_back(fixedString(info.name_, sizeof(info.name_)));
    key.push_back(std::to_string(slot));
}

std::vector<std::string> sortKey(const Monitor::TaskArray& tasks, unsigned long slot) {
    std::vector<std::string> key;
    std::vector<unsigned long> seen;
    appendSortKey(tasks, slot, key, seen);
    return key;
}

std::string idleString(time_t last) {
    long age = ::time(nullptr) - last;
    if (age < 0) {
        age = 0;
    }

    long s = age % 60;
    age /= 60;
    long m = age % 60;
    age /= 60;
    long h = age % 24;
    age /= 24;
    long d = age;

    std::ostringstream out;
    if (d) {
        out << std::setw(d > 1 ? 2 : 3) << d << "day" << (d > 1 ? "s" : "");
    }
    else if (h || m) {
        out << std::setw(3) << h << ':' << std::setw(2) << std::setfill('0') << m << std::setfill(' ');
    }
    else {
        out << std::setw(6) << s;
    }
    return out.str();
}

std::string threadString(const pthread_t& thread);

void dumpPsHeader(std::ostream& out) {
    out << "name              Idle        Pid             Thread   Task       ID   Info      Request" << std::endl;
    out << "-------------------------------------------------------------------------------------" << std::endl;
}

void dumpPsTask(std::ostream& out, unsigned long slot, const TaskInfo& task) {
    const Info& info = task.rawInfo();

    for (long i = 0; i < info.depth_; i++) {
        out << "   ";
    }

    const std::string app = fixedString(info.name_, sizeof(info.name_));
    out << app;

    const long n = info.depth_ * 3 + static_cast<long>(app.length());
    for (long i = 0; i < 16 - n; i++) {
        out << ' ';
    }

    out << idleString(info.last_);
    out << ' ' << info.state_;
    out << std::setw(9) << info.pid_;
    out << std::setw(19) << threadString(info.thread_);
    out << "   " << std::setfill('0') << std::setw(3) << slot << std::setfill(' ');
    out << std::setw(11) << info.taskID_;
    out << std::setw(22) << fixedString(info.message_, sizeof(info.message_));
    out << "  " << fixedString(info.status_, sizeof(info.status_));

    if (info.progress_.max_ != info.progress_.min_) {
        const double x = (static_cast<double>(info.progress_.val_) - static_cast<double>(info.progress_.min_)) * 100.0 /
                         (static_cast<double>(info.progress_.max_) - static_cast<double>(info.progress_.min_));
        out << ' ' << x << '%';
    }

    out << std::endl;
}

std::string timeString(time_t value) {
    if (value == 0) {
        return "0";
    }

    struct tm tm;
    char formatted[64] = {};
    if (::localtime_r(&value, &tm) && ::strftime(formatted, sizeof(formatted), "%Y-%m-%d %H:%M:%S %z", &tm)) {
        std::ostringstream out;
        out << formatted << " (" << value << ')';
        return out.str();
    }

    return std::to_string(value);
}

std::string timeValueString(const timeval& value) {
    std::ostringstream out;
    out << value.tv_sec << '.' << std::setw(6) << std::setfill('0') << value.tv_usec;
    return out.str();
}

std::string stateString(char state) {
    if (state == '\0') {
        return "\\0";
    }

    if (std::isprint(static_cast<unsigned char>(state))) {
        return std::string(1, state);
    }

    std::ostringstream out;
    out << "0x" << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<unsigned int>(static_cast<unsigned char>(state));
    return out.str();
}

std::string threadString(const pthread_t& thread) {
    unsigned long long value = 0;
    std::memcpy(&value, &thread, std::min(sizeof(value), sizeof(thread)));

    std::ostringstream out;
    out << "0x" << std::hex << value;
    return out.str();
}

std::string escapedText(const char* data, size_t size) {
    std::ostringstream out;
    for (size_t i = 0; i < size; ++i) {
        const unsigned char c = static_cast<unsigned char>(data[i]);
        if (c == '\n' || c == '\r' || c == '\t' || std::isprint(c)) {
            out << data[i];
        }
        else {
            out << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(c)
                << std::dec << std::setfill(' ');
        }
    }
    return out.str();
}

void dumpTask(std::ostream& out, unsigned long slot, const TaskInfo& task) {
    const Info& info = task.rawInfo();

    out << "slot: " << slot << '\n';
    out << "  busy: " << std::boolalpha << info.busy_ << '\n';
    out << "  show: " << info.show_ << '\n';
    out << "  pid: " << info.pid_ << '\n';
    out << "  thread: " << threadString(info.thread_) << '\n';
    out << "  taskID: " << info.taskID_ << '\n';
    out << "  parent: " << info.parent_ << '\n';
    out << "  depth: " << info.depth_ << '\n';
    out << "  state: " << stateString(info.state_) << '\n';
    out << "  start: " << timeString(info.start_) << '\n';
    out << "  last: " << timeString(info.last_) << '\n';
    out << "  check: " << timeString(info.check_) << '\n';
    out << "  late: " << info.late_ << '\n';
    out << "  application: " << fixedString(info.application_, sizeof(info.application_)) << '\n';
    out << "  name: " << fixedString(info.name_, sizeof(info.name_)) << '\n';
    out << "  kind: " << fixedString(info.kind_, sizeof(info.kind_)) << '\n';
    out << "  status: " << fixedString(info.status_, sizeof(info.status_)) << '\n';
    out << "  message: " << fixedString(info.message_, sizeof(info.message_)) << '\n';
    out << "  host: " << fixedString(info.host_, sizeof(info.host_)) << '\n';
    out << "  port: " << info.port_ << '\n';
    out << "  flags:"
        << " stop=" << info.stop_ << " abort=" << info.abort_ << " stoppable=" << info.stoppable_
        << " stopped=" << info.stopped_ << " canceled=" << info.canceled_ << " exception=" << info.exception_
        << '\n';
    out << "  cancelMsg: " << fixedString(info.cancelMsg_, sizeof(info.cancelMsg_)) << '\n';
    out << "  config: " << info.config_ << '\n';
    out << "  resource: " << stateString(info.resource_) << '\n';
    out << "  progress:" << '\n';
    out << "    name: " << fixedString(info.progress_.name_, sizeof(info.progress_.name_)) << '\n';
    out << "    min: " << info.progress_.min_ << '\n';
    out << "    max: " << info.progress_.max_ << '\n';
    out << "    val: " << info.progress_.val_ << '\n';
    if (info.progress_.max_ > info.progress_.min_) {
        const double fraction = (static_cast<double>(info.progress_.val_) - static_cast<double>(info.progress_.min_)) /
                                (static_cast<double>(info.progress_.max_) - static_cast<double>(info.progress_.min_));
        out << "    percent: " << (100.0 * fraction) << '\n';
    }
    out << "    rate: " << info.progress_.rate_ << '\n';
    out << "    speed: " << info.progress_.speed_ << '\n';
    out << "    start: " << timeValueString(info.progress_.start_) << '\n';
    out << "    last: " << timeValueString(info.progress_.last_) << '\n';

    std::vector<char> log(Info::size_);
    unsigned long pos = 0;
    const unsigned long length = task.text(log.data(), log.size(), pos);
    out << "  log:" << '\n';
    if (length == 0) {
        out << "    <empty>" << '\n';
    }
    else {
        out << escapedText(log.data(), length);
        if (log[length - 1] != '\n') {
            out << '\n';
        }
    }
}

}  // namespace

class MonitorDump : public EckitTool {
public:

    MonitorDump(int argc, char** argv) : EckitTool(argc, argv) {
        options_.push_back(new option::SimpleOption<bool>("all", "Dump all slots, including empty slots"));
        options_.push_back(new option::SimpleOption<bool>("verbose", "Dump detailed per-slot information"));
    }

private:

    void execute(const option::CmdArgs& args) override {
        bool all = false;
        args.get("all", all);
        bool verbose = false;
        args.get("verbose", verbose);

        PathName path(args(0));
        std::unique_ptr<Monitor::TaskArray> tasks = Monitor::openReadOnlyTasks(path);

        std::vector<TaskEntry> entries;
        for (unsigned long i = 0; i < tasks->size(); ++i) {
            const Info& info = (*tasks)[i].rawInfo();
            if (all || (info.busy_ && (verbose || info.show_))) {
                entries.push_back(TaskEntry{i, &(*tasks)[i]});
            }
        }

        std::sort(entries.begin(), entries.end(), [&](const TaskEntry& a, const TaskEntry& b) {
            return sortKey(*tasks, a.slot) < sortKey(*tasks, b.slot);
        });

        if (verbose) {
            size_t busy = 0;
            for (unsigned long i = 0; i < tasks->size(); ++i) {
                if ((*tasks)[i].rawInfo().busy_) {
                    ++busy;
                }
            }

            std::cout << "monitor: " << path << '\n';
            std::cout << "slots: " << tasks->size() << '\n';

            for (const TaskEntry& entry : entries) {
                std::cout << '\n';
                dumpTask(std::cout, entry.slot, *entry.task);
            }

            std::cout << '\n' << "summary: busy=" << busy << " shown=" << entries.size() << " total=" << tasks->size()
                      << '\n';
        }
        else {
            dumpPsHeader(std::cout);
            for (const TaskEntry& entry : entries) {
                dumpPsTask(std::cout, entry.slot, *entry.task);
            }
        }
    }

    void usage(const std::string& tool) const override {
        Log::info() << std::endl << "Usage: " << tool << " [--verbose] [--all] <monitor-file>" << std::endl;
        EckitTool::usage(tool);
    }

    int numberOfPositionalArguments() const override { return 1; }
    int minimumPositionalArguments() const override { return 1; }
};

}  // namespace eckit::tools


int main(int argc, char** argv) {
    eckit::tools::MonitorDump app(argc, argv);
    return app.start();
}
