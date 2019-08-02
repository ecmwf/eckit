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
class ResizableBuffer;

//----------------------------------------------------------------------------------------------------------------------

class Compressor : private eckit::NonCopyable {

public:  // methods
    Compressor();

    virtual ~Compressor();

    virtual size_t compress(const eckit::Buffer& in, eckit::ResizableBuffer& out) const   = 0;
    virtual size_t uncompress(const eckit::Buffer& in, eckit::ResizableBuffer& out) const = 0;

protected:  // methods
};

//----------------------------------------------------------------------------------------------------------------------

class NoCompressor : public Compressor {

public:  // types
    NoCompressor();

    virtual ~NoCompressor() = default;

    virtual size_t compress(const eckit::Buffer& in, eckit::ResizableBuffer& out) const;
    virtual size_t uncompress(const eckit::Buffer& in, eckit::ResizableBuffer& out) const;
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
    virtual Compressor* make() { return new T(); }

public:
    CompressorBuilder(const std::string& name) : CompressorBuilderBase(name) {}
    virtual ~CompressorBuilder() = default;
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
