String tools
============

:cpp:class:`eckit::StringTools` is a small static utility class that
collects the ``std::string`` manipulation primitives the rest of eckit
relies on. It lives in ``eckit/utils/StringTools.h`` and is part of the
core ``eckit`` library — no extra dependency is needed to use it.

The class has a deleted default constructor: every entry point is a
``static`` member. Most operations return a new ``std::string`` and leave
their inputs untouched; the lazy streaming helper :ref:`stringtools-joinostream`
is the one exception and returns a non-owning proxy instead.

Quick reference
---------------

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Function
     - Purpose
   * - :cpp:func:`eckit::StringTools::upper`,
       :cpp:func:`eckit::StringTools::lower`
     - ASCII case conversion (returns a new string).
   * - :cpp:func:`eckit::StringTools::trim`,
       :cpp:func:`eckit::StringTools::front_trim`,
       :cpp:func:`eckit::StringTools::back_trim`
     - Strip whitespace (or a custom set of characters) from both ends,
       the front, or the back of a string.
   * - :cpp:func:`eckit::StringTools::startsWith`,
       :cpp:func:`eckit::StringTools::beginsWith`,
       :cpp:func:`eckit::StringTools::endsWith`
     - Prefix / suffix tests; ``startsWith`` and ``beginsWith`` are
       synonyms.
   * - :cpp:func:`eckit::StringTools::split`
     - Split a string on every occurrence of a delimiter character set,
       returning a ``std::vector<std::string>``.
   * - :cpp:func:`eckit::StringTools::join`
     - Concatenate a sequence of strings into a single ``std::string``,
       inserting a delimiter between consecutive elements.
   * - :cpp:func:`eckit::StringTools::joinOstream`
     - As ``join``, but lazily streamed to an existing ``std::ostream``
       without ever materialising a temporary ``std::string``.
   * - :cpp:func:`eckit::StringTools::substitute`,
       :cpp:func:`eckit::StringTools::listVariables`
     - Substitute ``{name}`` placeholders from a dictionary; or list the
       placeholders found in a template.
   * - :cpp:func:`eckit::StringTools::isQuoted`,
       :cpp:func:`eckit::StringTools::unQuote`
     - Detect and strip matching surrounding quote characters.

Case conversion and trimming
----------------------------

.. code-block:: cpp

   #include "eckit/utils/StringTools.h"

   const std::string s = "  Hello, World!  ";

   eckit::StringTools::trim(s);                  // "Hello, World!"
   eckit::StringTools::front_trim(s);            // "Hello, World!  "
   eckit::StringTools::back_trim(s);             // "  Hello, World!"

   eckit::StringTools::trim("xxxhelloxxx", "x"); // "hello"
   eckit::StringTools::lower("Hello");           // "hello"
   eckit::StringTools::upper("Hello");           // "HELLO"

The two-argument trim variants take a string of *characters to strip*,
not a substring; ``trim("0001000", "0") == "1"``.

Searching and splitting
-----------------------

.. code-block:: cpp

   eckit::StringTools::startsWith("/etc/passwd", "/etc/");  // true
   eckit::StringTools::endsWith("file.tar.gz",   ".gz");    // true

   auto parts = eckit::StringTools::split(":/", "a:b/c:d");
   // parts == { "a", "b", "c", "d" }

The delimiter passed to ``split`` is treated as a *set of characters*:
any character in the delimiter string ends the current token. Empty
tokens between consecutive delimiters are collapsed.

Joining
-------

``StringTools::join`` builds a single ``std::string`` from any iterable
container of strings:

.. code-block:: cpp

   std::vector<std::string> v{"alpha", "beta", "gamma"};

   eckit::StringTools::join(", ", v);
   // "alpha, beta, gamma"

   eckit::StringTools::join("/", v.begin() + 1, v.end());
   // "beta/gamma"

Both the iterator-pair and the container overloads are templates, so a
``std::list``, ``std::set``, or any other range-iterable container of
strings works without conversion.

.. _stringtools-joinostream:

Lazy joining for streams
------------------------

When the result of a join is going to be written to an ``std::ostream``
(such as a log channel or a ``std::ostringstream``), allocating a
temporary ``std::string`` first is wasteful.
:cpp:func:`eckit::StringTools::joinOstream` returns a small proxy object
that streams the elements one at a time, separated by the delimiter, the
moment it is itself streamed:

.. code-block:: cpp

   #include "eckit/log/Log.h"
   #include "eckit/utils/StringTools.h"

   std::vector<std::string> values{"alpha", "beta", "gamma"};

   eckit::Log::error() << "values: ["
                       << eckit::StringTools::joinOstream(values, ", ")
                       << "]" << std::endl;
   // values: [alpha, beta, gamma]

The proxy is generic over both the container type and the element type;
anything streamable with ``operator<<`` works, so numeric containers and
custom types are equally fine:

.. code-block:: cpp

   std::set<std::string> s{"gamma", "alpha", "beta"};
   std::vector<int> nums{1, 2, 3, 4};

   std::ostringstream oss;
   oss << eckit::StringTools::joinOstream(s,    "-")  << '\n'
       << eckit::StringTools::joinOstream(nums, ", ");
   // alpha-beta-gamma
   // 1, 2, 3, 4

The delimiter is a ``std::string_view``, so ``const char *``,
``std::string``, and any ``string_view``-convertible type are all
accepted with no copy.

The proxy holds *references* to the container and the delimiter, so it
must be consumed in the same full-expression in which it is created.
This is the natural ``os << joinOstream(...)`` idiom; the type is
intentionally non-copyable to make accidental misuse harder. **Do not**
store the proxy in a variable that outlives the source container.

Template substitution
---------------------

``StringTools::substitute`` performs ``{name}``-style placeholder
expansion against a ``std::map``. ``listVariables`` returns the names
that appear in such a template, in left-to-right order with duplicates
preserved:

.. code-block:: cpp

   std::map<std::string, std::string> m{
       {"user", "alice"},
       {"host", "example.org"},
   };

   eckit::StringTools::substitute("{user}@{host}", m);
   // "alice@example.org"

   eckit::StringTools::listVariables("{user}@{host}/{user}");
   // { "user", "host", "user" }

Unbalanced braces, nested braces, and references to keys missing from
the dictionary all raise an exception derived from
:cpp:class:`eckit::Exception`.

Quoting
-------

``isQuoted`` returns true when a string starts and ends with a matching
``'`` or ``"`` character; ``unQuote`` strips one such pair off:

.. code-block:: cpp

   eckit::StringTools::isQuoted("\"hello\"");  // true
   eckit::StringTools::isQuoted("'hello'");    // true
   eckit::StringTools::isQuoted("'hello\"");   // false (mismatched)
   eckit::StringTools::unQuote("\"hello\"");   // "hello"

Mismatched or absent quotes leave the input unchanged.

API reference
-------------

.. doxygenclass:: eckit::StringTools
   :members:
