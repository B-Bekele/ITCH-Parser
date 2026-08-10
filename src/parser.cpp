#include "parser.hpp"

#include <chrono>
#include <cstdio>
#include <cstring>
#include <memory>

#include "endian.hpp"
#include "messages.hpp"

static constexpr size_t BUF_SIZE = 1 << 20;  // 1 MB

ParseResult Parser::parse_file(const std::string& path) {
    ParseResult result = {};

    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return result;

    auto start = std::chrono::high_resolution_clock::now();

    auto buffer = std::make_unique<uint8_t[]>(BUF_SIZE);
    size_t buffered = 0;
    size_t pos = 0;

    while (true) {
        if (pos > 0) {
            size_t leftover = buffered - pos;
            std::memmove(buffer.get(), buffer.get() + pos, leftover);
            buffered = leftover;
            pos = 0;
        }

        size_t bytes_read = std::fread(buffer.get() + buffered, 1, BUF_SIZE - buffered, f);
        buffered += bytes_read;

        while (pos + 2 <= buffered) {
            uint16_t len = to_host(*reinterpret_cast<uint16_t*>(buffer.get() + pos));
            if (pos + 2 + len > buffered) break;

            char msg_type = static_cast<char>(buffer[pos + 2]);
            result.message_counts[static_cast<uint8_t>(msg_type)]++;
            result.total_messages++;
            result.total_bytes += 2 + len;

            switch (msg_type) {
                case 'S':
                case 'R':
                case 'H':
                case 'Y':
                case 'L':
                case 'V':
                case 'W':
                case 'K':
                case 'J':
                case 'h':
                case 'A':
                case 'F':
                case 'E':
                case 'C':
                case 'X':
                case 'D':
                case 'U':
                case 'P':
                case 'Q':
                case 'B':
                case 'I':
                case 'N':
                    break;
                default:
                    break;
            }

            pos += 2 + len;
        }

        if (bytes_read == 0) break;
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsed_seconds = std::chrono::duration<double>(end - start).count();

    std::fclose(f);
    return result;
}
