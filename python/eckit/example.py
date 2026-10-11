# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from eckit.geo import Area
from eckit.geo import BoundingBox
from eckit.geo import Grid

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

# cropped grids: of the same type if supported (regular_ll by a bounding box), otherwise unstructured (e.g. by polygons,
# or projected grids), whose coordinates are cached and exchangeable by uid
print(grid.cropped(area).type)  # regular_ll
print(grid.cropped(polygons).type)  # unstructured_ll

rotated = Grid(grid=[1, 1], rotation=[-40, 10])
cropped = rotated.cropped(area)
print(cropped.type, len(cropped), Grid(uid=cropped.uid) == cropped)  # unstructured_ll 623 True
