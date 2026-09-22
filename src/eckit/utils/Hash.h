// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_utils_Hash_H
#define eckit_utils_Hash_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>

#include "eckit/eckit.h"
#include "eckit/thread/Mutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Hash {

public:  // types

    using digest_t = std::string;

public:  // methods

    Hash();

    Hash(const Hash&)            = delete;
    Hash& operator=(const Hash&) = delete;
    Hash(Hash&&)                 = delete;
    Hash& operator=(Hash&&)      = delete;

    virtual ~Hash();

    virtual void reset() const = 0;

    virtual digest_t digest() const = 0;

    // for one shot, stateless computation of the hash of the buffer
    virtual digest_t compute(const void*, long) = 0;

    void add(char x) { update(&x, sizeof(x)); }
    void add(unsigned char x) { update(&x, sizeof(x)); }

    void add(bool x) { update(&x, sizeof(x)); }

    void add(int x) { update(&x, sizeof(x)); }
    void add(unsigned int x) { update(&x, sizeof(x)); }

    void add(short x) { update(&x, sizeof(x)); }
    void add(unsigned short x) { update(&x, sizeof(x)); }

    void add(long x) { update(&x, sizeof(x)); }
    void add(unsigned long x) { update(&x, sizeof(x)); }

    void add(long long x) { update(&x, sizeof(x)); }
    void add(unsigned long long x) { update(&x, sizeof(x)); }

    void add(float x) { update(&x, sizeof(x)); }
    void add(double x) { update(&x, sizeof(x)); }

    void add(const void* x, long size) { update(x, size); }

    void add(const std::string& x) { update(x.c_str(), x.size()); }
    void add(const char* x) { update(x, std::strlen(x)); }

    template <class T>
    Hash& operator<<(const T& x) {
        add(x);
        return *this;
    }

    operator std::string() { return digest(); }

protected:  // methods

    // for incremental hashing
    virtual void update(const void*, long) = 0;

private:  // types

    // Make sure this is not called with a pointer
    template <class T>
    void add(const T* x);
    void add(const void*);

    /// Double hashing
    void add(const Hash& hash) { add(hash.digest()); }

protected:  // members

    mutable digest_t digest_;  ///< cached digest
};

//----------------------------------------------------------------------------------------------------------------------

class HashBuilderBase {
    std::string name_;

public:

    HashBuilderBase(const std::string&);
    virtual ~HashBuilderBase();
    virtual Hash* make()                         = 0;
    virtual Hash* make(const std::string& param) = 0;
};

template <class T>
class HashBuilder : public HashBuilderBase {
    Hash* make() override { return new T(); }
    Hash* make(const std::string& param) override { return new T(param); }

public:

    HashBuilder(const std::string& name) : HashBuilderBase(name) {}
    ~HashBuilder() override = default;
};

class HashFactory {
public:

    static HashFactory& instance();

    void add(const std::string& name, HashBuilderBase* builder);
    void remove(const std::string& name);

    bool has(const std::string& name);
    void list(std::ostream&);

    /// @returns default hash function
    Hash* build();

    /**
     * @param name  hash function name
     * @returns     hash function built by specified builder
     */
    Hash* build(const std::string& name);

    /**
     * @param name  hash function name
     * @param param the initialization string, passed directly to the hash function constructor
     * @returns     hash function built by specified builder
     */
    Hash* build(const std::string& name, const std::string& param);

private:

    HashFactory();

    std::map<std::string, HashBuilderBase*> builders_;
    eckit::Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // end namespace eckit

#endif
