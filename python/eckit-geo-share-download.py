#!/usr/bin/env python3

# Python>=3.6
# PyYAML>=6.0.1
# Requests>=2.31.0


import argparse
import logging
import os
from pathlib import Path
from urllib.parse import urlparse

import requests
import yaml

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")


CONFIG_PATH = (
    Path(os.path.abspath(__file__)).parents[2]
    / "share"
    / "atlas"
    / "grids"
    / "orca"
    / "grids.yaml"
)
CACHE_PATH = Path(os.getenv("ATLAS_CACHE_PATH", Path("/") / "tmp" / "cache"))
CHUNK_SIZE = 32768


def pretty_bytes(num: float, suffix="B"):
    """Convert bytes to a more readable format."""
    for unit in ["", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei", "Zi"]:
        if abs(num) < 1024.0:
            return f"{num:3.1f} {unit}{suffix}"
        num /= 1024.0
    return f"{num:.1f} Yi{suffix}"


def download_file(url: str, path: Path):
    try:
        logging.info(f"Downloading '{url}'...")
        with requests.get(url, stream=True, timeout=60) as r:
            r.raise_for_status()  # Verify status code

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

    urls = set()
    for grid in config.keys() if args.grid == ["all"] else args.grid:
        if grid not in config:
            logging.error(f"Grid {grid} is not a known grid")
            continue
        if "data" in config[grid] and is_url(config[grid]["data"]):
            urls.add(config[grid]["data"])

    for url in sorted(urls):
        path = path_from_url(url, args.cache_path)
        if path.exists() and not args.overwrite:
            logging.info(f"File already exists: '{path}'")
            continue

        if download_file(url, path):
            assert path.exists()


if __name__ == "__main__":
    p = argparse.ArgumentParser(description="Create binary grid data files")
    p.add_argument(
        "--config",
        default=CONFIG_PATH,
        help=f"Path to the YAML configuration file (default: {CONFIG_PATH})",
    )
    p.add_argument(
        "--cache-path",
        default=CACHE_PATH,
        help=f"Path to the cache directory (default: {CACHE_PATH})",
    )
    p.add_argument(
        "--grid", default=["all"], nargs="+", help="Grid to cache (default: all)"
    )
    p.add_argument("--overwrite", action="store_true", help="Overwrite existing files")

    execute(p.parse_args())
