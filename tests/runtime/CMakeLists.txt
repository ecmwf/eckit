# (C) Copyright 1996-2017 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

ecbuild_add_test( TARGET  eckit_test_producer
                  SOURCES test_producer.cc
                  ENVIRONMENT _TEST_ECKIT_HOME=/tmp/$ENV{USER}
                  LIBS    eckit
)

ecbuild_add_test( TARGET  eckit_test_context
                  SOURCES test_context.cc
                  LIBS    eckit
)
