# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import pytest

from eckit.geo import BoundingBox
from eckit.geo import Figure


def test_figure_earth():
    R = 6371229.0
    A = 510101140207791.56

    figure = Figure(R=R)

    assert figure.R == R
    assert figure.a == figure.R
    assert figure.b == figure.R
    assert figure.spherical
    assert figure.eccentricity == 0
    assert figure.flattening == 0
    assert figure.spec == dict(r=R)

    assert A == pytest.approx(figure.area())
    assert A == pytest.approx(
        figure.area(BoundingBox(north=90, west=0, south=-90, east=360))
    )
    assert A / 2 == pytest.approx(
        figure.area(BoundingBox(north=90, west=0, south=0, east=360))
    )


def test_figure_wgs84():
    A = 510065621724079.0

    figure = Figure(figure="wgs84")

    with pytest.raises(Exception):
        figure.R  # not defined for a != b

    assert figure.a == pytest.approx(6378137.0)
    assert figure.b == pytest.approx(6356752.314245)
    assert not figure.spherical
    assert 0 < figure.eccentricity < 1
    assert figure.flattening == pytest.approx(1 - figure.b / figure.a)
    assert figure.spec == dict(figure="wgs84")

    assert A == pytest.approx(figure.area())
    assert A == pytest.approx(
        figure.area(BoundingBox(north=90, west=0, south=-90, east=360))
    )
    assert A / 2 == pytest.approx(
        figure.area(BoundingBox(north=90, west=0, south=0, east=360))
    )
