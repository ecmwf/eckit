# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


import array

import pytest
from eckit.geo._eckit_geo import _spec_str

from eckit.geo import Area
from eckit.geo import Figure
from eckit.geo import Grid

np = pytest.importorskip("numpy")

LATS = [10.0, 20.0, 60.0]
LONS = [0.0, 10.0, 20.0]


def roundtrip(value):
    """Value as seen by eckit::geo, after spec serialisation and back."""
    from yaml import safe_load

    return safe_load(_spec_str(dict(value=value)))["value"]


def test_spec_str_scalars():
    assert roundtrip(np.float64(1.5)) == 1.5
    assert roundtrip(np.int64(2)) == 2
    assert roundtrip(np.bool_(True)) is True
    assert roundtrip("grid") == "grid"
    assert roundtrip(b"grid") == "grid"
    assert roundtrip(3.5) == 3.5


def test_spec_str_sequences():
    assert roundtrip(np.array([1.0, 2.0])) == [1.0, 2.0]
    assert roundtrip(np.array([[1, 2], [3, 4]])) == [[1, 2], [3, 4]]
    assert roundtrip(array.array("d", [1.0, 2.0])) == [1.0, 2.0]
    assert roundtrip((1, 2)) == [1, 2]
    assert roundtrip(range(3)) == [0, 1, 2]


def test_spec_str_nested_dict():
    spec = dict(grid=np.array([2.0, 2.0]), area=(90, 0, -90, 360))
    assert roundtrip(spec) == dict(grid=[2.0, 2.0], area=[90, 0, -90, 360])


def test_spec_str_no_python_tags():
    # numpy values must not serialise as YAML !!python/... tags, which
    # eckit::geo's parser cannot read
    s = _spec_str(dict(latitudes=np.array(LATS), longitudes=np.array(LONS)))
    assert "!!python" not in s
    assert "numpy" not in s


def test_grid_unstructured_ll_numpy_arrays():
    reference = Grid(latitudes=LATS, longitudes=LONS)

    grid = Grid(latitudes=np.array(LATS), longitudes=np.array(LONS))
    assert grid.type == "unstructured_ll"
    assert grid.uid == reference.uid
    assert grid.to_latlons() == (LATS, LONS)


def test_grid_unstructured_ll_numpy_dict_spec():
    grid = Grid(
        {
            "type": "unstructured_ll",
            "latitudes": np.array(LATS),
            "longitudes": np.array(LONS),
        }
    )
    assert grid.uid == Grid(latitudes=LATS, longitudes=LONS).uid


@pytest.mark.parametrize("dtype", ["float64", "float32", "int32", "int64"])
def test_grid_unstructured_ll_numpy_dtypes(dtype):
    grid = Grid(
        latitudes=np.array(LATS, dtype=dtype),
        longitudes=np.array(LONS, dtype=dtype),
    )
    assert grid.to_latlons() == (LATS, LONS)
    assert grid.uid == Grid(latitudes=LATS, longitudes=LONS).uid


def test_grid_numpy_array_value():
    assert Grid(grid=np.array([2.0, 2.0])).shape == Grid(grid=[2, 2]).shape


def test_grid_numpy_scalars_and_tuples():
    spec = dict(
        area=[71.9834273, -25, 25, 49.9834833],
        grid=[0.0166667, 0.0166667],
        reference=[5e-05, 0.0166167],
    )
    grid = Grid(
        area=np.array(spec["area"]),
        grid=tuple(spec["grid"]),
        reference=[np.float64(spec["reference"][0]), spec["reference"][1]],
    )
    assert grid.spec == spec


def test_area_numpy_scalars():
    area = Area(
        dict(north=np.float64(90), west=np.int64(0), south=np.float64(-90), east=360)
    )
    assert area.spec == dict(area=[90, 0, -90, 360])


def test_figure_numpy_scalar():
    R = 6371229.0
    assert Figure(R=np.float64(R)).spec == Figure(R=R).spec
