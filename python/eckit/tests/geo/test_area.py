# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import pytest

from eckit.geo import Area


@pytest.mark.parametrize(
    "spec, type_, expected",
    [
        (
            dict(north=90, west=0, south=-90, east=360),
            "bounding_box",
            dict(area=[90, 0, -90, 360]),
        ),
        (dict(area=[10, 1, 0, 10]), "bounding_box", dict(area=[10, 1, 0, 10])),
        (
            dict(type="bounding_box_xy", bounding_box_xy=[0, 0, 1, 1]),
            "bounding_box_xy",
            dict(type="bounding_box_xy", bounding_box_xy=[0, 0, 1, 1]),
        ),
    ],
)
def test_area_spec(spec, type_, expected):
    area = Area(spec)
    assert area.type == type_
    assert area.spec == expected
    assert Area(area.spec) == area


def test_area_equality():
    # equality is by spec, however the area is described
    assert Area(north=90, west=0, south=-90, east=360) == Area(area=[90, 0, -90, 360])
    assert Area(area=[10, 1, 0, 10]) != Area(area=[10, 0, 0, 10])
