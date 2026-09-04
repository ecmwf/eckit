# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


import pytest

from eckit.geo import BoundingBox

SPECS = [
    (dict(north=90, west=0, south=-90, east=360), dict(area=[90, 0, -90, 360])),
    (
        dict(north=10, west=360 * 99 + 1, south=0, east=360 * 42 + 10),
        dict(area=[10, 1, 0, 10]),
    ),
]


@pytest.mark.parametrize("_spec, _expected", SPECS)
def test_bbox_spec(_spec, _expected):
    bbox = BoundingBox(**_spec)
    assert bbox.spec == _expected


def test_bbox_global():
    bbox = BoundingBox(north=90, west=0, south=-90, east=360)

    assert bbox.as_list() == [90, 0, -90, 360]
    assert bbox.global_
    assert not bbox.empty
    assert bbox.periodic


def test_bbox_not_global():
    bbox = BoundingBox(north=10, west=0, south=0, east=10)

    assert not bbox.global_
    assert not bbox.empty
    assert not bbox.periodic


def test_bbox_intersects_and_contains():
    outer = BoundingBox(north=10, west=0, south=-10, east=10)
    inner = BoundingBox(north=5, west=2, south=-5, east=8)
    disjoint = BoundingBox(north=20, west=20, south=15, east=30)

    assert outer.intersects(inner)
    assert outer.contains(inner)
    assert not inner.contains(outer)
    assert not outer.intersects(disjoint)
    assert not outer.contains(disjoint)


def test_bbox_contains_point():
    bbox = BoundingBox(north=10, west=0, south=-10, east=10)

    assert bbox.contains_point(5, 5)
    assert not bbox.contains_point(50, 50)


def test_bbox_area():
    bbox = BoundingBox(north=90, west=0, south=-90, east=360)

    assert bbox.area > 0
