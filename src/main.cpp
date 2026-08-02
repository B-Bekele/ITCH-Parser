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

    uint16_t raw_len;
    uint8_t body[256];
    int add_orders_printed = 0;

    while (std::fread(&raw_len, sizeof(raw_len), 1, f) == 1) {
        uint16_t len = to_host(raw_len);
        if (std::fread(body, 1, len, f) != len) break;

        if (body[0] == 'A' && add_orders_printed < 5) {
            auto* msg = reinterpret_cast<const AddOrder*>(body);

            char stock[9] = {};
            std::memcpy(stock, msg->stock, 8);

            double price = to_host(msg->price) / 10000.0;

            std::printf("Add Order: ref=%llu side=%c shares=%u stock=%.8s price=%.4f\n",
                        to_host(msg->order_ref), msg->side, to_host(msg->shares), stock, price);

            add_orders_printed++;
            if (add_orders_printed == 5) break;
        }
    }

    std::fclose(f);
    return 0;
}
