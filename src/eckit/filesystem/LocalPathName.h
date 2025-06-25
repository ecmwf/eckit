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
/// @author Manuel Fuentes
/// @author Tiago Quintino
/// @date   May 1996


#ifndef eckit_filesystem_LocalPathName_h
#define eckit_filesystem_LocalPathName_h

#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/types/Types.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class Length;
class DataHandle;
class BasePathName;
class PathName;

struct FileSystemSize;

/// LocalPathName represents a UNIX path
//  It expanses '~' and tidies the path on construction and preserves trailing '/'

class LocalPathName {
public:  // methods

    friend void operator<<(Stream&, const LocalPathName&);
    friend void operator>>(Stream&, LocalPathName&);

    friend std::ostream& operator<<(std::ostream& s, const LocalPathName& p) { return s << p.path_; }

    LocalPathName(const char* p = "/", bool tildeIsUserHome = false, bool skipTildeExpansion = false) :
        path_(parsePath(p)) {
        if (!skipTildeExpansion) {
            tidy(tildeIsUserHome, skipTildeExpansion);
        }
    }
    LocalPathName(const std::string& p, bool tildeIsUserHome = false, bool skipTildeExpansion = false) :
        path_(parsePath(p)) {
        if (!skipTildeExpansion) {
            tidy(tildeIsUserHome, skipTildeExpansion);
        }
    }
    explicit LocalPathName(const PathName& path);
    LocalPathName(const LocalPathName& p) : path_(p.path_) {}

    // Assignment

    LocalPathName& operator=(const LocalPathName& p) {
        path_ = p.path_;
        return *this;
    }

    LocalPathName& operator=(const std::string& p) {
        path_ = parsePath(p);
        return tidy();
    }

    LocalPathName& operator=(const char* p) {
        path_ = parsePath(p);
        return tidy();
    }

    // Convertors

    operator const std::string&() const { return path_; }

    const char* localPath() const { return path_.c_str(); }

    const char* c_str() const { return path_.c_str(); }

    // Operators

    LocalPathName& operator+=(const std::string& s) {
        path_ += s;
        return tidy();
    }
    LocalPathName& operator+=(const char* s) {
        path_ += s;
        return tidy();
    }
    LocalPathName& operator+=(char s) {
        path_ += s;
        return tidy();
    }

    bool operator<(const LocalPathName& other) const { return path_ < other.path_; }

    bool operator>(const LocalPathName& other) const { return path_ > other.path_; }

    bool operator<=(const LocalPathName& other) const { return path_ <= other.path_; }

    bool operator>=(const LocalPathName& other) const { return path_ >= other.path_; }

    bool operator!=(const LocalPathName& other) const { return path_ != other.path_; }

    bool operator==(const LocalPathName& other) const { return path_ == other.path_; }

    // Methods

    static const char* type() { return "local"; }

    /// @returns a relative filepath to path to a start directory.
    /// This is a pure path computation, no filesystem is accessed to confirm the existence or nature of path
    /// @pre both paths need to be absolute paths, i.e starting with "/"
    LocalPathName relativePath(const LocalPathName& other) const;

    /// Directory part of the path
    /// @return directory part of the path
    LocalPathName dirName() const;

    /// Absolute path
    /// @return absolute path
    LocalPathName fullName() const;

    LocalPathName orphanName() const;

    BasePathName* checkClusterNode() const;

    std::string clusterName() const;

    /// Base name of the path
    /// @param ext if false the extension is stripped
    /// @return the name part of the path
    LocalPathName baseName(bool ext = true) const;

    /// File extension
    /// @return file extension (empty string if there is none)
    std::string extension() const;

    /// Return the hash digest of the file
    /// @param method - specifies the hash method to use (default xxHash)
    /// @return the hash digest
    std::string hash(const std::string& method = "xxh64") const;

    /// Check if path exists
    /// @return true if the path exists
    bool exists() const;

