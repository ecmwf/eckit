eckit-python
============

Requires a recent version of Cython_ and Python 3::

  ECKIT_SOURCE_DIR=<path> ECKIT_BUILD_DIR=<path> python3 setup.py build_ext -i

This builds the ``eckit`` extension module in the ``build/``. The built module still requires an externally installed eckit at runtime.

.. _Cython: https://cython.org/
