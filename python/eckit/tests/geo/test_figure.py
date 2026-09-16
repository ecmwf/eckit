# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


from math import pi
from math import sqrt

import pytest

from eckit.geo import BoundingBox
from eckit.geo import Figure
from eckit.geo import Grid

GLOBE = BoundingBox(north=90, west=-180, south=-90, east=180)
NORTH = BoundingBox(north=90, west=-180, south=0, east=180)
SOUTH = BoundingBox(north=0, west=-180, south=-90, east=180)

EARTH_RADIUS = 6371229.0  # [m] from eckit::geo::figure::DatumIFS::radius
EARTH_AREA = 510101140207791.56  # [m^2]


def test_figure_sphere():
    f1 = Figure(R=1)
    f2 = Figure(a=1, b=1)

    assert f1 == f2
    assert f1.spec_str == '{"r":1}'
    assert f1.spec == dict(r=1)

    assert f1.R == 1
    assert f1.a == f1.R
    assert f1.b == f1.R
    assert f1.spherical
    assert f1.eccentricity == pytest.approx(0.0)
    assert f1.flattening == pytest.approx(0.0)

    assert f1.area() == pytest.approx(4.0 * pi)
    assert f1.area(GLOBE) == pytest.approx(f1.area())
    assert f1.area(NORTH) == pytest.approx(0.5 * f1.area())
    assert f1.area(NORTH) == pytest.approx(f1.area(SOUTH))

    with pytest.raises(Exception):
        Figure(R=-1)  # negative radius


def test_figure_sphere_area_scales_with_radius():
    unit = Figure(R=1)
    two = Figure(R=2)

    assert two.R == 2
    assert two.area() == pytest.approx(4.0 * unit.area())
    assert two.area(NORTH) == pytest.approx(4.0 * unit.area(NORTH))


def test_figure_oblate_spheroid():
    figure = Figure(a=1, b=0.5)

    assert figure.spec_str == '{"a":1,"b":0.5}'
    assert figure.spec == dict(a=1, b=0.5)

    assert figure.a == 1
    assert figure.b == 0.5
    assert not figure.spherical
    assert figure.eccentricity == pytest.approx(sqrt(1 - 0.25))
    assert figure.flattening == pytest.approx(0.5)

    with pytest.raises(Exception):
        figure.R  # not defined for a != b

    with pytest.raises(Exception):
        Figure(a=0.5, b=1)  # prolate spheroid


def test_figure_earth():
    figure = Figure(figure="earth")

    assert figure == Figure(R=EARTH_RADIUS)
    assert figure.spec_str == '{"r":6371229}'
    assert figure.spec == dict(r=EARTH_RADIUS)

    assert figure.R == EARTH_RADIUS
    assert figure.a == figure.R
    assert figure.b == figure.R
    assert figure.spherical
    assert figure.eccentricity == pytest.approx(0.0)
    assert figure.flattening == pytest.approx(0.0)

    assert figure.area() == pytest.approx(EARTH_AREA)
    assert figure.area(GLOBE) == pytest.approx(EARTH_AREA)
    assert figure.area(NORTH) == pytest.approx(EARTH_AREA / 2)
    assert figure.area(NORTH) == pytest.approx(figure.area(SOUTH))


def test_figure_wgs84():
    A = 6378137.0  # [m] from eckit::geo::figure::DatumWgs84
    B = 6356752.314245
    AREA = 510065621724079.0
    INVERSE_FLATTENING = 298.257223563

    figure = Figure(figure="wgs84")

    assert figure == Figure(a=A, b=B)
    assert figure.spec_str == '{"figure":"wgs84"}'
    assert figure.spec == dict(figure="wgs84")

    assert figure.a == pytest.approx(A)
    assert figure.b == pytest.approx(B)
    assert not figure.spherical
    assert 0 < figure.eccentricity < 1
    assert figure.flattening == pytest.approx(1 - figure.b / figure.a)
    assert 1 / figure.flattening == pytest.approx(INVERSE_FLATTENING)

    with pytest.raises(Exception):
        figure.R  # not defined for a != b

    assert figure.area() == pytest.approx(AREA)
    assert figure.area(GLOBE) == pytest.approx(AREA)
    assert figure.area(NORTH) == pytest.approx(AREA / 2)
    assert figure.area(NORTH) == pytest.approx(figure.area(SOUTH))


@pytest.mark.parametrize("spec", ["O320", "N320", "1/1"])
def test_grid_default_figure_is_spherical_earth(spec):
    figure = Grid(spec).figure

    assert isinstance(figure, Figure)
    assert figure == Figure(figure="earth")

    assert figure.spherical
    assert figure.R == EARTH_RADIUS
    assert figure.a == EARTH_RADIUS
    assert figure.b == EARTH_RADIUS
    assert figure.eccentricity == pytest.approx(0.0)
    assert figure.flattening == pytest.approx(0.0)
    assert figure.spec == dict(r=EARTH_RADIUS)
