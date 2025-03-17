/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_utils_Compressor_H
#define eckit_utils_Compressor_H

#include <map>
#include <string>

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"

namespace eckit {

class Buffer;

//----------------------------------------------------------------------------------------------------------------------

class Compressor : private eckit::NonCopyable {

public:  // methods

    Compressor();

    virtual ~Compressor();

    /// Compresses the bytestream within the in buffer
    /// @param in input buffer that holds the uncompressed bytesteam.
    /// @param len input buffer size.
    /// @param out output buffer to hold the compressed bytesteam. Buffer may be oversized, in which case
    ///            resizing is implementation specific. Nevertheless, it is expected that implementations
    ///            will take this as oppurtunity for optimising and avoid resizing.
    /// @note if needed, it will resize or replace the internal buffer of out to match the compressed bytestream size
    /// @returns the size of the compressed bytestream inside out buffer, it is less or equal than out.size()
    virtual size_t compress(const void* in, size_t len, eckit::Buffer& out) const = 0;

    /// Uncompresses the bytestream within the in buffer
    /// Since some compression algorithms dont record uncompressed size in the bytestream, client code is
    /// required to record and handle the uncompressed size separately and pass it in via parameter outlen.
    /// @param in input buffer that holds the compressed bytesteam.
    /// @param len input buffer size.
    /// @param out output buffer to hold the uncompressed bytesteam.
    /// @param outlen the expected size of the uncompressed bytestream. Buffer may be resized or if large enough
    ///        implementations will try to used what is passed in. This could be a way to reuse the same memory
    ///        buffer in a tight loop.
    /// @note may resize or replace the internal buffer of out
    virtual void uncompress(const void* in, size_t len, eckit::Buffer& out, size_t outlen) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------

class NoCompressor : public Compressor {

public:  // types

    NoCompressor();

    ~NoCompressor() override = default;

    size_t compress(const void* in, size_t len, eckit::Buffer& out) const override;
    void uncompress(const void* in, size_t len, eckit::Buffer& out, size_t outlen) const override;
};

//----------------------------------------------------------------------------------------------------------------------

class CompressorBuilderBase {
    std::string name_;

public:

    CompressorBuilderBase(const std::string&);
    virtual ~CompressorBuilderBase();
    virtual Compressor* make() = 0;
};

template <class T>
class CompressorBuilder : public CompressorBuilderBase {
    Compressor* make() override { return new T(); }

public:

    CompressorBuilder(const std::string& name) : CompressorBuilderBase(name) {}
    ~CompressorBuilder() override = default;
};

class CompressorFactory {
public:

    static CompressorFactory& instance();

    void add(const std::string& name, CompressorBuilderBase* builder);
    void remove(const std::string& name);

    bool has(const std::string& name);
    void list(std::ostream&);

    /// @returns default compressor
    Compressor* build();

    /**
     * @param name  compressor name
     * @returns     compressor built by specified builder
     */
    Compressor* build(const std::string&);

private:

    CompressorFactory();

    std::map<std::string, CompressorBuilderBase*> builders_;
    eckit::Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
