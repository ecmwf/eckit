# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


def test_area():
    from eckit.geo import Area

    area = Area(dict(north=90, west=0, south=-90, east=360))
    assert area.spec == dict(area=[90, 0, -90, 360])
