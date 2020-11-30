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

#include <dlfcn.h> // for dlopen

#include "eckit/system/LibraryManager.h"

#include "eckit/config/Resource.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/log/Log.h"
#include "eckit/log/OStreamTarget.h"
#include "eckit/log/PrefixTarget.h"
#include "eckit/os/System.h"
#include "eckit/system/SystemInfo.h"
#include "eckit/system/Library.h"
#include "eckit/system/Plugin.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/Translator.h"
#include "eckit/config/YAMLConfiguration.h"
#include "eckit/config/LocalConfiguration.h"

namespace eckit {
namespace system {

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
                
                ::dlerror(); // clear error

                void* plib = ::dlopen(path.localPath(), RTLD_NOW | RTLD_GLOBAL);
                
                if (plib == nullptr) {
                    std::ostringstream ss;
                    ss << "dlopen(" << path.realName() << ", ...)" << ::dlerror();
                    throw FailedSystemCall(ss.str().c_str(), Here());
                }

                Log::debug() << "Loaded library " << path.realName() << std::endl;

                return plib;
            }
        }

        // now we try with the system LD_LIBRARY_PATH environment variable
        void* plib = ::dlopen(dynamicLibraryName.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (plib) {
            Log::debug() << "Loaded library from LD_LIBRARY_PATH or system: " << dynamicLibraryName.c_str() << std::endl;
            return plib;
        }


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
            if(plugin) {
                Log::debug() << "Loaded plugin [" << name << "] from library [" << lib << "]" << std::endl;
                plugin->handle(libhandle);
                plugin->init();
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
        if(plugin) {
            Log::warning() << "Plugin " << name << " already loaded" << std::endl;
            return *plugin;
        }

        std::ostringstream ss;
        ss << "A library " << name << " is loaded but it is not a Plugin library";
        throw UnexpectedState(ss.str(), Here());
    }

    void autoLoadPlugins(const std::vector<std::string>& inlist) {

        std::vector<std::string> plugins = inlist;

        AutoLock<Mutex> lockme(mutex_);

        static std::string pluginManifestPath = Resource<std::string>("$PLUGINS_MANIFEST_PATH;pluginManifestPath", "~/share/plugins");

        LocalPathName dir(pluginManifestPath);
        if(not dir.exists() or not dir.isDir())
            return;
        
        Log::debug() << "Scanning for plugins manifest files in " << dir << std::endl;

        // scan the manifests to discover the plugins and their respective libraries
        std::map<std::string, LocalConfiguration> manifests;

        std::vector<LocalPathName> files;
        std::vector<LocalPathName> dirs;
        dir.children(files, dirs);
        for (const auto& p : files) {
            PathName path(p);
            Log::debug() << "Found plugin manifest " << path << std::endl;
            YAMLConfiguration conf(path);
            if (conf.has("plugin")) {
                LocalConfiguration manifest = conf.getSubConfiguration("plugin");
                Log::debug() << "Loaded plugin manifest " << manifest << std::endl;
                std::string name = manifest.getString("name");
                ASSERT(manifests.find(name) == manifests.end()); // no duplicate manifests
                manifests[name] = manifest;
            }
        }

        if(plugins.empty()) {
            for(const auto& kv: manifests) {
                plugins.push_back(kv.first);
            }
        }

        Log::debug() << "Going to load following plugins " << plugins << std::endl;

        for (const auto& pname : plugins) {
            if (manifests.find(pname) != manifests.end()) {
                LocalConfiguration manifest = manifests[pname];
                std::string name = manifest.getString("name");
                ASSERT(pname == name);
                std::string lib  = manifest.getString("library");
                Plugin& plugin   = loadPlugin(name, lib);
            }
            else {
                Log::warning() << "Could not find manifest file for plugin " << pname << std::endl;
            }
        }
    }


    void enregisterPlugin(const std::string& name, const std::string& libname) {
        AutoLock<Mutex> lockme(mutex_);
        Log::debug() << "Registered plugin [" << name << "] with library [" << libname << "]" << std::endl;
        ASSERT(plugins_.find(name) == plugins_.end());
        plugins_[name] = libname;
    }

    void deregisterPlugin(const std::string& name) {
        AutoLock<Mutex> lockme(mutex_);
        Log::debug() << "Deregistered plugin [" << name << "]" << std::endl;
        plugins_.erase(name);
    }

private:  // members
    LibraryMap libs_;
    std::map<std::string, std::string> plugins_; //< map plugin name to library
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
