// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! Round trips through `MemoryStream`, pinning the `eckit::Stream` wire
//! encoding the wrapper relies on.

use eckit::{MemoryStream, Stream};

const TAG_START_OBJ: u8 = 1;
const TAG_END_OBJ: u8 = 2;
const TAG_UNSIGNED_LONG: u8 = 10;
const TAG_UNSIGNED_LONG_LONG: u8 = 12;

fn written(write: impl FnOnce(&mut dyn Stream) -> eckit::Result<()>) -> Vec<u8> {
    let mut writer = MemoryStream::writer();
    write(&mut writer).expect("write to memory stream");
    writer.buffer().expect("memory stream buffer").to_vec()
}

#[test]
fn u32_is_unsigned_long_on_the_wire() {
    eckit::init();
    let bytes = written(|s| s.write_u32(0xDEAD_BEEF));
    assert_eq!(bytes, [TAG_UNSIGNED_LONG, 0xDE, 0xAD, 0xBE, 0xEF]);

    let mut reader = MemoryStream::reader(&bytes);
    assert_eq!(reader.read_u32().expect("read u32"), 0xDEAD_BEEF);
}

#[test]
fn u64_is_unsigned_long_long_on_the_wire() {
    eckit::init();
    let bytes = written(|s| s.write_u64(0x0102_0304_0506_0708));
    assert_eq!(bytes, [TAG_UNSIGNED_LONG_LONG, 1, 2, 3, 4, 5, 6, 7, 8]);

    let mut reader = MemoryStream::reader(&bytes);
    assert_eq!(reader.read_u64().expect("read u64"), 0x0102_0304_0506_0708);
}

#[test]
fn object_framing_round_trip() {
    eckit::init();
    let bytes = written(|s| {
        s.start_object()?;
        s.write_string("FetchAgent")?;
        s.write_string("ref")?;
        s.end_object()
    });
    assert_eq!(bytes[0], TAG_START_OBJ);
    assert_eq!(bytes[bytes.len() - 1], TAG_END_OBJ);

    let mut reader = MemoryStream::reader(&bytes);
    assert!(reader.next_object().expect("next object"));
    assert_eq!(reader.read_string().expect("class name"), "FetchAgent");
    assert_eq!(reader.read_string().expect("argument"), "ref");
    assert!(
        !reader
            .next_object()
            .expect("end of stream after end object")
    );
}

#[test]
fn next_object_is_false_on_an_empty_stream() {
    eckit::init();
    let mut reader = MemoryStream::reader(&[]);
    assert!(!reader.next_object().expect("next object on empty stream"));
}

#[test]
fn next_object_rejects_a_value_where_an_object_is_expected() {
    eckit::init();
    let bytes = written(|s| s.write_i32(7));
    let mut reader = MemoryStream::reader(&bytes);
    assert!(reader.next_object().is_err());
}
