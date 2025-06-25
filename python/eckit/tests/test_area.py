# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


import pytest

SPECS = [
    (dict(north=90, west=0, south=-90, east=360), dict()),
    (dict(north=10, west=360*99+1, south=0, east=360*42+10), dict(north=10, west=1, south=0, east=10)),
]


# @pytest.mark.parametrize( "_spec, _expected", SPECS)
@pytest.mark.skip(reason="no way of currently testing this")
def test_area(_spec, _expected):
    from eckit.geo import Area

    area = Area(_spec)
    assert area.spec == _expected
