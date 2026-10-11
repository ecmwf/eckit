# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from eckit.geo import Area
from eckit.geo import BoundingBox
from eckit.geo import Grid
from eckit.geo import features

grid = Grid(grid="1/1")
print(grid.shape)

# cropping: points in an area (bounding box, polygons, or spec), and the cropped grid (unprojected grids)
area = BoundingBox(north=10, west=-10, south=-10, east=10)
print(len(grid.crop(area)), grid.cropped(area).spec)

polygon = Area(type="polygon", latitudes=[0, 0, 10, 10], longitudes=[0, 10, 10, 0])
print(len(grid.crop(polygon)), polygon.spec)

# polygons of multiple rings (from coordinates or numpy arrays, or grids)
polygons = Area(
    type="polygon",
    rings=[
        dict(latitudes=[0, 0, 10, 10], longitudes=[0, 10, 10, 0]),
        Grid(latitudes=[-20, -20, -10, -10], longitudes=[100, 110, 110, 100]),
    ],
)
print(len(grid.crop(polygons)))

# shapefile areas (by name or index), downloaded and cached transparently (if supported)
if {"shapefile", "download"} <= features():
    portugal = Area(
        type="shapefile",
        url="https://gisco-services.ec.europa.eu/distribution/v2/countries/shp/CNTR_RG_60M_2024_4326.shp.zip",
        name_field="CNTR_NAME",
        name="Portugal",
    )
    print(len(Grid(grid=[0.1, 0.1]).crop(portugal)))

    # the same, from the registered shapefile libraries (see share/eckit/geo/shapefile.yaml)
    portugal_from_library = Area(type="shapefile", library="cntr-rg-60m-2024", name="Portugal")
    print(portugal_from_library.spec, portugal_from_library == portugal)  # ... True

# cropped grids: of the same type if supported (regular_ll by a bounding box), otherwise unstructured (e.g. by polygons,
# or projected grids), whose coordinates are cached and exchangeable by uid
print(grid.cropped(area).type)  # regular_ll
print(grid.cropped(polygons).type)  # unstructured_ll

rotated = Grid(grid=[1, 1], rotation=[-40, 10])
cropped = rotated.cropped(area)
print(cropped.type, len(cropped), Grid(uid=cropped.uid) == cropped)  # unstructured_ll 623 True
