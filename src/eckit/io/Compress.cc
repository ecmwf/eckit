/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <iomanip>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/BitIO.h"
#include "eckit/io/Compress.h"
#include "eckit/io/DataHandle.h"

// This code is written for readibility, not speed
// See https://users.cs.cf.ac.uk/Dave.Marshall/Multimedia/node214.html
// for a description of the algorythm
// and https://www.cs.duke.edu/csed/curious/compression/lzw.html


namespace eckit {

namespace {

inline static size_t MAX_CODE(size_t nbits) {
    return (1 << nbits) - 1;
}

enum
{
    RESET_TABLE = 256,
    END_MARKER  = 257,
    FIRST_CODE  = 258,

    MIN_BITS = 9,

};

/* UNUSED
static void print_code(std::ostream& out, size_t s) {
    switch (s) {
        case RESET_TABLE:
            out << "(reset)";
            break;
        case END_MARKER:
            out << "(end)";
            break;

        default:
            if (::isprint(s)) {
                out << char(s);
            }
            else {
                if (s < 256) {
                    out << std::hex << std::setfill('0') << std::setw(2) << s << std::dec
                        << std::setfill(' ');
                }
                else {
                    out << '(' << s << ')';
                }
            }
            break;
    }
}
*/

class Entry {
    std::vector<unsigned char> chars_;
    size_t code_;

public:
    Entry(size_t = END_MARKER);

    Entry operator+(unsigned char) const;
    Entry& operator=(unsigned char);
    bool empty() const;

    void code(size_t c) { code_ = c; }

    unsigned char firstChar() const {
        ASSERT(chars_.size());
        return chars_[0];
    }

    bool operator<(const Entry& other) const { return chars_ < other.chars_; }

    /* unused */
    //    friend std::ostream& operator<<(std::ostream& out, const Entry& e) {
    //        e.print(out);
    //        return out;
    //    }

    /* unused */
    //    void Entry::print(std::ostream& out) const {
    //        out << '[';
    //        print_code(out, code_);
    //        out << " -> ";
    //        for(size_t i = 0; i < chars_.size(); ++i) {
    //            print_code(out, chars_[i]);
    //        }
    //        out << ']';
    //    }

    void output(eckit::BitIO& out) const;
    void output(eckit::BitIO& out, size_t nbits) const;
};

Entry::Entry(size_t code) :
    code_(code) {
    if (code < 256) {
        chars_.push_back(code);
    }
}

Entry Entry::operator+(unsigned char c) const {
    Entry result = *this;
    result.chars_.push_back(c);
    return result;
}

Entry& Entry::operator=(unsigned char c) {
    chars_.clear();
    chars_.push_back(c);
    code_ = c;
    return *this;
}

void Entry::output(eckit::BitIO& out) const {
    // std::cout << "Output " << *this << std::endl;

    for (size_t i = 0; i < chars_.size(); ++i) {
        out.write(chars_[i], 8);
    }
}

bool Entry::empty() const {
    return chars_.empty();
}

void Entry::output(eckit::BitIO& out, size_t nbits) const {
    // std::cout << "Send "
    //           << *this
    //           << " (nbits="
    //           << nbits
    //           << ")"
    //           << std::endl;

    ASSERT(code_ <= MAX_CODE(nbits));
    out.write(code_, nbits);
}
//----------------------------------------------------------------------------------------------------------------------


static inline size_t next_byte(BitIO& in) {
    size_t byte = in.read(8, END_MARKER);
    // std::cout << "Read ";
    // print_code(std::cout, byte);
    // std::cout << std::endl;
    return byte;
}

static void init_table(std::set<Entry>& table) {
    table.clear();
    for (size_t c = 0; c < 256; ++c) {
        table.insert(Entry(c));
    }
}
}  // namespace

Compress::Compress(size_t maxBits) :
    maxBits_(maxBits) {}


size_t Compress::encode(DataHandle& in, DataHandle& out) {
    Entry eoi(END_MARKER);
    Entry reset(RESET_TABLE);

    std::set<Entry> code_table;
    init_table(code_table);

    BitIO bin(in);
    BitIO bout(out);

    size_t nbits     = MIN_BITS;
    size_t next_code = FIRST_CODE;
    size_t max_code  = MAX_CODE(nbits);

    reset.output(bout, nbits);

    Entry w;
    for (;;) {
        size_t k = next_byte(bin);

        if (k == END_MARKER) {
            break;
        }

        Entry wk = w + k;

        std::set<Entry>::iterator j = code_table.find(wk);

        // Sequence in table
        if (j != code_table.end()) {
            // '*j' is the same as 'wp'
            // but should contain a valid code
            w = *j;
        }
        else {
            w.output(bout, nbits);
            wk.code(next_code++);
            code_table.insert(wk);
            w = k;

            if (next_code >= max_code) {
                if (nbits == maxBits_) {
                    reset.output(bout, nbits);

                    nbits     = MIN_BITS;
                    max_code  = MAX_CODE(nbits);
                    next_code = FIRST_CODE;
                    init_table(code_table);
                }
                else {
                    nbits++;
                    max_code = MAX_CODE(nbits);
                }
            }
        }
    }

    if (!w.empty()) {
        w.output(bout, nbits);
    }

    eoi.output(bout, nbits);

    return bout.byteCount();
}
//----------------------------------------------------------------------------------------------------------------------


static void init_table(std::map<size_t, Entry>& table) {
    table.clear();
    for (size_t i = 0; i < 256; ++i) {
        table[i] = Entry(i);
    }
}

size_t Compress::decode(DataHandle& in, DataHandle& out) {
    std::map<size_t, Entry> table;
    init_table(table);

    BitIO bin(in);
    BitIO bout(out);

    size_t nbits = MIN_BITS;

    Entry w;

    size_t next_code = FIRST_CODE;
    size_t max_code  = MAX_CODE(nbits) - 1;

    for (;;) {
        if (next_code >= max_code) {
            nbits    = std::min(maxBits_, nbits + 1);
            max_code = MAX_CODE(nbits) - 1;
            // std::cout << "DECODE nbits " << nbits << std::endl;
        }

        size_t k = bin.read(nbits, END_MARKER);

        // std::cout << "Got code ";
        // print_code(std::cout, k);
        // std::cout << std::endl;

        if (k == END_MARKER) {
            break;
        }

        /* This should be the first code */
        if (k == RESET_TABLE) {
            // std::cout << "RESET_TABLE" << std::endl;

            nbits     = MIN_BITS;
            max_code  = MAX_CODE(nbits) - 1;
            next_code = FIRST_CODE;
            init_table(table);

            k = bin.read(nbits, END_MARKER);
            if (k == END_MARKER) {
                break;
            }

            // std::cout << "Got code ";
            // print_code(std::cout, k);
            // std::cout << std::endl;

            w = k;

            w.output(bout);

            continue;
        }

        Entry e;

        std::map<size_t, Entry>::iterator j = table.find(k);
        if (j != table.end()) {
            e = (*j).second;
        }
        else {
            ASSERT(k == next_code);
            e = w + w.firstChar();
        }

        e.output(bout);

        Entry n = w + e.firstChar();
        n.code(next_code);
        table[next_code] = n;
        next_code++;

        // std::cout << "Add code " << n << std::endl;

        w = e;
    }

    return bout.byteCount();
}
//----------------------------------------------------------------------------------------------------------------------
}  // namespace eckit