    /// Check if path is available
    /// @return true (local paths are always available)
    bool available() const;

    /// Check if path is offsite
    /// @return true (local paths are always available)
    bool offsite() const;

    /// Touch the path. The path leading to it is created if it does not exist.
    void touch() const;

    /// Reserve a given number of Bytes (file must be of length 0 or not exist)
    /// @param len number of Bytes to reserves
    void reserve(const Length&) const;

    /// File size
    /// @return Size in Bytes
    Length size() const;

    /// Last access time
    /// @return Time of last access
    time_t lastAccess() const;

    /// Last modification time
    /// @return Time of last modification
    time_t lastModified() const;

    /// Creation time
    /// @return Time of creation
    time_t created() const;

    /// Check if path is a directory
    /// @return true if the path is a directory
    bool isDir() const;

    /// Check if path is a symlink
    /// @return true if the path is a symlink
    bool isLink() const;

    /// Clear the file
    void empty() const;

    /// Truncate the file to given number of Bytes.
    /// @param len number of Bytes to truncat to
    void truncate(Length) const;

    /// Create the directory and all directories leading to it with given mode
    /// @param mode file mode bits
    void mkdir(short mode = 0777) const;

    /// Change the file mode
    /// @param mode file mode bits
    void chmod(short mode) const;

    /// Unlink the path
    void unlink(bool verbose = true) const;

    /// Remove the directory
    void rmdir(bool verbose = true) const;

    /// Create a copy with a unique path name
    void backup() const;

    /// Create a copy with the given path name
    void copy(const LocalPathName&) const;

    /// Check if a given path points to the same inode as this path
    /// @return true if the path points to the same inode as the given path
    bool sameAs(const LocalPathName&) const;

    /// Mount point of the path
    /// @return mount point of path
    LocalPathName mountPoint() const;

    /// Real path (with symlinks resolved)
    /// @return the real path (with symlinks resolved)
    LocalPathName realName() const;

    /// Get child files and directories
    /// @param files vector to be filled with child files of path
    /// @param directories vector to be filled with child diretories of path
    void children(std::vector<LocalPathName>& files, std::vector<LocalPathName>& dirs) const;

    const std::string& node() const;

    /// String representation
    /// @return string representation of path
    const std::string& path() const;

    void fileSystemSize(FileSystemSize&) const;
    DataHandle* fileHandle(bool overwrite) const;
    DataHandle* partHandle(const OffsetList&, const LengthList&) const;
    DataHandle* partHandle(const Offset&, const Length&) const;

    void syncParentDirectory() const;

    // Class methods

    static LocalPathName unique(const LocalPathName&);
    static void match(const LocalPathName&, std::vector<LocalPathName>&, bool recursive = false);
    static void link(const LocalPathName& from, const LocalPathName& to);
    static void rename(const LocalPathName& from, const LocalPathName& to);
    static void rename(const LocalPathName& from, const std::string& newBase);

    static LocalPathName cwd();

private:

    // Members

    std::string path_;

    // Methods

    LocalPathName& tidy(bool tildeIsUserHome = false, bool skipTildeExpansion = false);

    std::string parsePath(const std::string& p) {
        if (p.compare(0, 8, "local://") == 0) {
            return p.substr(8);
        }
        return p;
    }

    // friend

    friend LocalPathName operator+(const LocalPathName& p, const std::string& s) { return LocalPathName(p.path_ + s); }

    friend LocalPathName operator+(const LocalPathName& p, const char* s) { return LocalPathName(p.path_ + s); }

    friend LocalPathName operator+(const LocalPathName& p, char s) { return LocalPathName(p.path_ + s); }

    friend LocalPathName operator/(const LocalPathName& p1, const LocalPathName& p2);

    friend LocalPathName operator/(const LocalPathName& p1, const char* p2);
};

template <>
struct VectorPrintSelector<LocalPathName> {
    typedef VectorPrintSimple selector;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
