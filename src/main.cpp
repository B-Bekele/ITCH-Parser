#include <cstdint>
#include <cstdio>
#include <cstring>

#include "endian.hpp"
#include "messages.hpp"

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

    // Read the first message: should be a System Event
    uint16_t raw_len;
    std::fread(&raw_len, sizeof(raw_len), 1, f);
    uint16_t len = to_host(raw_len);

    uint8_t body[256];
    std::fread(body, 1, len, f);
    std::fclose(f);

    std::printf("First message: length=%u, type='%c'\n", len, body[0]);

    if (body[0] == 'S') {
        auto* msg = reinterpret_cast<const SystemEvent*>(body);
        uint64_t ts = read_timestamp(msg->timestamp);
        double seconds = ts / 1e9;

        std::printf("  System Event\n");
        std::printf("  stock_locate:     %u\n", to_host(msg->stock_locate));
        std::printf("  tracking_number:  %u\n", to_host(msg->tracking_number));
        std::printf("  timestamp:        %llu ns (%.3f sec since midnight)\n", ts, seconds);
        std::printf("  event_code:       '%c'", msg->event_code);

        switch (msg->event_code) {
            case 'O': std::printf(" (Start of Messages)\n"); break;
            case 'S': std::printf(" (Start of System Hours)\n"); break;
            case 'Q': std::printf(" (Start of Market Hours)\n"); break;
            case 'M': std::printf(" (End of Market Hours)\n"); break;
            case 'E': std::printf(" (End of System Hours)\n"); break;
            case 'C': std::printf(" (End of Messages)\n"); break;
            default: std::printf(" (Unknown)\n"); break;
        }
    }

    return 0;
}
