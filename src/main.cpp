#include <cstdio>
#include <cstdint>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: itch_parser <file.itch>\n");
        return 1;
    }

    std::FILE* f = std::fopen(argv[1], "rb");
    if (!f) {
        std::fprintf(stderr, "Failed to open: %s\n", argv[1]);
        return 1;
    }

    uint8_t buf[100];
    size_t n = std::fread(buf, 1, sizeof(buf), f);
    std::fclose(f);

    std::printf("Read %zu bytes from %s\n\n", n, argv[1]);
    for (size_t i = 0; i < n; ++i) {
        std::printf("%02X ", buf[i]);
        if ((i + 1) % 16 == 0)
            std::printf("\n");
    }
    std::printf("\n");

    return 0;
}
