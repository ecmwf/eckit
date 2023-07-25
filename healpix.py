#!/usr/bin/env python3


import numpy as np


class Healpix:
    def __init__(self, Nside):
        assert 0 < Nside
        self._N = Nside

        pl = np.array([self.pl(i) for i in range(self.Nrow())], dtype=int)
        self._pla = np.cumsum(pl)

    def Nside(self):
        return self._N

    def Nrow(self):
        return 4 * self._N - 1

    def size(self):
        return 12 * self._N * self._N

    def pl(self, i):
        assert 0 <= i < self.Nrow(), f"i={i}"
        if i >= self._N * 2:
            return self.pl(self.Nrow() - 1 - i)
        return self._N * min(4, i + 1)

    def row(self, count):
        assert 0 <= count < self.size()
        return np.searchsorted(self._pla, count, side="right")

    def latlon(self, count):
        r = self.row(count)
        dlat = 45.0 / self._N
        dlon = 180.0 / self.pl(r)

        j = count - (self._pla[r - 1], 0)[r == 0]
        lon = dlon * (2 * j + (0, 1)[r % 2])
        lat = 90.0 - dlat * (r + 1)

        return lat, lon


h = Healpix(4)

for i in range(h.size()):
    print(h.latlon(i))
