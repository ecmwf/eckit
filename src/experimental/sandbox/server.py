from __future__ import print_function

import socket
import sys
import json

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)


def trigger_event(message):

    if not len(message):
        return

    event = json.loads(message)

    step = event['step']
    app = event['app']

    eprint('Received event from app {app} step {step}'.format(**event))


def handle_message(connection):

    try:
        msg = ""
        while True:
            data = connection.recv(1024)
            eprint('received "%s"' % data)
            if data:
                msg += data
            else:
                break

    finally:
        connection.close()
        trigger_event(data)


def run_server():

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server_name = 'localhost'  # sys.argv[1]
    server_address = (server_name, 10000)

    eprint('starting up on %s port %s' % server_address)
    sock.bind(server_address)
    sock.listen(1)

    while True:

        eprint('waiting for a connection')
        connection, client_address = sock.accept()

        eprint('client connected:', client_address)
        handle_message(connection)


if __name__ == "__main__":
    run_server()
