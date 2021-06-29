import socket
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--hard', action='store_true')
parser.add_argument('ifile')
parser.add_argument('ofile')
args = parser.parse_args()

is_hard = args.hard

s = socket.create_connection(('h4x.0x04.net', 1337))
sf = s.makefile('rb')

with open(args.ifile, 'rb') as f:
    d = f.read()

s.sendall(bytes([is_hard]) + len(d).to_bytes(4, 'little') + args.ifile.encode() + b'\0' + d)

key = sf.read(64)
if len(key) != 64:
    raise ValueError()

key = key.decode()

print(f'KEY: {key}')

sz = sf.read(4)
if len(sz) != 4:
    raise ValueError()

sz = int.from_bytes(sz, 'little')

d = sf.read(sz)
if len(d) != sz:
    raise ValueError()

with open(args.ofile, 'wb') as f:
    f.write(d)
