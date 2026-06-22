// eckit log bridge — routes `eckit::Log` output to Rust's `log` crate.
#pragma once

#include "eckit/log/LogTarget.h"
#include "eckit/runtime/Main.h"

#include "rust/cxx.h"

#include <cstdint>
#include <string>

namespace eckit_bridge {

//----------------------------------------------------------------------------------------------------------------------

// Forward declarations — full definitions in cxx-generated code
enum class LogLevel : std::uint8_t;
void rust_log(LogLevel level, rust::Str target, rust::Str msg) noexcept;

//----------------------------------------------------------------------------------------------------------------------

/// LogTarget that routes all output to Rust's log crate.
/// Accumulates writes until a newline or flush, then emits a single log call.
/// `target` is the tracing/log target string passed to Rust (e.g. "eckit",
/// "metkit") — global channels use "eckit", per-library debug channels use
/// the library name from `eckit::system::LibraryManager`.
class RustLogTarget : public eckit::LogTarget {
public:

    RustLogTarget(LogLevel level, std::string target) : level_(level), target_(std::move(target)) {}

    void write(const char* start, const char* end) override;
    void flush() override;

private:

    LogLevel level_;
    std::string target_;
    std::string buffer_;
};

//----------------------------------------------------------------------------------------------------------------------

/// Main subclass that installs `RustLogTarget` on all channels.
/// Every new thread automatically gets `RustLogTarget` via the factory methods.
class RustMain : public eckit::Main {
    std::string app_name_;

public:

    RustMain(int argc, char** argv);

    eckit::LogTarget* createInfoLogTarget() const override;
    eckit::LogTarget* createWarningLogTarget() const override;
    eckit::LogTarget* createErrorLogTarget() const override;
    eckit::LogTarget* createDebugLogTarget() const override;
    eckit::LogTarget* createMetricsLogTarget() const override;
};

//----------------------------------------------------------------------------------------------------------------------

/// Initialize eckit runtime with Rust log bridge.
void init();

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit_bridge
