// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

use eckit::Config;

const YAML: &str = "name: fdb\nport: 9000\ndatabases:\n  - class: od\n  - class: rd\n";

fn main() -> Result<(), Box<dyn std::error::Error>> {
    eckit::init();

    let path = std::env::temp_dir().join("config.yaml");
    std::fs::write(&path, YAML)?;

    // [read]
    let cfg = Config::from_path(&path)?;

    let name: String = cfg.get("name", String::new())?;
    let port: i64 = cfg.get("port", 0)?;
    println!("{name} on port {port}");

    for sub in cfg.subs(Some("databases"))? {
        let class: String = sub?.get("class", String::new())?;
        println!("database class {class}");
    }
    // [end read]

    // [parse]
    let cfg: Config = "{host: localhost, verbose: true}".parse()?;
    let verbose: bool = cfg.get("verbose", false)?;
    // [end parse]
    println!("verbose {verbose}");

    // [set]
    let mut cfg = Config::new();
    cfg.set("host", "localhost").set("port", 9000_i64);
    // [end set]
    println!("has port {}", cfg.has("port"));
    Ok(())
}
