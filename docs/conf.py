# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Imports -----------------------------------------------------------------

import sys
import os
import datetime
import re
import subprocess


# -- Path manipulation--------------------------------------------------------

sys.path.append(os.path.abspath("../tests"))


# -- Run Doxygen -------------------------------------------------------------

# Generate Doxygen documentation in the XML format.
subprocess.check_call("doxygen Doxyfile", shell=True)


# -- Project information -----------------------------------------------------

project = "eckit"
author = "ECMWF"

year = datetime.datetime.now().year
years = "1996-%s" % (year,)

copyright = "%s, %s" % (years, author)


def parse_version(ver_str):
    return re.sub("^((([0-9]+)\\.)+([0-9]+)).*", "\\1", ver_str)


here = os.path.abspath(os.path.dirname(__file__))

# Get the current version directly from the source.
with open(os.path.join(here, "..", "VERSION"), "r") as f:
    release = f.readline().strip()  # full version string

version = parse_version(release)  # feature version


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "pydata_sphinx_theme",
    "breathe",
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ["_templates"]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

source_suffix = ".rst"
master_doc = "index"
pygments_style = "sphinx"

highlight_language = "c++"


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = "pydata_sphinx_theme"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
#html_static_path = ["_static"]

html_context = {"css_files": ["_static/style.css"]}

# Remove links to the reST sources from the page headers.
html_show_sourcelink = False

# Remove "Created using Sphinx" from the HTML footer.
html_show_sphinx = False


# -- Breathe configuration ---------------------------------------------------
breathe_projects = {
    "ECKIT": os.path.abspath("_build/xml")  # Path to generated XML
}
breathe_default_project = "ECKIT"


