# ITCH Parser

A C++ parser for the Nasdaq TotalView-ITCH 5.0 binary protocol. Decodes all 22 message types from multi-gigabyte session files and validates order lifecycle integrity.

## Performance

Tested on 3 full trading sessions (845M messages, 24.7 GB):

| Session | Messages | Throughput | Orphaned Refs |
|---|---|---|---|
| 12/30/2019 | 268,744,780 | 3.41M msg/sec | 0 |
| 07/30/2019 | 282,229,684 | 3.26M msg/sec | 0 |
| 10/30/2019 | 293,989,079 | 3.27M msg/sec | 0 |

Key optimizations:
- **1 MB buffered reads** with `memmove` for messages spanning buffer boundaries, reducing syscalls from one per message to one per block
- **Jump-table switch dispatch** over all 22 message types (compiles to a jump table at `-O2`)
- **Packed structs** with `#pragma pack(push, 1)` for zero-copy message decoding

## Order Lifecycle Validation

The parser tracks every order reference through its full lifecycle using an `unordered_set`:
- `Add Order` (A/F) inserts the reference
- `Order Replace` (U) erases the old reference and inserts the new one
- `Order Executed` (E), `Order Executed w/ Price` (C), and `Order Cancel` (X) verify the reference exists
- `Order Delete` (D) verifies and removes the reference

Any execute, cancel, or delete without a prior add is counted as an orphaned reference. Across 3 full sessions (845M messages), zero orphaned references were found.

## Building

Requires C++17, CMake 3.16+, and a compiler with big-endian byte-swap intrinsics (MSVC, GCC, Clang).

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Usage

```bash
# Parse a single session file
./build/itch_parser data/12302019.NASDAQ_ITCH50

# Parse multiple sessions
./build/itch_parser data/12302019.NASDAQ_ITCH50 data/07302019.NASDAQ_ITCH50
```

Output includes message type counts, timestamp range, throughput, and orphaned reference count for each file, with an aggregate summary when parsing multiple files.

## Data

ITCH 5.0 session files can be downloaded from the [Nasdaq FTP server](https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/). Files are gzip-compressed and typically 3-4 GB compressed, 7-8 GB decompressed.

A small 1000-message fixture (`data/sample.itch`) is included for testing.

## Testing

```bash
cmake --build build --config Release --target parser_test
ctest --test-dir build --output-on-failure
```

30 tests covering endian conversion, message struct decoding, parser edge cases, and order lifecycle validation.

## Benchmarking

```bash
cmake --build build --config Release --target parser_bench
./build/Release/parser_bench
```

Uses [Google Benchmark](https://github.com/google/benchmark) to measure parser throughput.

## Docker

```bash
docker build -t itch-parser .
docker run -v /path/to/data:/data itch-parser /data/12302019.NASDAQ_ITCH50
```

## Message Types

All 22 ITCH 5.0 message types are supported:

| Type | Name | Size |
|---|---|---|
| S | System Event | 12 |
| R | Stock Directory | 39 |
| H | Stock Trading Action | 25 |
| Y | Reg SHO Restriction | 20 |
| L | Market Participant Position | 26 |
| V | MWCB Decline Level | 35 |
| W | MWCB Status | 12 |
| K | IPO Quoting Period Update | 28 |
| J | LULD Auction Collar | 35 |
| h | Operational Halt | 21 |
| A | Add Order | 36 |
| F | Add Order (MPID Attribution) | 40 |
| E | Order Executed | 31 |
| C | Order Executed With Price | 36 |
| X | Order Cancel | 23 |
| D | Order Delete | 19 |
| U | Order Replace | 35 |
| P | Trade | 44 |
| Q | Cross Trade | 40 |
| B | Broken Trade | 19 |
| I | NOII | 50 |
| N | Retail Price Improvement | 20 |
