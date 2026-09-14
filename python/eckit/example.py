# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from eckit.geo import Grid

grid = Grid(grid="1/1")
print(grid.shape)
