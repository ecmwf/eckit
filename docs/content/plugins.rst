Plugins
=======

eckit supports loading additional functionality at runtime via plugins:
shared libraries that derive from :cpp:class:`eckit::system::Plugin` and
self-register with :cpp:class:`eckit::system::LibraryManager` as their
static instance is constructed.

Plugins are described by YAML manifest files. The loader discovers
manifests, validates them against the running process and ``dlopen``\ s
the corresponding shared object on demand.

Once loaded, the `init()` method of the plugin is called. Prior to unloading
the plugin, during rollback of a failed load, or during clean shutdown of
the process, the plugin's `finalise()` method is called.

Many plugins create global resources, which self-register with appropriate
global factories (in eckit or elsewhere). The self-registering resources
should be instantiated in the plugin's `init()` method, and cleaned up in
`finalise()`, rather than relying on static (global) initialisation and
destruction, if this is at all possible.

Manifest discovery
------------------

When ``LibraryManager`` scans for manifests it visits the following
directories, in this order, stopping at the first match per fully
qualified plugin name:

#. Each directory listed in ``$PLUGINS_MANIFEST_PATH``
   (or the ``pluginManifestPath`` resource), ``:``-separated.
#. ``<path>/share/plugins`` for every directory registered with
   :cpp:func:`eckit::system::LibraryManager::addPluginSearchPath`.
#. The directories returned by
   :cpp:func:`eckit::system::Library::pluginManifestPaths` for each
   currently registered library (default: ``~<libname>/share/plugins``).
#. ``~eckit/share/plugins``
#. ``~/share/plugins``

Symlinks resolve to canonical paths and the same manifest directory is
never scanned twice. The first manifest seen for a fully qualified name
``namespace.name`` wins; subsequent duplicates are logged and ignored.

Manifest schema
---------------

A manifest is a YAML file with a single top-level ``plugin:`` mapping.

.. list-table::
   :header-rows: 1
   :widths: 20 15 65

   * - Key
     - Required
     - Meaning
   * - ``name``
     - yes
     - Plugin name (matches the argument passed to the
       :cpp:class:`Plugin <eckit::system::Plugin>` constructor in C++).
   * - ``namespace``
     - yes
     - Namespace used to build the fully qualified name (example int.ecmwf)
       ``namespace.name``.
   * - ``library``
     - yes
     - Shared library file stem, without the ``lib`` prefix or
       platform-specific extension.
   * - ``for-library``
     - no
     - Owning library name. When set, the plugin is *scoped* and is
       only loaded by an explicit request from that library or by
       fully-qualified name (see below). When absent the plugin is
       *global*.
   * - ``min-version``
     - no
     - Minimum version of ``for-library`` required for the plugin to
       load. Compared as semver-ish numeric ``major.minor[.patch]``;
       if the registered library's version is lower throw ``BadValue``.
   * - ``version``
     - no
     - Pinned exact value of the plugin's self-reported
       :cpp:func:`Plugin::version`. Compared verbatim (string equality,
       not semver) against the version reported by the Plugin object
       at runtime. On mismatch the loader unloads the offending shared
       object again and throws ``BadValue`` without calling the Plugin's
       ``init()`` function.
   * - ``tags``
     - no
     - Free-form list of labels used by ``loadPluginsFor`` for filtering.

Example:

.. code-block:: yaml

   plugin:
     name: my-plugin
     namespace: int.ecmwf
     library: my-plugin
     for-library: eckit
     min-version: 1.20.0
     version: 0.4.2
     tags: [grids, io]

Loading model
-------------

There are three orthogonal ways a plugin can be loaded.

**Global auto-load (Main startup)**
   Plugins without ``for-library`` are loaded by :cpp:class:`eckit::Main`
   on construction when both:

   * the ``Main`` constructor's ``autoLoadPlugins`` parameter is
     ``true`` (the default), and
   * the resource ``$AUTO_LOAD_PLUGINS`` (default ``true``) is true.

   Scoped plugins are *not* loaded by this path.

   ***Note: This mechanism is deprecated and will be removed in the future.***
   Plugins should be explictily loaded by their owning library via the scoped
   mechanism described below.

**Scoped load (library-driven)**
   A library opts in to loading its scoped plugins by calling
   :cpp:func:`eckit::system::LibraryManager::loadPluginsFor` from its
   initialisation routine, or lazily during execution when required, e.g.::

       LibraryManager::loadPluginsFor("my-lib", {"grids"});

   Only manifests whose ``for-library`` matches and whose ``tags``
   include *all* of the requested tags are considered. An empty tag
   list matches every scoped manifest for the library. ``min-version``
   and ``version`` checks apply.

**Explicit list**
   Setting ``$LOAD_PLUGINS`` (or the ``loadPlugins`` resource) to a
   comma-separated list of fully qualified names loads exactly those
   plugins, scoped or not, at ``Main`` startup. A name with no matching
   manifest, or a scoped entry whose owning library is not registered,
   throws ``BadValue``.

Disabling plugins
-----------------

Setting ``$DISABLE_PLUGINS`` (or the ``disablePlugins`` resource) to a
comma- or colon-separated list of fully qualified plugin names
suppresses both auto-load and scoped load for those plugins.

Subclassing ``Main``
--------------------

A subclass that should not perform global plugin auto-load passes
``false`` for the ``autoLoadPlugins`` constructor argument::

   class MyMain : public eckit::Main {
   public:
       MyMain(int argc, char** argv) :
           Main(argc, argv, /*homeenv=*/nullptr, /*autoLoadPlugins=*/false) {}
   };

The default value of ``true`` is expected to flip to ``false`` in a
future release once existing plugins have migrated to scoped
(``for-library``) manifests.

Migration notes
---------------

Behavioural changes introduced with the scoped-plugin support:

* ``LibraryManager::autoLoadPlugins({})`` no longer loads scoped
  plugins. Previously every discovered manifest was loaded
  indiscriminately. Library authors must now load their scoped plugins
  via ``loadPluginsFor`` from their library's initialisation path.
* A name listed in ``$LOAD_PLUGINS`` that has no matching manifest now
  throws ``BadValue``. Previously the loader emitted a warning and
  continued.
* A manifest declaring ``version:`` whose value differs from the
  installed shared object's :cpp:func:`Plugin::version` now causes the
  shared object to be unloaded again and ``BadValue`` is thrown. This
  helps detect cases where a manifest on disk no longer matches the
  binary that satisfies its ``library:`` entry.
* :cpp:func:`Plugin::finalise` may be invoked even when
  :cpp:func:`Plugin::init` was never called (the rollback path of a
  failed ``version:`` pin check). Implementations must tolerate this.
