# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


def test_area():
    from eckit.geo import Area

    area = Area(dict(north=90, west=0, south=-90, east=360))
    assert area.spec == dict(area=[90, 0, -90, 360])
