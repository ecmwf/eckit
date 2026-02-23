#!/usr/bin/env python3

import os
import signal
from flask import Flask, request, jsonify

"""

MockREST provides a simple REST API over HTTP to support tests.

The API requires a Python environment with `flask` installed to execute -- see requirements.txt.

The API consists of the following end-points:

 * /ping [GET]
    - always replies "Hi!" (200)
 * /insufficient-permissions [GET]
    - always replies "Forbidden" (403)
 * /shutdown [GET]
    - shuts down the running server
 * /blobs [GET]
    - replies with the list of currently available blobs (in JSON)

 * /blob/<key> [GET]
    - replies with the blob associated with <key> (in JSON) (200)
    - if <key> not found, replies with "Not found" (404)
 
 * /blob [POST]
    - creates a new blob, based on payload '{"key": "<key>", "value": "<value>"}'
    - replies with "Request must be JSON" (415) is payload is not JSON
    - replies with "Bad Request" (400) if payload doesn't include "key" or "value"
    - replies with "Bad Request" (400) if a blob is already associated with <key>
    - replies with "Created" (201), in case of success

 * /blob [PUT]
    - updates an existing blob, based on payload '{"key": "<key>", "value": "<value>"}'
    - replies with "Request must be JSON" (415) is payload is not JSON
    - replies with "Bad Request" (400) if payload doesn't include "key" or "value"
    - replies with "Bad Request" (400) if no blob is associated with <key>
    - replies with "OK" (200), in case of success
 
 * /blob/<key> [DELETE]
    - deletes an existing blob, associated with <key>
    - if <key> not found, replies with "Not found" (404)
    - replies with "" (204), in case of success
 
 * /blob/<key>/content [GET]
    - replies with the blob value (in Plain Text) (200)
    - if <key> not found, replies with "Not found" (404)

"""

app = Flask(__name__)


blobs = {}


def _blob_has_all_of(blob, fields):
    for field in fields:
        if field not in blob:
            return False, field
    return True, None


@app.get("/ping")
def ping():
    return "Hi!", 200


@app.get("/insufficient-permissions")
def no_permissions():
    return "Forbidden", 403


@app.get("/shutdown")
def shutdown():
    # terminate own running process!...
    os.kill(os.getpid(), signal.SIGINT)
    return jsonify(blobs)


@app.get("/blobs")
def get_blobs():
    return jsonify(blobs)


@app.get("/blob/<key>")
def get_blob(key):
    if key not in blobs:
        return {"error": f"Not Found: Unable to find blob with key '{key}'"}, 404
    blob = {"key": key, "value": blobs[key]}
    return jsonify(blob)


@app.get("/blob/<key>/content")
def get_blob_content(key):
    if key not in blobs:
        return {"error": f"Not Found: Unable to find blob with key '{key}'"}, 404
    return blobs[key]


@app.post("/blob")
def create_blob():
    if not request.is_json:
        return {"error": "Request must be JSON"}, 415

    blob = request.get_json()

    found, field = _blob_has_all_of(blob, ["key", "value"])
    if not found:
        return {"error": f"Bad Request: New blob must contain field '{field}'"}, 400

    key = blob["key"]
    if key in blobs:
        return {"error": f"Bad Request: Key {blob['key']} already exists"}, 400

    value = blob["value"]
    blobs[key] = value
    return f"Created: {blob}", 201


@app.put("/blob")
def update_blob():
    if not request.is_json:
        return {"error": "Request must be JSON"}, 415

    blob = request.get_json()

    found, field = _blob_has_all_of(blob, ["key", "value"])
    if not found:
        return {"error": f"Bad Request: Update blob must contain field '{field}'"}, 400

    key = blob["key"]
    if key not in blobs:
        return {"error": f"Bad Request: Key {blob['key']} does not exist"}, 400

    value = blob["value"]
    blobs[key] = value
    return f"OK: {blob}", 200


@app.delete("/blob/<key>")
def delete_blob(key):
    if key not in blobs:
        return {"error": f"Not Found (Unable to find blob with key '{key}')"}, 404
    del blobs[key]
    return "", 204
