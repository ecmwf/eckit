#!/usr/bin/env python3

# Requirements:
#   Python >= 3.6
#   PyYAML >= 6.0.1
#   Requests >= 2.31.0
#
# Install dependencies with pip:
#   pip install pyyaml requests


import argparse
import logging
import os
from pathlib import Path
from urllib.parse import urlparse

import requests
import yaml

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")


# TODO: configuration options control the cache path, available grids, but they're not considered here
CONFIG_PATH = (
    Path(os.path.abspath(__file__)).parents[2] / "eckit" / "share" / "eckit" / "geo" / "ORCA.yaml"
)
CACHE_PATH = Path(
    os.getenv(
        "ECKIT_GEO_CACHE_PATH", Path.home() / ".local" / "share" / "eckit" / "geo"
    )
)
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

    # Build lookup dict from grid_names
    # Each entry is a flat dict where the grid name key has None value (due to YAML anchor)
    # and the rest of the keys are grid properties
    grids_by_name = {}
    for entry in config.get("grid_names", []):
        if isinstance(entry, dict):
            # Find the grid name (key with None value, which is the anchor)
            grid_name = None
            for key, value in entry.items():
                if value is None:
                    grid_name = key
                    break
            if grid_name:
                # The grid info is all the other keys in the entry
                grid_info = {k: v for k, v in entry.items() if k != grid_name}
                grids_by_name[grid_name] = grid_info

    # Handle --list-grids
    if args.list_grids:
        print("Known grid names:")
        for name in sorted(grids_by_name.keys()):
            grid = grids_by_name[name]
            uid = grid.get("orca_uid", "")
            print(f"  {name}" + (f" (uid: {uid})" if uid else ""))
        return

    # Determine which grids to process
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


if __name__ == "__main__":
    p = argparse.ArgumentParser(description="Create binary grid data files")
    p.add_argument(
        "--list-grids",
        action="store_true",
        help="List all known grid names and UIDs, then exit."
    )
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
