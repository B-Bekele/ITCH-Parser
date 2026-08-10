#include <cstdio>

#include "parser.hpp"

static const char* message_name(char type) {
    switch (type) {
        case 'S':
            return "System Event";
        case 'R':
            return "Stock Directory";
        case 'H':
            return "Stock Trading Action";
        case 'Y':
            return "Reg SHO";
        case 'L':
            return "Market Participant Position";
        case 'V':
            return "MWCB Decline Level";
        case 'W':
            return "MWCB Status";
        case 'K':
            return "IPO Quoting Period";
        case 'J':
            return "LULD Auction Collar";
        case 'h':
            return "Operational Halt";
        case 'A':
            return "Add Order";
        case 'F':
            return "Add Order (MPID)";
        case 'E':
            return "Order Executed";
        case 'C':
            return "Order Executed w/ Price";
        case 'X':
            return "Order Cancel";
        case 'D':
            return "Order Delete";
        case 'U':
            return "Order Replace";
        case 'P':
            return "Trade";
        case 'Q':
            return "Cross Trade";
        case 'B':
            return "Broken Trade";
        case 'I':
            return "NOII";
        case 'N':
            return "Retail Price Improvement";
        default:
            return "Unknown";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: itch_parser <file.itch>\n");
        return 1;
    }

    Parser parser;
    ParseResult result = parser.parse_file(argv[1]);

    if (result.total_messages == 0) {
        std::fprintf(stderr, "Failed to parse: %s\n", argv[1]);
        return 1;
    }

    std::printf("%-30s %15s\n", "Message Type", "Count");
    std::printf("%-30s %15s\n", "------------------------------", "---------------");

    for (int i = 0; i < 256; i++) {
        if (result.message_counts[i] > 0) {
            std::printf("%-30s %15llu\n", message_name(static_cast<char>(i)),
                        result.message_counts[i]);
        }
    }

    std::printf("%-30s %15s\n", "------------------------------", "---------------");
    std::printf("%-30s %15llu\n", "Total", result.total_messages);

    double mb = result.total_bytes / (1024.0 * 1024.0);
    double msg_per_sec = result.total_messages / result.elapsed_seconds;

    double first_sec = result.first_timestamp / 1e9;
    double last_sec = result.last_timestamp / 1e9;
    int first_h = static_cast<int>(first_sec) / 3600;
    int first_m = (static_cast<int>(first_sec) % 3600) / 60;
    int last_h = static_cast<int>(last_sec) / 3600;
    int last_m = (static_cast<int>(last_sec) % 3600) / 60;

    std::printf("\nTimestamp range: %02d:%02d - %02d:%02d (%.1f hours)\n", first_h, first_m, last_h,
                last_m, (last_sec - first_sec) / 3600.0);
    std::printf("%.2f MB in %.2f sec\n", mb, result.elapsed_seconds);
    std::printf("%.0f messages/sec (%.2fM msg/sec)\n", msg_per_sec, msg_per_sec / 1e6);

    return 0;
}
