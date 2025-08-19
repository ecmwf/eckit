/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @date May 96

#ifndef eckit_Stream_h
#define eckit_Stream_h

#include <map>
#include <string>

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"


namespace eckit {


template <class T>
class BufferedWriter;
template <class T>
class BufferedReader;
template <class T>
class IOBuffer;
class Buffer;

class Stream : private NonCopyable {
public:

    virtual ~Stream();

    // Output

    Stream& operator<<(char);
    Stream& operator<<(unsigned char);

    Stream& operator<<(bool);

    Stream& operator<<(int);
    Stream& operator<<(unsigned int);

    Stream& operator<<(short);
    Stream& operator<<(unsigned short);

    Stream& operator<<(long);
    Stream& operator<<(unsigned long);

    Stream& operator<<(long long);
    Stream& operator<<(unsigned long long);

    Stream& operator<<(float);
    Stream& operator<<(double);

    Stream& operator<<(const std::string&);
    Stream& operator<<(const char*);


    Stream& operator<<(const std::exception&);

    // Blobs
    Stream& operator<<(const Buffer&);

    // Input

    Stream& operator>>(char&);
    Stream& operator>>(unsigned char&);

    Stream& operator>>(int&);
    Stream& operator>>(unsigned int&);

    Stream& operator>>(bool&);

    Stream& operator>>(long&);
    Stream& operator>>(unsigned long&);

    Stream& operator>>(long long&);
    Stream& operator>>(unsigned long long&);

    Stream& operator>>(short&);
    Stream& operator>>(unsigned short&);

    Stream& operator>>(float&);
    Stream& operator>>(double&);

    Stream& operator>>(std::string&);

    // Blobs
    Stream& operator>>(Buffer&);

    Stream& operator>>(std::map<std::string, std::string>&);

    // -- Methods

    bool next(std::string&);
    bool next(bool&);
    bool next(int&);

    bool endObjectFound();
    bool next();
    void skipEndObject();
    void startObject();
    void endObject();

    void writeBlob(const void*, size_t);
    void readBlob(void*, size_t);

    void writeLargeBlob(const void*, size_t);
    void readLargeBlob(void*, size_t);

    virtual void rewind();
    virtual void closeOutput();
    virtual void closeInput();

    long long bytesWritten() { return writeCount_; }
    void resetBytesWritten() { writeCount_ = 0; }

    void startRecord(unsigned long);
    void endRecord();
    bool nextRecord(unsigned long&, bool sync = false);

    void lock() { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }


    static void dump(std::ostream&, const char*, size_t);


protected:

    // -- Contructors

    Stream();

    // -- Methods

    virtual std::string name() const = 0;
    virtual void print(std::ostream& s) const;

    size_t blobSize();

private:

    enum tag {
        tag_zero,
        tag_start_obj,
        tag_end_obj,
        tag_char,
        tag_unsigned_char,
        tag_int,
        tag_unsigned_int,
        tag_short,
        tag_unsigned_short,
        tag_long,
        tag_unsigned_long,
        tag_long_long,
        tag_unsigned_long_long,
        tag_float,
        tag_double,
        tag_string,
        tag_blob,
        tag_exception,
        tag_start_rec,
        tag_end_rec,
        tag_eof,
        tag_large_blob,  // For blobs >= 2Gb
        last_tag
    };

    // -- Members

    tag lastTag_;
    Mutex mutex_;
    long writeCount_;

    // -- Methods

    // These are the two methods to override

    virtual long write(const void*, long) = 0;
    virtual long read(void*, long)        = 0;

    unsigned char getChar();
    unsigned long getLong();

    void putChar(unsigned char);
    void putLong(unsigned long);

    void badTag(tag, tag);
    tag nextTag();
    tag readTag(tag = tag_zero);
    void writeTag(tag);

    void getBytes(void*, long);
    void putBytes(const void*, long);

    friend std::ostream& operator<<(std::ostream&, tag);

    friend class BufferedWriter<Stream>;
    friend class BufferedReader<Stream>;
    friend class IOBuffer<Stream>;

    friend std::ostream& operator<<(std::ostream& out, const Stream& s);
};


}  // namespace eckit

#endif
