#!/usr/bin/env python3
"""
pngclean.py - Lossless PNG size reducer (standard library only).

Removes all ancillary chunks (EXIF/XMP/text metadata, ICC profiles,
timestamps, background color, physical dimensions, ...) and re-compresses
the image data stream with maximum zlib compression. Pixels are not modified:
the decompressed IDAT stream is kept byte by byte.

Usage:
    python pngclean.py <file.png | directory>

The file is overwritten in-place. If the argument is a directory, all .png
files inside it (non-recursive) are processed.
"""

import os
import struct
import sys
import zlib

PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"

# Chunks required to render the image correctly
KEEP_CHUNKS = (b"IHDR", b"PLTE", b"tRNS", b"IDAT", b"IEND")


def i_chunk(ctype, data):
    crc = zlib.crc32(ctype + data) & 0xFFFFFFFF
    return struct.pack(">I", len(data)) + ctype + data + struct.pack(">I", crc)


def i_parse(data):
    if data[:8] != PNG_SIGNATURE:
        raise ValueError("not a PNG file")
    pos = 8
    chunks = []
    while pos < len(data):
        if pos + 8 > len(data):
            raise ValueError("truncated chunk header")
        (length,) = struct.unpack(">I", data[pos:pos + 4])
        ctype = data[pos + 4:pos + 8]
        body = data[pos + 8:pos + 8 + length]
        if len(body) != length:
            raise ValueError("truncated chunk '%s'" % ctype.decode("latin1"))
        chunks.append((ctype, body))
        pos += length + 12
        if ctype == b"IEND":
            break
    return chunks


def i_clean(data):
    chunks = i_parse(data)
    out = [PNG_SIGNATURE]
    idat = b"".join(body for ctype, body in chunks if ctype == b"IDAT")
    idat_done = False

    if len(idat) == 0:
        raise ValueError("no IDAT chunk")

    # Re-compress the image data (lossless). Keep the original if smaller.
    raw = zlib.decompress(idat)
    recomp = zlib.compress(raw, 9)
    if len(recomp) >= len(idat):
        recomp = idat

    for ctype, body in chunks:
        if ctype not in KEEP_CHUNKS:
            continue
        if ctype == b"IDAT":
            if idat_done is False:
                out.append(i_chunk(b"IDAT", recomp))
                idat_done = True
            continue
        out.append(i_chunk(ctype, body))

    return b"".join(out)


def i_process_file(path):
    with open(path, "rb") as f:
        data = f.read()

    try:
        cleaned = i_clean(data)
    except (ValueError, zlib.error) as e:
        print("ERROR: %s: %s" % (path, e))
        return 0, 0

    if len(cleaned) >= len(data):
        print("%s: already optimized, cannot be reduced (%d bytes)" % (path, len(data)))
        return len(data), len(data)

    with open(path, "wb") as f:
        f.write(cleaned)

    print("%s: %d -> %d bytes (-%.1f%%)" % (path, len(data), len(cleaned), 100.0 * (len(data) - len(cleaned)) / len(data)))
    return len(data), len(cleaned)


def main(argv):
    if len(argv) != 2:
        print("Usage: python pngclean.py <file.png | directory>")
        return 1

    target = argv[1]
    if os.path.isdir(target):
        files = sorted(os.path.join(target, n) for n in os.listdir(target) if n.lower().endswith(".png"))
        if len(files) == 0:
            print("No .png files found in '%s'" % target)
            return 0
        total_before = 0
        total_after = 0
        for path in files:
            before, after = i_process_file(path)
            total_before += before
            total_after += after
        print("Total: %d files, %d -> %d bytes" % (len(files), total_before, total_after))
        return 0
    elif os.path.isfile(target):
        i_process_file(target)
        return 0
    else:
        print("ERROR: '%s' is not a file or directory" % target)
        return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
