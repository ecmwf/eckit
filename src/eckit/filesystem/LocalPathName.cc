/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "LocalPathName.h"

#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#include <utime.h>

#include <cstring>  // for strlen
#include <deque>
#include <fstream>
#include <sstream>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/filesystem/BasePathNameT.h"
#include "eckit/filesystem/PathNameFactory.h"
#include "eckit/filesystem/StdDir.h"
#include "eckit/io/FileHandle.h"
#include "eckit/io/Length.h"
#include "eckit/io/PartFileHandle.h"
#include "eckit/io/StdFile.h"
#include "eckit/io/cluster/ClusterDisks.h"
#include "eckit/io/cluster/NodeInfo.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/TimeStamp.h"
#include "eckit/os/Stat.h"
#include "eckit/runtime/Main.h"
#include "eckit/system/Library.h"
#include "eckit/system/LibraryManager.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/thread/StaticMutex.h"
#include "eckit/types/Types.h"
#include "eckit/utils/Hash.h"
#include "eckit/utils/Regex.h"
#include "eckit/utils/Tokenizer.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

static pthread_once_t once = PTHREAD_ONCE_INIT;
static std::vector<std::pair<std::string, std::string> > pathsTable;

static void expandTilde(std::string& path, bool tildeIsUserHome);

static void readPathsTable() {

    // We would normally just use a PathName object, defaulting to ~/etc/paths. However, this function
    // is typically called within the constructor of a PathName object, and the recursive call will
    // hang forever on the std::mutex in the PathName factory.
    //
    // expandTilde is internal to LocalPathName anyway, so we call it directly.

    static std::string path = eckit::Resource<std::string>("libraryConfigPaths,$LIBRARY_CONFIG_PATHS", "~/etc/paths");
    static bool expanded    = false;

    if (!expanded) {
        expandTilde(path, false);
        expanded = true;
    }

    std::ifstream in(path);

    eckit::Log::debug() << "Loading library paths from " << path << std::endl;

    if (!in) {
        eckit::Log::debug() << "Failed to read " << path << " -- " << eckit::Log::syserr << std::endl;
        return;
    }

    eckit::Tokenizer parse(" ");

    char line[1024];
    while (in.getline(line, sizeof(line))) {
        std::vector<std::string> s;
        parse(line, s);

        size_t i = 0;
        while (i < s.size()) {
            if (s[i].length() == 0) {
                s.erase(s.begin() + i);
            }
            else {
                i++;
            }
        }

        if (s.size() == 0 || s[0][0] == '#') {
            continue;
        }

        switch (s.size()) {
            case 2:
                pathsTable.push_back(std::make_pair(s[0] + "/", s[1]));
                break;

            default:
                eckit::Log::warning() << "Library Paths: Invalid line ignored: " << line << std::endl;
                break;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

LocalPathName LocalPathName::baseName(bool ext) const {
    const char* q = path_.c_str();
    int n         = -1;
    int i         = 0;

    while (*q) {
        if (*q == '/')
            n = i;
        q++;
        i++;
    }

    std::string s(path_.c_str() + n + 1);
    if (!ext) {
        n = -1;
        i = 0;
        q = s.c_str();
        while (*q) {
            if (*q == '.') {
                n = i;
                break;
            }
            q++;
            i++;
        }
        if (n >= 0)
            s.resize(n);
    }

    // Don't perform tilde expansion on paths that have already been expanded

    bool tildeIsUserHome    = false;
    bool skipTildeExpansion = true;
    return LocalPathName(s, tildeIsUserHome, skipTildeExpansion);
}

std::string LocalPathName::extension() const {
    const std::string base = baseName();
    const size_t lastDot   = base.find_last_of('.');
    // If no . was found return the empty string
    if (lastDot == std::string::npos)
        return "";
    return base.substr(lastDot);
}

std::string LocalPathName::hash(const std::string& method) const {
    std::unique_ptr<Hash> h(HashFactory::instance().build(method));

    FileHandle fh(path_, /* overwrite */ false);
    fh.openForRead();
    AutoClose closer(fh);
    Length size = this->size();
    Buffer buffer(64 * 1024 * 1024);
    Length len = 0;
    while (len < size) {
        Length chunk = fh.read(buffer, buffer.size());
        len += chunk;
        h->add(buffer, chunk);
    }
    ASSERT(len == size);

    return h->digest();
}

LocalPathName LocalPathName::dirName() const {

    int n         = -1;
    int i         = 0;
    const char* q = path_.c_str();
    while (*q) {
        if (*q == '/')
            n = i;
        q++;
        i++;
    }

    switch (n) {
        case -1:
            return std::string(".");

        case 0:
            return std::string("/");
    }

    std::string s(path_);
    s.resize(n);
    return s;
}

BasePathName* LocalPathName::checkClusterNode() const {
    std::string n = ClusterDisks::node(path_);
    if (n != "local") {
        //        Log::warning() << *this << " is now on node [" << n << "]" << std::endl;
        return PathNameFactory::build(std::string("marsfs://") + n + path_);
    }
    return new BasePathNameT<LocalPathName>(LocalPathName(path_));
}

LocalPathName LocalPathName::orphanName() const {

    std::ostringstream os;
    os << mountPoint() << "/orphans/";

    const char* q = path_.c_str();
    while (*q) {
        if (*q == '/')
            os << '_';
        else
            os << *q;
        q++;
    }

    return os.str();
}

bool LocalPathName::exists() const {
    return ::access(path_.c_str(), F_OK) == 0;
}

bool LocalPathName::available() const {
    return true;
}

bool LocalPathName::offsite() const {
    return false;
}

LocalPathName LocalPathName::cwd() {
    char buf[PATH_MAX + 1];
    if (!::getcwd(buf, sizeof(buf)))
        throw FailedSystemCall("getcwd");
    return LocalPathName(buf);
}

static StaticMutex local_mutex;  // for LocalPathName::unique()

LocalPathName LocalPathName::unique(const LocalPathName& path) {
    AutoLock<StaticMutex> lock(local_mutex);

    std::string hostname = Main::hostname();

    static unsigned long long n = (((unsigned long long)::getpid()) << 32);

    static std::string format = "%Y%m%d.%H%M%S";
    std::ostringstream os;
    os << path << '.' << TimeStamp(format) << '.' << hostname << '.' << n++;

    std::string name = os.str();

    while (::access(name.c_str(), F_OK) == 0) {
        std::ostringstream os;
        os << path << '.' << TimeStamp(format) << '.' << hostname << '.' << n++;
        name = os.str();
    }

    LocalPathName result(name);
    result.dirName().mkdir();
    return result;
}

static void mkdir_if_not_exists(const char* path, short mode) {
    Stat::Struct info;

    if (Stat::stat(path, &info) < 0) {
        if (errno == ENOENT)  // no such file or dir
        {
            if (::mkdir(path, mode) < 0) {
                /* don't throw error if it was created meanwhile by another process */
                if (errno != EEXIST) {
                    throw FailedSystemCall(std::string("mkdir ") + path);
                }
            }
        }
        else  // stat fails for unknown reason
        {
            throw FailedSystemCall(std::string("stat ") + path);
        }
    }
}

void LocalPathName::mkdir(short mode) const {
    try {
        char path[PATH_MAX + 1];

        size_t l = path_.length();

        ASSERT(sizeof(path) > l);

        ::strcpy(path, path_.c_str());

        for (size_t i = 1; i < l; i++) {
            if (path[i] == '/') {
                path[i] = 0;

                mkdir_if_not_exists(path, mode);

                path[i] = '/';  // put slash back
            }
        }

        mkdir_if_not_exists(path, mode);
    }
    catch (FailedSystemCall& e) {
        Log::warning() << "Failed to mkdir " << path_ << std::endl;
        throw;
    }
}

void LocalPathName::chmod(short mode) const {
    SYSCALL(::chmod(path_.c_str(), mode));
}

void LocalPathName::link(const LocalPathName& from, const LocalPathName& to) {
    if (::link(from.c_str(), to.c_str()) != 0)
        throw FailedSystemCall(std::string("::link(") + from.path_ + ',' + to.path_ + ')');
}

void LocalPathName::rename(const LocalPathName& from, const LocalPathName& to) {
    if (::rename(from.c_str(), to.c_str()) != 0)
        throw FailedSystemCall(std::string("::rename(") + from.path_ + ',' + to.path_ + ')');
}

void LocalPathName::unlink(bool verbose) const {
    (verbose ? Log::info() : Log::debug<LibEcKit>()) << "Unlink " << path_ << std::endl;
    if (::unlink(path_.c_str()) != 0) {
        if (errno != ENOENT)
            throw FailedSystemCall(std::string("unlink ") + path_);
        else
            Log::info() << "Unlink failed " << path_ << Log::syserr << std::endl;
    }
}

void LocalPathName::rmdir(bool verbose) const {
    (verbose ? Log::info() : Log::debug<LibEcKit>()) << "Rmdir " << path_ << std::endl;
    if (::rmdir(path_.c_str()) != 0) {
        if (errno != ENOENT)
            throw FailedSystemCall(std::string("rmdir ") + path_);
        else
            Log::info() << "Rmdir failed " << path_ << Log::syserr << std::endl;
    }
}


void operator<<(Stream& s, const LocalPathName& path) {
    s << path.path_;
}

void operator>>(Stream& s, LocalPathName& path) {
    s >> path.path_;
}

LocalPathName LocalPathName::fullName() const {
    if (path_.length() > 0 && path_[0] != '/') {
        char buf[PATH_MAX];
        return LocalPathName(std::string(getcwd(buf, PATH_MAX)) + "/" + path_);
    }

    return *this;
}


static void expandTilde(std::string& path, bool tildeIsUserHome) {
    if (path[0] == '~') {
        if (path.length() > 1 && path[1] != '/') {

            if (tildeIsUserHome) {
                NOTIMP;
            }

            std::string s;
            size_t j = 1;
            while (j < path.length() && path[j] != '/') {
                s += path[j];
                j++;
            }


            // 1. match against a path defined in application / tool home ~/etc/paths
            //    or file defined in $LIBRARY_CONFIG_PATHS

            pthread_once(&once, readPathsTable);

            std::vector<std::pair<std::string, std::string> >::const_iterator best = pathsTable.end();
            size_t match                                                           = 0;

            for (std::vector<std::pair<std::string, std::string> >::const_iterator k = pathsTable.begin();
                 k != pathsTable.end(); ++k) {
                const std::string& prefix = (*k).first;
                size_t m                  = prefix.length();

                if (path.substr(0, m) == prefix) {  // longest match is selected
                    if (m > match) {
                        match = m;
                        best  = k;
                    }
                }
            }

            if (match) {
                path = (*best).second + "/" + path.substr(match);
                return;
            }

            // 2. if it has the form ~libname/, then delegate to the matching library to expand the path

            if (eckit::system::LibraryManager::exists(s)) {
                path = eckit::system::LibraryManager::lookup(s).expandPath(path);
                return;
            }

            // in case the library is unknown (not registered), issue error

            std::ostringstream oss;
            oss << "Could not expand tilde in path " << path << std::endl;

            throw SeriousBug(oss.str());
        }

        if (tildeIsUserHome) {

            struct passwd* pw = ::getpwuid(::geteuid());
            ASSERT(pw);
            path = std::string(pw->pw_dir) + "/" + path.substr(1);
            return;
        }

        // 3. expand ~/ with registered home

        path = Main::instance().home() + "/" + path.substr(1);
    }
}

static void tidy_path_tokens(std::vector<std::string>& v) {

    if (v.empty())
        return;

    bool absolute = (v[0] == "");  // path started with / -> absolute path

    std::deque<std::string> path;
    path.push_back(v[0]);

    for (size_t i = 1; i < v.size(); ++i) {

        const std::string& s = v[i];

        if (s.empty() or s == ".") {  // when pushing ignore empty entries generated by '//' or '.'
            continue;
        }

        if (s == "..") {
            if (not path.empty()) {
                if (path.back() == "..") {
                    path.push_back(s);
                }
                else if (path.back() == "") {
                    // "/.." -> "/"
                }
                else if (path.back() == ".") {
                    path.pop_back();
                    path.push_back(s);
                }
                else {
                    path.pop_back();
                }
            }
            else {
                if (not absolute) {
                    path.push_back(s);
                }
            }
            continue;
        }

        path.push_back(s);
    }

    v.clear();
    v.reserve(path.size());
    std::copy(path.begin(), path.end(), std::back_inserter(v));
}

static void rebuild_path(const std::vector<std::string>& v, std::string& path, bool trail = false) {
    path.clear();

    if (v.size() == 1 && v[0] == "") {  // first entry as empty string is '/'
        path = "/";
        return;
    }

    for (std::vector<std::string>::const_iterator p = v.begin(); p != v.end(); ++p) {
        path += *p;

        if (p != v.end() - 1)
            path += '/';
    }
    if (trail) {  // preserve trailing '/'
        path += '/';
    }
}

LocalPathName& LocalPathName::tidy(bool tildeIsUserHome, bool skipTildeExpansion) {
    if (path_.length() == 0)
        return *this;

    if (!skipTildeExpansion) {
        expandTilde(path_, tildeIsUserHome);
    }

    bool trail = (path_[path_.length() - 1] == '/');  // remember to put back ending '/' if there is one

    std::vector<std::string> tokens;

    eckit::Tokenizer tok('/', true);
    tok(path_, tokens);

    tidy_path_tokens(tokens);
    rebuild_path(tokens, path_, trail);

    return *this;
}

void LocalPathName::match(const LocalPathName& root, std::vector<LocalPathName>& result, bool recursive) {
    // Note that pattern matching will only be done
    // on the base name.

    LocalPathName dir = root.dirName();
    std::string base  = root.baseName();

    //    Log::info() << "dir  = " << dir << std::endl;
    //    Log::info() << "base = " << base << std::endl;

    // all those call should be members of LocalPathName

    // long len = base.length() * 2 + 2;

    Regex re(base, true);

    StdDir d(dir);

    if (d == 0) {
        Log::error() << "opendir(" << dir << ")" << Log::syserr << std::endl;
        throw FailedSystemCall(std::string("opendir(") + std::string(dir) + ")");
    }

    for (;;) {
        struct dirent* e = d.dirent();
        if (e == nullptr)
            break;

        //        Log::info() << "e->d_name = " << e->d_name << std::endl;

        if (re.match(e->d_name)) {
            //            Log::info() << "match !!! ---> " << e->d_name << std::endl;
            LocalPathName path = std::string(dir) + std::string("/") + std::string(e->d_name);
            result.push_back(path);
        }

        if (recursive && e->d_name[0] != '.') {
            LocalPathName full = dir + "/" + e->d_name;
            if (full.isDir()) {
                match(full + "/" + base, result, true);
            }
        }
    }
}

LocalPathName LocalPathName::relativePath(const LocalPathName& other) const {

    ASSERT(path_.size() && path_[0] == '/');              // ensure this path is absolute
    ASSERT(other.path_.size() && other.path_[0] == '/');  // ensure other path is absolute

    eckit::Tokenizer tokens("/");

    std::vector<std::string> s1;
    tokens(path_, s1);

    std::vector<std::string> s2;
    tokens(other.path_, s2);

    // find common token
    std::vector<std::string>::const_iterator i = s1.begin();
    std::vector<std::string>::const_iterator j = s2.begin();
    for (; i != s1.end() && j != s2.end(); ++i, ++j) {
        //        std::cout << "comparing " << *i << " " << *j << std::endl;
        if (*i != *j) {
            break;
        }
    }

    std::string result;
    const char* p = 0;

    for (std::vector<std::string>::const_iterator r = j; r != s2.end(); ++r) {
        if (p)
            result += p;
        result += "..";
        p = "/";
    }

    for (std::vector<std::string>::const_iterator r = i; r != s1.end(); ++r) {
        if (p)
            result += p;
        result += *r;
        p = "/";
    }

    if (result.empty()) {
        result = ".";
    }

    return result;
}


void LocalPathName::children(std::vector<LocalPathName>& files, std::vector<LocalPathName>& dirs) const {
    StdDir d(*this);

    if (d == 0) {
        Log::error() << "opendir(" << *this << ")" << Log::syserr << std::endl;
        throw FailedSystemCall("opendir");
    }

    for (;;) {
        struct dirent* e = d.dirent();
        if (e == nullptr) {
            break;
        }

        if (e->d_name[0] == '.')
            if (e->d_name[1] == 0 || (e->d_name[1] == '.' && e->d_name[2] == 0))
                continue;

        LocalPathName full = *this + "/" + e->d_name;

        bool do_stat = true;

#if defined(eckit_HAVE_DIRENT_D_TYPE)
        do_stat = false;
        if (e->d_type == DT_DIR) {
            dirs.push_back(full);
        }
        else if (e->d_type == DT_UNKNOWN) {
            do_stat = true;
        }
        else {
            files.push_back(full);
        }
#endif

        if (do_stat) {
            Stat::Struct info;
            if (Stat::stat(full.c_str(), &info) == 0) {
                if (S_ISDIR(info.st_mode))
                    dirs.push_back(full);
                else
                    files.push_back(full);
            }
            else {
                Log::error() << "Cannot stat " << full << Log::syserr << std::endl;
            }
        }
    }
}

void LocalPathName::touch() const {
    dirName().mkdir();

    if (exists()) {
        SYSCALL(::utime(path_.c_str(), nullptr));  // set to current time
    }
    else {
        AutoStdFile f(*this, "a");  // this creates the file, ther may be a race but is unimportant in this context
    }
}

// This method is used by TxnLog. It is important that
// the inode is preserved, otherwise ftok will give different
// result

void LocalPathName::empty() const {
    AutoStdFile f(*this, "w");  // This should clear the file
}

void LocalPathName::copy(const LocalPathName& other) const {
    FileHandle in(*this);
    FileHandle out(other);
    in.saveInto(out);
}

void LocalPathName::backup() const {
    copy(unique(*this));
}

Length LocalPathName::size() const {
    Stat::Struct info;

    if (Stat::stat(path_.c_str(), &info) < 0)
        throw FailedSystemCall(path_);

    // Should ASSERT(is file)

    return info.st_size;
}

time_t LocalPathName::created() const {
    Stat::Struct info;
    if (Stat::stat(path_.c_str(), &info) < 0)
        throw FailedSystemCall(path_);
    return info.st_ctime;
}

time_t LocalPathName::lastModified() const {
    Stat::Struct info;
    if (Stat::stat(path_.c_str(), &info) < 0)
        throw FailedSystemCall(path_);
    return info.st_mtime;
}

time_t LocalPathName::lastAccess() const {
    Stat::Struct info;
    if (Stat::stat(path_.c_str(), &info) < 0)
        throw FailedSystemCall(path_);
    return info.st_atime;
}

bool LocalPathName::isDir() const {
    Stat::Struct info;
    if (Stat::stat(path_.c_str(), &info) < 0)
        throw FailedSystemCall(path_);
    return S_ISDIR(info.st_mode);
}

bool LocalPathName::isLink() const {
    Stat::Struct info;
    if (Stat::lstat(path_.c_str(), &info) < 0)
        throw FailedSystemCall(path_);
    return S_ISLNK(info.st_mode);
}

bool LocalPathName::sameAs(const LocalPathName& other) const {
    if (!exists() || !other.exists())
        return false;

    Stat::Struct info1;
    SYSCALL(Stat::stat(path_.c_str(), &info1));
    Stat::Struct info2;
    SYSCALL(Stat::stat(other.path_.c_str(), &info2));
    return (info1.st_dev == info2.st_dev) && (info1.st_ino == info2.st_ino);
}

LocalPathName LocalPathName::realName() const {
    char result[PATH_MAX + 1];
    // This code is certainly machine dependant
    if (!::realpath(c_str(), result))
        throw FailedSystemCall(std::string("realpath ") + path_);
    return result;
#if 0
    char save[PATH_MAX + 1];
    if (!getcwd(save, sizeof(save)))
        throw FailedSystemCall("getcwd");

    bool direct = isDir();

    if (direct)
        SYSCALL(::chdir(path_.c_str()));
    else
        SYSCALL(::chdir(dirName().path_.c_str()));

    char dir[PATH_MAX + 1];
    if (!getcwd(dir, sizeof(dir)))
    {
        PANIC(::chdir(save) != 0); // We must go back
        throw FailedSystemCall("getcwd");
    }

    if (direct)
        return LocalPathName(dir);
    else
        return LocalPathName(std::string(dir) + "/" + baseName());
#endif
}

void LocalPathName::truncate(Length len) const {
    SYSCALL(::truncate(path_.c_str(), len));
}

void LocalPathName::reserve(const Length& len) const {
    ASSERT(!exists() || size() == Length(0));

    PartFileHandle part("/dev/zero", 0, len);
    FileHandle file(*this);

    Log::status() << "Reserving " << Bytes(len) << std::endl;
    part.saveInto(file);
}

void LocalPathName::fileSystemSize(FileSystemSize& fs) const {
    struct statvfs d;
    SYSCALL(::statvfs(path_.c_str(), &d));
    long unavail = (d.f_bfree - d.f_bavail);
    fs.available = (unsigned long long)d.f_bavail * (unsigned long long)d.f_bsize;
    fs.total     = (unsigned long long)(d.f_blocks - unavail) * (unsigned long long)d.f_bsize;
}


DataHandle* LocalPathName::fileHandle(bool overwrite) const {
    return new FileHandle(path_, overwrite);
}

DataHandle* LocalPathName::partHandle(const OffsetList& o, const LengthList& l) const {
    return new PartFileHandle(path_, o, l);
}

DataHandle* LocalPathName::partHandle(const Offset& o, const Length& l) const {
    return new PartFileHandle(path_, o, l);
}

LocalPathName LocalPathName::mountPoint() const {
    //  dev_t last;
    Stat::Struct s;
    LocalPathName p(*this);

    ASSERT(p.path_.length() > 0 && p.path_[0] == '/');

    SYSCALL2(Stat::stat(p.c_str(), &s), p);
    dev_t dev = s.st_dev;

    while (p != "/") {
        LocalPathName q(p.dirName());
        SYSCALL(Stat::stat(q.c_str(), &s));
        if (s.st_dev != dev)
            return p;
        p = q;
    }

    return p;
}

void LocalPathName::syncParentDirectory() const {
    PathName directory = dirName();
#ifdef eckit_HAVE_DIRFD
    //    Log::info() << "Syncing directory " << directory << std::endl;
    DIR* d = opendir(directory.localPath());
    if (!d)
        SYSCALL(-1);

    int dir;
    SYSCALL(dir = dirfd(d));
    int ret = fsync(dir);

    while (ret < 0 && errno == EINTR) {
        ret = fsync(dir);
    }

    if (ret < 0) {
        std::ostringstream oss;
        Log::error() << "Cannot fsync directory [" << directory << "]";
        throw FailedSystemCall(oss.str());
    }

    ::closedir(d);
#else
    Log::info() << "Syncing directory " << directory << " (not supported)" << std::endl;
#endif
}

const std::string& LocalPathName::node() const {
    return NodeInfo::thisNode().node();
}

const std::string& LocalPathName::path() const {
    return path_;
}

std::string LocalPathName::clusterName() const {
    std::ostringstream os;
    os << "marsfs://" << node() << fullName();
    return os.str();
}


}  // namespace eckit
