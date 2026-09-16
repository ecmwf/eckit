# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


from json import loads

import pytest

from eckit.geo import Figure
from eckit.geo import Grid
from eckit.geo import Projection

SPECS = [
    (dict(grid="1/1"), (181, 360)),
    (dict(grid="H2", ordering="nested"), (12 * 2 * 2,)),
    (dict(grid=[2, 2]), (91, 180)),
    ("{grid: 3/3}", (61, 120)),
    (dict(grid="o2"), (sum([20, 24, 24, 20]),)),
]


@pytest.mark.parametrize("spec, shape", SPECS)
def test_grid_shape(spec, shape):
    grid = Grid(spec)
    assert shape == grid.shape


def test_grid_unstructured_ll():
    grid = Grid(latitudes=[1, 2, 3], longitudes=[4, 5, 6])

    assert grid.spec == dict(type="unstructured_ll", uid=grid.uid)
    assert grid == Grid(uid=grid.uid)

    name = "another-custom-grid"
    grid = Grid(longitudes=[4, 5, 6], latitudes=[1, 2, 3], name=name)

    assert grid == Grid(name)
    assert grid.spec == dict(grid=name)


def test_grid_to_unstructured_ll():
    grid = Grid(latitudes=[1, 2, 3], longitudes=[4, 5, 6])

    a = grid.to_unstructured_ll()
    assert a.type == "unstructured_ll"
    assert a.to_latlons() == grid.to_latlons()
    assert a.uid == grid.uid  # no name given, same points -> same uid

    name = "custom-to-unstructured-ll"
    b = grid.to_unstructured_ll(name)
    assert b.uid == grid.uid  # name doesn't change uid
    assert b == Grid(name)


def test_grid_regular_ll():
    spec = dict(
        area=[71.9834273, -25, 25, 49.9834833],
        grid=[0.0166667, 0.0166667],
        reference=[5e-05, 0.0166167],
    )
    grid = Grid(spec)
    assert grid.spec == spec


def test_grid_rotated():
    canonical = dict(
        grid=[5, 5],
        projection=dict(south_pole=[-20, -40], type="rotation"),
    )

    for spec in [
        dict(grid=[5, 5], rotation=[-20, -40]),
        canonical,
        dict(grid=[5, 5], projection=dict(south_pole_lon=-20, south_pole_lat=-40, type="rotation")),
        dict(grid=[5, 5], projection=dict(rotation=[-20, -40], type="rotation")),
    ]:
        assert Grid(spec).spec == canonical


def test_grid_rotated_default_south_pole_is_not_a_rotation():
    for spec in [
        dict(grid=[5, 5]),
        dict(grid=[5, 5], rotation=[0, -90]),
        dict(grid=[5, 5], projection=dict(south_pole=[0, -90], type="rotation")),
    ]:
        assert Grid(spec).spec == dict(grid=[5, 5])


def test_grid_rotated_south_pole_lon_lat_required_together():
    for projection in [
        dict(south_pole_lon=-20, type="rotation"),
        dict(south_pole_lat=-40, type="rotation"),
    ]:
        with pytest.raises(Exception):
            Grid(dict(grid=[5, 5], projection=projection))


def test_grid_figure():
    grid = Grid(grid="1/1")
    figure = grid.figure

    assert isinstance(figure, Figure)
    assert figure.spec_str == figure.spec_str
    assert figure.spec == loads(figure.spec_str)
    assert figure.a == figure.b  # default figure is spherical
    assert figure.spherical

    # borrowed reference stays valid once the grid goes out of scope
    del grid
    assert figure.R > 0


def test_grid_projection():
    grid = Grid(grid="1/1")
    projection = grid.projection

    assert isinstance(projection, Projection)
    assert projection.spec == loads(projection.spec_str)
    assert projection.type

    assert isinstance(projection.figure, Figure)
    assert projection.figure == grid.figure

    del grid
    assert projection.spec_str
