#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../src/endian.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::fprintf(stderr, "Usage: extract_fixture <input.itch> <output.itch> [count]\n");
        return 1;
    }

    int count = 1000;
    if (argc >= 4) count = std::atoi(argv[3]);

    std::FILE* in = std::fopen(argv[1], "rb");
    std::FILE* out = std::fopen(argv[2], "wb");
    if (!in || !out) {
        std::fprintf(stderr, "Failed to open files\n");
        return 1;
    }

    uint16_t raw_len;
    uint8_t body[256];
    int written = 0;

    while (written < count && std::fread(&raw_len, sizeof(raw_len), 1, in) == 1) {
        uint16_t len = to_host(raw_len);
        if (std::fread(body, 1, len, in) != len) break;

        std::fwrite(&raw_len, sizeof(raw_len), 1, out);
        std::fwrite(body, 1, len, out);
        written++;
    }

    std::fclose(in);
    std::fclose(out);
    std::printf("Extracted %d messages\n", written);
    return 0;
}
