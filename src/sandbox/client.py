from __future__ import print_function

import socket
import sys
import json


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)



def send_event(message):

    server_name = 'localhost'  # sys.argv[1]

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (server_name, 10000)

        eprint('connecting to %s port %s' % server_address)
        sock.connect(server_address)

        eprint('sending "%s"' % message)
        sock.sendall(message)

    finally:
        eprint('closing socket')
        sock.close()


if __name__ == "__main__":

    for step in range(0, 24, 3):
        event = {'step': step, 'app': 'ifs'}
        send_event(json.dumps(event))
