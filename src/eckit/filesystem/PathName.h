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


#ifndef eckit_filesystem_PathName_h
#define eckit_filesystem_PathName_h

#include "eckit/filesystem/FileSystemSize.h"
#include "eckit/io/Length.h"
#include "eckit/io/Offset.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/types/Types.h"

#include <map>
#include <mutex>

namespace eckit {

class BasePathName;
class DataHandle;
class FileMode;
class Hash;
class Length;
class LocalPathName;

// The class PathName represent a unix path name.

class PathName {
public:

    friend void operator<<(Stream&, const PathName&);
    friend void operator>>(Stream&, PathName&);

    friend std::ostream& operator<<(std::ostream& s, const PathName& p) {
        p.print(s);
        return s;
    }

    // Contructors

    PathName(const char* p = "/", bool tildeIsUserHome = false);
    PathName(const std::string& p, bool tildeIsUserHome = false);
    PathName(const std::string& type, const std::string& p, bool tildeIsUserHome = false);
    PathName(const PathName& p);
    PathName(const LocalPathName&);

    // Destructor
    ~PathName();

    // Assignment

    PathName& operator=(const PathName& p);
    PathName& operator=(const std::string& p);
    PathName& operator=(const char* p);

    // Convertors

    std::string asString() const;
    operator std::string() const { return asString(); }
    /* const char* c_str() const      { return path_.c_str(); } */

    const char* localPath() const;

    // Operators

    PathName& operator+=(const std::string& s);
    PathName& operator+=(const char* s);
    PathName& operator+=(char s);

    PathName& operator/=(const std::string& s);
    PathName& operator/=(const char* s);
    PathName& operator/=(char s);

    bool operator<(const PathName& other) const;
    bool operator>(const PathName& other) const;
    bool operator<=(const PathName& other) const;
    bool operator>=(const PathName& other) const;
    bool operator!=(const PathName& other) const;
    bool operator==(const PathName& other) const;

    // Methods

    /// What type of PathName object is this
    const char* type() const;

    /// Directory part of the path
    /// @return directory part of the path
    PathName dirName() const;

    /// Absolute path
    /// @return absolute path
    PathName fullName() const;

    PathName clusterName() const;

    /// Base name of the path
    /// @param ext if false the extension is stripped
    /// @return the name part of the path
    PathName baseName(bool ext = true) const;

    /// path extension
    /// @return file extension (empty string if there is none)
    std::string extension() const;

    /// Check if path exists
    /// @return true if the path exists
    bool exists() const;

    /// Check if path is available
    /// @return true (local paths are always available)
    bool available() const;

    /// Check if path is offsite
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

    /// File owner
    /// @return Owner ID
    uid_t owner() const;

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
    void chmod(const FileMode& mode) const;

    /// Unlink the path
    void unlink(bool verbose = true) const;

    /// Remove the directory
    void rmdir(bool verbose = true) const;

    /// Synchronise the parent directory entries to persistent storage
    void syncParentDirectory() const;

    /// Create a copy with a unique path name
    void backup() const;

    /// Create a copy with the given path name
    void copy(const PathName&) const;

    /// Check if a given path points to the same inode as this path
    /// @return true if the path points to the same inode as the given path
    bool sameAs(const PathName&) const;

    /// Mount point of the path
    /// @return mount point of path
    PathName mountPoint() const;

    /// Real path (with symlinks resolved)
    /// @return the real path (with symlinks resolved)
    PathName realName() const;

    PathName orphanName() const;
    PathName checkClusterNode() const;

    const std::string& node() const;

    /// String representation
    /// @return string representation of path
    const std::string& path() const;

    /// Hash the contents of this pathname (only valid for files)
    void hash(Hash&) const;

    /// Hash a path as in-place as possible. Returns the hash digest
    std::string hash(const std::string& method = "xxh64") const;

    /// Get child files and directories
    /// @param files vector to be filled with child files of path
    /// @param directories vector to be filled with child diretories of path
    void children(std::vector<PathName>& files, std::vector<PathName>& dirs) const;

    /// Get child files and directories descending recursively on all sub directories
    /// @param files vector to be filled with child files of path
    /// @param directories vector to be filled with child diretories of path
    void childrenRecursive(std::vector<PathName>& files, std::vector<PathName>& dirs) const;

    void fileSystemSize(FileSystemSize&) const;

    DataHandle* fileHandle(bool overwrite = false) const;
    DataHandle* partHandle(const OffsetList&, const LengthList&) const;
    DataHandle* partHandle(const Offset&, const Length&) const;

    // Class methods

    static PathName unique(const PathName&);
    static void match(const PathName&, std::vector<PathName>&, bool = false);
    static void link(const PathName& from, const PathName& to);
    static void rename(const PathName& from, const PathName& to);
    static void rename(const PathName& from, const std::string& newBase);

    static std::string shorten(const std::string&);
    static std::string metricsTag(const std::string&);

private:

    PathName(BasePathName*);

    // Members


    BasePathName* path_;

    // Methods

    PathName& tidy();
    void print(std::ostream&) const;

    // friend

    friend PathName operator+(const PathName& p, const std::string& s);
    friend PathName operator+(const PathName& p, const char* s);
    friend PathName operator+(const PathName& p, char s);

    friend PathName operator/(const PathName& p, const std::string& s);
    friend PathName operator/(const PathName& p, const char* s);
    friend PathName operator/(const PathName& p, char s);
};

template <>
struct VectorPrintSelector<PathName> {
    typedef VectorPrintSimple selector;
};

}  // namespace eckit

#endif
