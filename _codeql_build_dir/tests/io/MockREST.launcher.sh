#!/usr/bin/env bash

set -ex

BASE_DIR="$( cd "$( dirname "$0" )" >/dev/null 2>&1 && pwd )"

export FLASK_APP=${BASE_DIR}/MockREST.py
flask run -h localhost -p 49111
