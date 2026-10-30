// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

//! C++ exceptions reach Rust as the matching `Error` variant.

use eckit::{DataHandle, Error};

#[test]
fn missing_file_is_cant_open_file() {
    eckit::init();
    let Err(err) = DataHandle::from_path("/nonexistent/eckit-rust-test.grib")
        .and_then(DataHandle::open_for_read)
    else {
        panic!("opening a missing file succeeded");
    };
    assert!(matches!(err, Error::CantOpenFile(_)), "got {err:?}");
}

#[cfg(feature = "geo")]
#[test]
fn invalid_grid_spec_is_typed() {
    eckit::init();
    let err = eckit::Grid::from_spec(r#"{"grid":"no-such-grid"}"#).expect_err("unknown grid fails");
    assert!(!matches!(err, Error::Other(_)), "got {err:?}");
}
