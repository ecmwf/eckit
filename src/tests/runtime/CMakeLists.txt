# (C) Copyright 1996-2014 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

ecbuild_add_test(	TARGET  test_producer
					SOURCES test_producer.cc
					LIBS    eckit
)

ecbuild_add_test(	TARGET  test_runtime
					BOOST
					SOURCES test_context.cc
					LIBS    eckit
)

ecbuild_add_test(	TARGET test_dispatcher
					BOOST
					SOURCES  test_dispatcher.cc
					LIBS     eckit
)
