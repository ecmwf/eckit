// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use eckit::{MemoryStream, Stream, StreamRead, StreamWrite};

// [types]
struct Request {
    verb: String,
    count: i64,
}

impl StreamWrite for Request {
    fn write_to(&self, stream: &mut dyn Stream) -> eckit::Result<()> {
        stream.write_string(&self.verb)?;
        stream.write_i64(self.count)
    }
}

impl StreamRead for Request {
    fn read_from(stream: &mut dyn Stream) -> eckit::Result<Self> {
        Ok(Self {
            verb: stream.read_string()?,
            count: stream.read_i64()?,
        })
    }
}
// [end types]

fn main() -> eckit::Result<()> {
    eckit::init();

    // [roundtrip]
    let mut out = MemoryStream::writer();
    out.write_string("retrieve")?;
    out.write_i64(42)?;
    let bytes = out.buffer()?.to_vec();

    let mut input = MemoryStream::reader(&bytes);
    assert_eq!(input.read_string()?, "retrieve");
    assert_eq!(input.read_i64()?, 42);
    // [end roundtrip]

    // [typed]
    let request = Request {
        verb: "archive".to_string(),
        count: 3,
    };

    let mut out = MemoryStream::writer();
    request.write_to(&mut out)?;
    let bytes = out.buffer()?.to_vec();

    let mut input = MemoryStream::reader(&bytes);
    let back = Request::read_from(&mut input)?;
    // [end typed]
    println!("{} {}", back.verb, back.count);

    let path = std::env::temp_dir().join("request.stream");
    // [file]
    std::fs::write(&path, out.buffer()?).map_err(|e| eckit::Error::Other(e.to_string()))?;

    let bytes = std::fs::read(&path).map_err(|e| eckit::Error::Other(e.to_string()))?;
    let from_file = Request::read_from(&mut MemoryStream::reader(&bytes))?;
    // [end file]
    println!("{} {}", from_file.verb, from_file.count);
    Ok(())
}
