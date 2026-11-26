#!/usr/bin/env bash

# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

set -ex

BASE_DIR="$( cd "$( dirname "$0" )" >/dev/null 2>&1 && pwd )"

export FLASK_APP=${BASE_DIR}/MockREST.py
flask run -h localhost -p 49111
