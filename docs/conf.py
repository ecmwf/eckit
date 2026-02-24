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

project = "EcKit"
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
    "sphinx.ext.autosectionlabel",
    "sphinxcontrib.mermaid",
    "sphinx.ext.viewcode",
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
html_theme = "pydata_sphinx_theme"
# Do not show source link on each page
html_show_sourcelink = False
html_sidebars = {
    "**": [],
}
html_theme_options = {
    "navbar_align": "left",
    "navbar_start": ["navbar-logo"],
    "navbar_center": ["navbar-nav"],
    "navbar_end": ["navbar-icon-links", "theme-switcher", "version-switcher"],
    "navbar_persistent": ["search-button"],
    "primary_sidebar_end": [],
    # On local builds no version.json is present
    "check_switcher": False,
    "logo": {"text": "EcKit"},
}

html_context = {
    # Enable auto detection of light/dark mode
    "default_mode": "auto"
}

# -- Breathe configuration ---------------------------------------------------
breathe_projects = {
    "ECKIT": os.path.abspath("_build/xml")  # Path to generated XML
}
breathe_default_project = "ECKIT"
