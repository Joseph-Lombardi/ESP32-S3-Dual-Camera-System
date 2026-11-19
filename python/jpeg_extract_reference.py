"""Reference-only offline JPEG carving, NOT the original validated serial receiver.

Usage: python jpeg_extract_reference.py capture.bin output_dir
Extracts JPEG SOI/EOI-delimited images from a previously saved binary stream.
No packet sequencing, CRC, framing, or serial protocol guarantees.
"""
import argparse
from pathlib import Path

def extract(data: bytes):
    pos = 0
    while True:
        start = data.find(b"\xff\xd8", pos)
        if start < 0: return
        end = data.find(b"\xff\xd9", start + 2)
        if end < 0: return
        yield data[start:end + 2]
        pos = end + 2

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('capture', type=Path)
    parser.add_argument('output_dir', type=Path)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    for n, jpeg in enumerate(extract(args.capture.read_bytes()), 1):
        output = args.output_dir / f'image_{n:04d}.jpg'
        output.write_bytes(jpeg)
        print(f'{output}: {len(jpeg)} bytes')

if __name__ == '__main__': main()
