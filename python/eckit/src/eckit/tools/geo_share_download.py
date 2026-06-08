#!/usr/bin/env python3

# (C) Copyright 2026- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

import argparse
import logging
import os
from pathlib import Path
from urllib.parse import urlparse

import requests
import yaml

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")

CHUNK_SIZE = 32768


def _default_config_path() -> Path:
    if env := os.getenv("ECKIT_GEO_CONFIG_PATH"):
        return Path(env)
    try:
        import eckit
        return Path(eckit.home()) / "share" / "eckit" / "geo" / "ORCA.yaml"
    except Exception:
        return Path("/usr/local/share/eckit/geo/ORCA.yaml")


CACHE_PATH = Path(os.getenv("ECKIT_GEO_CACHE_PATH", Path.home() / ".local" / "share" / "eckit" / "geo"))


def pretty_bytes(num: float, suffix="B"):
    for unit in ["", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi"]:
        if abs(num) < 1024.0:
            return f"{num:3.1f} {unit}{suffix}"
        num /= 1024.0
    return f"{num:.1f} Yi{suffix}"


def download_file(url: str, path: Path):
    try:
        logging.info(f"Downloading '{url}'...")
        with requests.get(url, stream=True, timeout=60) as r:
            r.raise_for_status()

            path.parent.mkdir(parents=True, exist_ok=True)
            bytes = 0
            with open(path, "wb") as f:
                for chunk in r.iter_content(chunk_size=CHUNK_SIZE):
                    bytes += f.write(chunk)

            logging.info(f"Downloaded '{path}' ({pretty_bytes(bytes)}).")
            if "Content-Length" in r.headers:
                assert bytes == int(r.headers["Content-Length"])
        return True

    except Exception as e:
        logging.error(f"Error downloading from '{url}' to '{path}': {e}")
        return False


def is_url(string):
    try:
        result = urlparse(string)
        return all([result.scheme, result.netloc])
    except ValueError:
        return False


def path_from_url(url: str, cache_path: Path):
    path_parts = urlparse(url).path.strip("/").split("/")
    if path_parts and path_parts[0] == "repository":
        path_parts.remove("repository")
    return cache_path.joinpath(*path_parts)


def execute(args):
    with open(args.config, "r") as file:
        config = yaml.safe_load(file)

    grids_by_name = {}
    for entry in config.get("grid_names", []):
        if isinstance(entry, dict):
            grid_name = None
            for key, value in entry.items():
                if value is None:
                    grid_name = key
                    break
            if grid_name:
                grid_info = {k: v for k, v in entry.items() if k != grid_name}
                grids_by_name[grid_name] = grid_info

    if args.list_grids:
        print("Known grid names:")
        for name in sorted(grids_by_name.keys()):
            grid = grids_by_name[name]
            uid = grid.get("uid", "")
            print(f"  {name}" + (f" (uid: {uid})" if uid else ""))
        return

    if args.grid == ["all"]:
        grids_to_process = list(grids_by_name.values())
    else:
        grids_to_process = []
        for key in args.grid:
            grid = grids_by_name.get(key)
            if not grid:
                logging.error(f"Grid '{key}' is not a known grid name")
                continue
            grids_to_process.append(grid)

    urls = set()
    for grid in grids_to_process:
        url = grid.get("url")
        if url and is_url(url):
            urls.add(url)

    for url in sorted(urls):
        path = path_from_url(url, args.cache_path)
        if path.exists() and not args.overwrite:
            logging.info(f"File already exists: '{path}'")
            continue

        if download_file(url, path):
            assert path.exists()


def main():
    config_path = _default_config_path()
    p = argparse.ArgumentParser(description="Download eckit-geo share files")
    p.add_argument(
        "--list-grids",
        action="store_true",
        help="List all known grid names and UIDs, then exit.",
    )
    p.add_argument(
        "--config",
        default=config_path,
        help=f"Path to YAML configuration file (default: {config_path})",
    )
    p.add_argument(
        "--cache-path",
        default=CACHE_PATH,
        help=f"Path to cache directory (default: {CACHE_PATH})",
    )
    p.add_argument("--grid", default=["all"], nargs="+", help="Grid to cache (default: all)")
    p.add_argument("--overwrite", action="store_true", help="Overwrite existing files")

    execute(p.parse_args())


if __name__ == "__main__":
    main()
