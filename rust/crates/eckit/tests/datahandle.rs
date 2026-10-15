// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! `DataHandle` adapters: `from_writer`.

use std::io::Write;
use std::sync::{Arc, Mutex};

use eckit::DataHandle;

#[derive(Clone)]
struct SharedSink(Arc<Mutex<Vec<u8>>>);

impl Write for SharedSink {
    fn write(&mut self, buf: &[u8]) -> std::io::Result<usize> {
        self.0.lock().expect("sink lock").extend_from_slice(buf);
        Ok(buf.len())
    }

    fn flush(&mut self) -> std::io::Result<()> {
        Ok(())
    }
}

struct FailingSink;

impl Write for FailingSink {
    fn write(&mut self, _buf: &[u8]) -> std::io::Result<usize> {
        Err(std::io::Error::other("sink failure"))
    }

    fn flush(&mut self) -> std::io::Result<()> {
        Ok(())
    }
}

#[test]
fn from_writer_streams_to_rust_sink() -> eckit::Result<()> {
    let sink = SharedSink(Arc::new(Mutex::new(Vec::new())));
    let bytes = sink.0.clone();

    let handle = DataHandle::from_writer(sink)?;
    assert!(!handle.can_seek()?);

    let mut handle = handle.open_for_write(0)?;
    handle.write_all(b"hello ").expect("write");
    handle.write_all(b"world").expect("write");
    handle.close()?;

    assert_eq!(bytes.lock().expect("sink lock").as_slice(), b"hello world");
    Ok(())
}

#[test]
fn from_writer_surfaces_sink_errors() -> eckit::Result<()> {
    let mut handle = DataHandle::from_writer(FailingSink)?.open_for_write(0)?;
    assert!(handle.write_all(b"data").is_err());
    Ok(())
}

#[test]
fn from_writer_surfaces_sink_panics_as_errors() -> eckit::Result<()> {
    struct PanickingSink;

    impl Write for PanickingSink {
        fn write(&mut self, _buf: &[u8]) -> std::io::Result<usize> {
            panic!("sink panic");
        }

        fn flush(&mut self) -> std::io::Result<()> {
            Ok(())
        }
    }

    let mut handle = DataHandle::from_writer(PanickingSink)?.open_for_write(0)?;
    assert!(handle.write_all(b"data").is_err());
    Ok(())
}
