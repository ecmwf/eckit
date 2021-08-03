/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date   November 2020

#include <algorithm>
#include <cctype>
#include <map>

#include <dlfcn.h>   // for dlopen
#include <limits.h>  // for PATH_MAX

#include "eckit/system/LibraryManager.h"


#include "eckit/config/LocalConfiguration.h"
#include "eckit/config/Resource.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/log/Log.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/PrefixTarget.h"
#include "eckit/os/System.h"
#include "eckit/system/Library.h"
#include "eckit/system/Plugin.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/Tokenizer.h"
#include "eckit/utils/Translator.h"

namespace eckit {
namespace system {

//----------------------------------------------------------------------------------------------------------------------

static std::string path_from_libhandle(const std::string& libname, void* handle) {
#ifdef eckit_HAVE_DLINFO
    char path[PATH_MAX];
    if (::dlinfo(handle, RTLD_DI_ORIGIN, path) < 0) {
        std::ostringstream ss;
        ss << "dlinfo(" << libname << ", ...) " << ::dlerror();
        throw FailedSystemCall(ss.str().c_str(), Here());
    }
    return std::string(path) + "/" + libname;
#else
    return libname;
#endif
}

//----------------------------------------------------------------------------------------------------------------------

/// Registry for all libraries
///
class LibraryRegistry {

    typedef std::map<std::string, Library*> LibraryMap;

public:  // methods
    /// Builds the registry on demand, needed for correct static initialization
    /// because factories can be initialized first
    static LibraryRegistry& instance() {
        static LibraryRegistry reg;
        return reg;
    }

    ~LibraryRegistry() {
        for (auto p : loadedPlugins()) {
            unloadPlugin(p);
        }
    }

    /// Registers an entry to the registry
    /// @pre Cannot exist yet
    /// @param obj pointer must be valid
    void enregister(const std::string& name, Library* obj) {
        AutoLock<Mutex> lockme(mutex_);
        ASSERT(obj);
        ASSERT(libs_.find(name) == libs_.end());
        Log::debug() << "Registering library [" << name << "] with address [" << obj << "]" << std::endl;
        libs_[name] = obj;
    }

    /// Removes an entry from the registry
    /// @pre Must exist
    void deregister(const std::string& name) {
        AutoLock<Mutex> lockme(mutex_);
        ASSERT(libs_.find(name) != libs_.end());
        libs_.erase(name);
    }

    /// List entries in library
    std::vector<std::string> list() const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> result;
        for (LibraryMap::const_iterator j = libs_.begin(); j != libs_.end(); ++j) {
            result.push_back(j->first);
        }
        return result;
    }

    /// List loaded library plugins
    std::vector<std::string> loadedPlugins() const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> result;
        for (const auto& p : plugins_) {
            result.push_back(p.first);
        }
        return result;
    }

    /// Check entry exists in registry
    bool exists(const std::string& name) const {
        AutoLock<Mutex> lockme(mutex_);
        LibraryMap::const_iterator j = libs_.find(name);
        return (j != libs_.end());
    }

    /// Prints the entries in registry
    void print(std::ostream& out, const char* separator) const {
        AutoLock<Mutex> lockme(mutex_);
        std::vector<std::string> l = LibraryRegistry::instance().list();
        const char* sep            = "";
        for (auto j : l) {
            out << sep << j;
            sep = separator;
        }
    }

    /// Lookup entry in the registry
    Library& lookup(const std::string& name) const {
        AutoLock<Mutex> lockme(mutex_);

        LibraryMap::const_iterator j = libs_.find(name);

        if (j == libs_.end()) {
            eckit::Log::error() << "No Library found with name '" << name << "'" << std::endl;
            eckit::Log::error() << "Registered libraries are:";
            print(eckit::Log::error(), "\n");
            throw eckit::SeriousBug(std::string("No Library found with name ") + name);
        }

        ASSERT(j->second);
        return *(j->second);
    }

