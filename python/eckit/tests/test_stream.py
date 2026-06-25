# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


import io
import math

import pytest
from eckit.stream import Stream


def _roundtrip(write_fn, read_fn, value):
    """Write value to buffer, read it back, return result."""
    buf = io.BytesIO()
    s = Stream(buf)
    write_fn(s, value)
    buf.seek(0)
    return read_fn(Stream(buf))


@pytest.mark.parametrize("v", [0, 1, 255, 65535, 2**31 - 1])
def test_int_roundtrip(v):
    assert _roundtrip(Stream.write_int, Stream.read_int, v) == v


@pytest.mark.parametrize("v", [0, 1, 100, 2**32 - 1])
def test_unsigned_int_roundtrip(v):
    assert _roundtrip(Stream.write_unsigned_int, Stream.read_unsigned_int, v) == v


@pytest.mark.parametrize("v", [0, 1, 42, 2**32 - 1])
def test_unsigned_long_roundtrip(v):
    assert _roundtrip(Stream.write_unsigned_long, Stream.read_unsigned_long, v) == v


@pytest.mark.parametrize("v", [0, 1, 2**32 - 1])
def test_unsigned_long_long_roundtrip(v):
    # write_unsigned_long_long hard-codes 0 for the high 32 bits — values > 2**32-1 not supported
    assert (
        _roundtrip(Stream.write_unsigned_long_long, Stream.read_unsigned_long_long, v)
        == v
    )


@pytest.mark.parametrize(
    "v",
    [
        0.0,
        1.0,
        -1.0,
        3.14159265358979323846,
        -2.718281828459045,
        1e-300,
        1e300,
        float("inf"),
        float("-inf"),
        1.1754943508222875e-38,  # min normal float32
    ],
)
def test_double_roundtrip(v):
    result = _roundtrip(Stream.write_double, Stream.read_double, v)
    if math.isnan(v):
        assert math.isnan(result)
    elif math.isinf(v):
        assert math.isinf(result) and math.copysign(1, result) == math.copysign(1, v)
    else:
        assert result == v


def test_double_nan_roundtrip():
    nan = float("nan")
    result = _roundtrip(Stream.write_double, Stream.read_double, nan)
    assert math.isnan(result)


@pytest.mark.parametrize("v", [b"", b"hello", b"eckit stream test", b"\x00\x01\x02"])
def test_string_roundtrip(v):
    assert _roundtrip(Stream.write_string, Stream.read_string, v) == v


@pytest.mark.parametrize(
    "data",
    [
        b"",
        b"\x00" * 1,
        bytes(range(256)),
        b"binary\x00data\xff\xfe",
        b"x" * 65536,
    ],
)
def test_large_blob_roundtrip(data):
    assert _roundtrip(Stream.write_large_blob, Stream.read_large_blob, data) == data


def test_multiple_values_sequential():
    buf = io.BytesIO()
    s = Stream(buf)
    s.write_unsigned_long(42)
    s.write_double(3.14)
    s.write_large_blob(b"hello")
    s.write_unsigned_long(99)

    buf.seek(0)
    s2 = Stream(buf)
    assert s2.read_unsigned_long() == 42
    assert s2.read_double() == pytest.approx(3.14)
    assert s2.read_large_blob() == b"hello"
    assert s2.read_unsigned_long() == 99


def test_end_object_skipped_between_reads():
    buf = io.BytesIO()
    s = Stream(buf)
    s.write_unsigned_long(10)
    s.end_object()  # TAG_END_OBJ interleaved
    s.write_unsigned_long(20)

    buf.seek(0)
    s2 = Stream(buf)
    assert s2.read_unsigned_long() == 10
    assert s2.read_unsigned_long() == 20  # TAG_END_OBJ skipped transparently


def test_wrong_tag_raises():
    buf = io.BytesIO()
    s = Stream(buf)
    s.write_unsigned_long(1)

    buf.seek(0)
    with pytest.raises(RuntimeError, match="Unexpected tag"):
        Stream(buf).read_int()  # expects TAG_INT, gets TAG_UNSIGNED_LONG


def test_read_long_long_not_implemented():
    buf = io.BytesIO()
    s = Stream(buf)
    s.write_long_long(1)
    buf.seek(0)
    with pytest.raises(NotImplementedError):
        Stream(buf).read_long_long()
