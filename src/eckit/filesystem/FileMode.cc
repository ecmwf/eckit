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
#include <string>
#include <type_traits>
#include <vector>

#include "eckit/filesystem/FileMode.h"
#include "eckit/filesystem/PathName.h"


namespace eckit {

namespace {

// If mode_t is a signed type
template <typename Mode, typename std::enable_if<std::is_signed<Mode>::value>::type* = nullptr>
bool invalidMode(Mode m) {
    return m < 0 || m > 0777;
}

// If mode_t is a unsigned type
template <typename Mode, typename std::enable_if<std::is_unsigned<Mode>::value>::type* = nullptr>
bool invalidMode(Mode m) {
    return m > 0777;
}

}  // namespace

FileMode::FileMode(mode_t m) :
    mode_(m) {
    if (invalidMode(m)) {
        std::ostringstream oss;
        oss << "FileMode: invalid mode 0" << std::setw(3) << std::setfill('0') << std::oct << m;
        throw BadValue(oss.str(), Here());
    }
}

FileMode::FileMode(const std::string& s) {
    mode_ = toMode(s);
}

FileMode& FileMode::operator=(const std::string& s) {
    mode_ = toMode(s);
    return *this;
}

FileMode& FileMode::operator=(mode_t m) {
    mode_ = m;
    return *this;
}

bool FileMode::operator==(const FileMode& other) const {
    return mode_ == other.mode_;
}

mode_t FileMode::mask(const FileMode& target) const {
    return target.mode_ & ~mode_;
}

void FileMode::testAssign(const std::string& s, char got, char test, mode_t& mode, mode_t mask) const {
    if (got == test)
        mode |= mask;
    else {
        if (got != '-') {
            std::ostringstream oss;
            oss << "Bad character " << got << " in string describing mode permissions " << s;
            throw BadValue(oss.str(), Here());
        }
    }
}

mode_t FileMode::toMode(const std::string& s) const {


    ASSERT(s.size() > 0);

    if (s[0] == '0') {

        mode_t m = 0;
        for (char c : s) {

            if (c >= '0' && c <= '7') {
                m *= 8;
                m += c - '0';
            }
            else {
                std::ostringstream oss;
                oss << "Bad string describing mode permissions (invalid octal number)" << s;
                throw BadValue(oss.str(), Here());
            }
        }

        if (invalidMode(m)) {
            std::ostringstream oss;
            oss << "FileMode: invalid mode 0" << std::setw(3) << std::setfill('0') << std::oct << m;
            throw BadValue(oss.str(), Here());
        }

        return m;
    }


    std::vector<std::string> t;
    eckit::Tokenizer(",")(s, t);

    if (t.size() != 3) {
        std::ostringstream oss;
        oss << "Bad string describing mode permissions " << s;
        throw BadValue(oss.str(), Here());
    }

    if (t[0].size() != 3 or t[1].size() != 3 or t[2].size() != 3) {
        std::ostringstream oss;
        oss << "Bad string describing mode permissions " << s;
        throw BadValue(oss.str(), Here());
    }

    mode_t mode = 0;
    testAssign(s, t[0][0], 'r', mode, S_IRUSR);
    testAssign(s, t[0][1], 'w', mode, S_IWUSR);
    testAssign(s, t[0][2], 'x', mode, S_IXUSR);

    testAssign(s, t[1][0], 'r', mode, S_IRGRP);
    testAssign(s, t[1][1], 'w', mode, S_IWGRP);
    testAssign(s, t[1][2], 'x', mode, S_IXGRP);

    testAssign(s, t[2][0], 'r', mode, S_IROTH);
    testAssign(s, t[2][1], 'w', mode, S_IWOTH);
    testAssign(s, t[2][2], 'x', mode, S_IXOTH);

    //        Log::info() << "s: " << s << " mode: " << mode << std::endl;

    return mode;
}

std::string FileMode::toString() const {

    std::string s("---,---,---");

    if (mode_ & S_IRUSR)
        s[0] = 'r';
    if (mode_ & S_IWUSR)
        s[1] = 'w';
    if (mode_ & S_IXUSR)
        s[2] = 'x';

    if (mode_ & S_IRGRP)
        s[4] = 'r';
    if (mode_ & S_IWGRP)
        s[5] = 'w';
    if (mode_ & S_IXGRP)
        s[6] = 'x';

    if (mode_ & S_IROTH)
        s[8] = 'r';
    if (mode_ & S_IWOTH)
        s[9] = 'w';
    if (mode_ & S_IXOTH)
        s[10] = 'x';

    return s;
}

FileMode FileMode::fromPath(const PathName& path) {
    struct stat s;
    SYSCALL(::stat(path.asString().c_str(), &s));
    return FileMode((s.st_mode & S_IRUSR)
                    | (s.st_mode & S_IWUSR)
                    | (s.st_mode & S_IXUSR) | (s.st_mode & S_IRGRP) | (s.st_mode & S_IWGRP) | (s.st_mode & S_IXGRP) | (s.st_mode & S_IROTH) | (s.st_mode & S_IWOTH) | (s.st_mode & S_IXOTH));
}

void FileMode::print(std::ostream& os) const {
    os << str();
}

}  // namespace eckit