    void* loadDynamicLibrary(const std::string& libname) {

        static std::vector<std::string> libPaths(Resource<std::vector<std::string>>(
            "dynamicLibraryPath;$DYNAMIC_LIBRARY_PATH", {"~/lib64", "~/lib", "~eckit/lib64", "~eckit/lib"}));

        // Get the (system specific) library name for the given library instance
        std::string dynamicLibraryName = SystemInfo::instance().dynamicLibraryName(libname);

        // Try the various paths in the way
        for (const std::string& dir : libPaths) {

            LocalPathName path = dir + "/" + dynamicLibraryName;

            if (path.exists()) {

                ::dlerror();  // clear error

                Log::debug() << "Loading library " << path.realName() << std::endl;

                void* plib = ::dlopen(path.localPath(), RTLD_NOW | RTLD_GLOBAL);

                if (plib == nullptr) {
                    std::ostringstream ss;
                    ss << "dlopen(" << path.realName() << ", ...) " << ::dlerror();
                    throw FailedSystemCall(ss.str().c_str(), Here());
                }

                Log::debug() << "Loaded library " << path_from_libhandle(dynamicLibraryName, plib) << std::endl;
                return plib;
            }
        }

        // now we try with the system LD_LIBRARY_PATH environment variable
        Log::debug() << "Loading library " << dynamicLibraryName << " from LD_LIBRARY_PATH or system paths"
                     << std::endl;
        void* plib = ::dlopen(dynamicLibraryName.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (plib) {
            Log::debug() << "Loaded library " << path_from_libhandle(dynamicLibraryName, plib) << std::endl;
            return plib;
        }

        Log::warning() << "Failed to load library " << dynamicLibraryName << std::endl;
        return nullptr;
    }

    Plugin* lookupPlugin(const std::string& name) const {
        auto it = plugins_.find(name);
        if (it != plugins_.end()) {
            std::string libname = it->second;
            return dynamic_cast<Plugin*>(&lookup(libname));
        }
        else {
            return nullptr;
        }
    }

    Plugin& loadPlugin(const std::string& name, const std::string& libname = std::string()) {

        AutoLock<Mutex> lockme(mutex_);

        std::string lib = libname.empty() ? name : libname;

        // check if respective library is already loaded
        if (!exists(lib)) {

            // lets load since the associated library isn't registered
            void* libhandle = loadDynamicLibrary(lib);

            // the plugin should self-register when the library loads
            Plugin* plugin = lookupPlugin(name);
            if (plugin) {
                Log::debug() << "Loaded plugin [" << name << "] from library [" << lib << "]" << std::endl;
                plugin->handle(libhandle);
                initPlugin(plugin);
                return *plugin;
            }
            else {
                // If the plugin library still doesn't exist after a successful call of dlopen, then
                // we have managed to load something other than a (self-registering) eckit Plugin library
                std::ostringstream ss;
                ss << "Plugin library " << lib << " loaded but Plugin object " << name << " not registered";
                throw UnexpectedState(ss.str(), Here());
            }
        }

        Plugin* plugin = lookupPlugin(name);
        if (plugin) {
            // Plugin is already loaded, likely because it is explicitly linked into the executable
            Log::debug() << "Plugin [" << name << "] already loaded" << std::endl;
            initPlugin(plugin);
            return *plugin;
        }

        std::ostringstream ss;
        ss << "A library " << name << " is loaded but it is not a Plugin library";
        throw UnexpectedState(ss.str(), Here());
    }

    bool unloadPlugin(const std::string& name) {
        Plugin* plugin = lookupPlugin(name);
        if (plugin) {
            plugin->finalise();
            void* handle = plugin->handle();
            if (handle) {
                ::dlerror();  // clear error
                int err = ::dlclose(handle);
                if (err) {
                    std::ostringstream ss;
                    ss << "Failed system call to ::dlclose() for plugin " << name << " - " << ::dlerror();
                    throw FailedSystemCall(ss.str().c_str(), Here());
                }
                return true;
            }
        }
        return false;
    }

    void initPlugin(Plugin* plugin) {
        if (!is_plugin_initialized_[plugin->name()]) {
            Log::debug() << "Initializing plugin [" << plugin->name() << "]" << std::endl;
            plugin->init();
            is_plugin_initialized_[plugin->name()] = true;
        }
    }

    std::map<std::string, LocalConfiguration> scanManifestPaths() {
        std::map<std::string, LocalConfiguration> manifests;

        static std::string pluginManifestPath = Resource<std::string>("$PLUGINS_MANIFEST_PATH;pluginManifestPath", "");

        eckit::Tokenizer tokenize(":");
        std::vector<std::string> scanPaths;
        tokenize(pluginManifestPath, scanPaths);

        // always scan ~eckit/share/plugins and ~/share/plugins as a last resort
        scanPaths.push_back("~eckit/share/plugins");
        scanPaths.push_back("~/share/plugins");

        Log::debug() << "Plugins manifest candidate paths " << scanPaths << std::endl;

        std::set<LocalPathName> visited;  //< we dont visit same path twice

        for (auto& path : scanPaths) {

            LocalPathName dir(path);
            if (not dir.exists() or not dir.isDir())
                continue;

            LocalPathName realdir(dir.realName());
            if (visited.count(realdir)) {
                Log::debug() << "Skipping plugins manifest dir already visited: " << realdir << std::endl;
                continue;
            }

            visited.insert(realdir);

            Log::debug() << "Scanning for plugins manifest path " << path << " resolved to " << realdir << std::endl;

            // scan the manifests to discover the plugins and their respective libraries

            std::vector<LocalPathName> files;
            std::vector<LocalPathName> dirs;
            realdir.children(files, dirs);
            for (const auto& p : files) {
                PathName path(p);
                Log::debug() << "Found plugin manifest " << path << std::endl;
                YAMLConfiguration conf(path);
                if (conf.has("plugin")) {
                    LocalConfiguration manifest = conf.getSubConfiguration("plugin");
                    Log::debug() << "Loaded plugin manifest " << manifest << std::endl;
                    std::string name              = manifest.getString("name");
                    std::string namespce          = manifest.getString("namespace");
                    std::string fullQualifiedName = namespce + "." + name;
                    if (manifests.find(fullQualifiedName) == manifests.end()) {
                        manifests[fullQualifiedName] = manifest;
                    }
                    else {
                        Log::debug() << "The plugin " << fullQualifiedName
                                     << " was already found before, skipping plugin defined in " << path << std::endl;
                    }
                }
            }
        }
        return manifests;
    }


    void autoLoadPlugins(const std::vector<std::string>& inlist) {

        std::vector<std::string> plugins = inlist;

        AutoLock<Mutex> lockme(mutex_);

        std::map<std::string, LocalConfiguration> manifests = scanManifestPaths();

        // if no plugins configured we load all what was found in the manifests
        if (plugins.empty()) {
            for (const auto& kv : manifests) {
                plugins.push_back(kv.first);
            }
        }

        Log::debug() << "Going to load following plugins " << plugins << std::endl;

        // loop over full qualified plugin names
        for (const auto& fqname : plugins) {
            if (manifests.find(fqname) != manifests.end()) {
                LocalConfiguration manifest   = manifests[fqname];
                std::string name              = manifest.getString("name");
                std::string namespce          = manifest.getString("namespace");
                std::string fullQualifiedName = namespce + "." + name;
                ASSERT(fqname == fullQualifiedName);
                std::string lib = manifest.getString("library");
                Plugin& plugin  = loadPlugin(name, lib);
            }
            else {
                Log::warning() << "Could not find manifest file for plugin " << fqname << std::endl;
            }
        }
    }


    void enregisterPlugin(const std::string& name, const std::string& libname) {
        AutoLock<Mutex> lockme(mutex_);
        Log::debug() << "Registered plugin [" << name << "] with library [" << libname << "]" << std::endl;
        ASSERT(plugins_.find(name) == plugins_.end());
        plugins_[name]               = libname;
        is_plugin_initialized_[name] = false;
    }

    void deregisterPlugin(const std::string& name) {
        // dont add logging here, since order of destruction of plugins not guaranteed to be reversed from Log
        // singletons
        AutoLock<Mutex> lockme(mutex_);
        plugins_.erase(name);
        is_plugin_initialized_.erase(name);
    }

private:  // members
    LibraryMap libs_;
    std::map<std::string, std::string> plugins_;  //< map plugin name to library
    std::map<std::string, bool> is_plugin_initialized_;
    mutable Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

void LibraryManager::enregister(const std::string& name, Library* obj) {
    LibraryRegistry::instance().enregister(name, obj);
}

std::vector<std::string> LibraryManager::list() {
    return LibraryRegistry::instance().list();
}

std::vector<std::string> LibraryManager::loadedPlugins() {
    return LibraryRegistry::instance().loadedPlugins();
}

void LibraryManager::list(std::ostream& out) {
    return LibraryRegistry::instance().print(out, ", ");
}

bool LibraryManager::exists(const std::string& name) {
    return LibraryRegistry::instance().exists(name);
}

const Library& LibraryManager::lookup(const std::string& name) {
    return LibraryRegistry::instance().lookup(name);
}

const Plugin& LibraryManager::lookupPlugin(const std::string& name) {
    Plugin * plugin = LibraryRegistry::instance().lookupPlugin(name);
    if(plugin)
        return *plugin;
    throw eckit::BadValue("Plugin " + name + " not loaded");
}

void* LibraryManager::loadLibrary(const std::string& libname) {
    return LibraryRegistry::instance().loadDynamicLibrary(libname);
}

Plugin& LibraryManager::loadPlugin(const std::string& name, const std::string& lib) {
    return LibraryRegistry::instance().loadPlugin(name, lib);
}

void LibraryManager::autoLoadPlugins(const std::vector<std::string>& plugins) {
    LibraryRegistry::instance().autoLoadPlugins(plugins);
}

void LibraryManager::enregisterPlugin(const std::string& name, const std::string& libname) {
    LibraryRegistry::instance().enregisterPlugin(name, libname);
}

void LibraryManager::deregisterPlugin(const std::string& name) {
    LibraryRegistry::instance().deregisterPlugin(name);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace system
}  // namespace eckit
