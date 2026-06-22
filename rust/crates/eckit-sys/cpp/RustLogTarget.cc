// eckit log bridge — implementation.

// trycatch handler — must come before the cxx-generated header so the
// generated wrappers' Result<T> handling picks up our specialization.
#include "eckit_exceptions.h"

#include "RustLogTarget.h"
#include "eckit-sys/src/lib.rs.h"  // cxx-generated — provides LogLevel values

#include "eckit/system/Library.h"
#include "eckit/system/LibraryManager.h"

#ifdef __APPLE__
#include <cstdlib>  // getprogname
#elif defined(__linux__)
extern "C" char* program_invocation_short_name;
#endif

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

static const char* progname() {
#ifdef __APPLE__
    return getprogname();
#elif defined(__linux__)
    return program_invocation_short_name;
#else
    return nullptr;
#endif
}

//----------------------------------------------------------------------------------------------------------------------

void RustLogTarget::write(const char* start, const char* end) {
    buffer_.append(start, end);

    std::string::size_type pos;
    while ((pos = buffer_.find('\n')) != std::string::npos) {
        std::string line = buffer_.substr(0, pos);
        while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) {
            line.pop_back();
        }
        if (!line.empty()) {
            rust_log(level_, rust::Str(target_.data(), target_.size()), rust::Str(line.data(), line.size()));
        }
        buffer_.erase(0, pos + 1);
    }
}

void RustLogTarget::flush() {
    if (!buffer_.empty()) {
        while (!buffer_.empty() && (buffer_.back() == '\r' || buffer_.back() == ' ')) {
            buffer_.pop_back();
        }
        if (!buffer_.empty()) {
            rust_log(level_, rust::Str(target_.data(), target_.size()), rust::Str(buffer_.data(), buffer_.size()));
            buffer_.clear();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

RustMain::RustMain(int argc, char** argv) : Main(argc, argv), app_name_(displayName()) {}

eckit::LogTarget* RustMain::createInfoLogTarget() const {
    return new RustLogTarget(LogLevel::Info, app_name_);
}
eckit::LogTarget* RustMain::createWarningLogTarget() const {
    return new RustLogTarget(LogLevel::Warn, app_name_);
}
eckit::LogTarget* RustMain::createErrorLogTarget() const {
    return new RustLogTarget(LogLevel::Error, app_name_);
}
eckit::LogTarget* RustMain::createDebugLogTarget() const {
    return new RustLogTarget(LogLevel::Debug, app_name_);
}
eckit::LogTarget* RustMain::createMetricsLogTarget() const {
    return new RustLogTarget(LogLevel::Trace, app_name_);
}

/// Install a per-library `RustLogTarget` on every registered library's debug
/// channel. Each library's debug output is then tagged with its own name as
/// the tracing/log target. Idempotent — `Channel::setTarget` replaces.
static void install_per_library_targets() {
    for (const auto& libname : eckit::system::LibraryManager::list()) {
        const auto& lib = eckit::system::LibraryManager::lookup(libname);
        lib.debugChannel().setTarget(new RustLogTarget(LogLevel::Debug, libname));
    }
}

//----------------------------------------------------------------------------------------------------------------------

void init() {
    if (!eckit::Main::ready()) {
        const char* name                         = progname();
        static const char* argv[]                = {name ? name : "eckit-rs", nullptr};
        [[maybe_unused]] static auto* main_inst_ = new RustMain(1, const_cast<char**>(argv));
        install_per_library_targets();
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
