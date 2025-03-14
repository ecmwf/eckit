eckit-python
============

Requires a recent version of Cython_ and Python 3::

  ECKIT_SOURCE_DIR=<path> ECKIT_BUILD_DIR=<path> python3 setup.py build_ext -i

This builds the ``eckit`` extension module in the ``build/``. The built module still requires an externally installed eckit at runtime.

.. _Cython: https://cython.org/

Alternatively, one can build using the pypi-published eckit wheel -- to do so, run `PUBLISH_TO=nowhere ./build_chain.sh`. Note you need
the `uv` tool installed, which then creates a local `venv` and installs cython, setup tools, eckit, etc. To build using devel version
of eckit, additionally set `PIP_OVERRIDE` envvar to eg `'eckitlib==1.28.5.dev0'` or `'<path-to-your-wheel>'`.
