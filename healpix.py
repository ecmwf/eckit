#!/usr/bin/env python3


import numpy as np


class HEALPix:
    def __init__(self, Nside):
        assert 0 < Nside
        self._N = Nside

        pl = np.array([self.pl(i) for i in range(self.Nrow())], dtype=int)
        self._pla = np.cumsum(pl)

    def Nside(self):
        return self._N

    def Nrow(self):
        return 4 * self._N - 1

    def __len__(self):
        return 12 * self._N * self._N

    def pl(self, i):
        assert 0 <= i < self.Nrow(), f"i={i}"
        if i >= self._N * 2:
            return self.pl(self.Nrow() - 1 - i)
        return self._N * min(4, i + 1)

    def latlon(self, count):
        assert 0 <= count < len(self)

        i = np.searchsorted(self._pla, count, side="right")
        j = count if i == 0 else count - self._pla[i - 1]
        dlat = 45.0 / self._N
        dlon = 180.0 / self.pl(i)

        lon = dlon * (2 * j + (0, 1)[i % 2])
        lat = 90.0 - dlat * (i + 1)

        return lat, lon


h = HEALPix(4)

for i in range(len(h)):
    print(h.latlon(i))
