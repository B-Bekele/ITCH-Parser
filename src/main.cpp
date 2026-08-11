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

static void print_result(const char* filename, const ParseResult& result) {
    std::printf("=== %s ===\n\n", filename);

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
    std::printf("Order validation: %llu orphaned references\n\n", result.orphaned_refs);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::fprintf(stderr, "Usage: itch_parser <file.itch> [file2.itch ...]\n");
        return 1;
    }

    uint64_t total_messages = 0;
    uint64_t total_bytes = 0;
    uint64_t total_orphaned = 0;
    double total_elapsed = 0.0;
    int files_parsed = 0;

    for (int i = 1; i < argc; i++) {
        Parser parser;
        ParseResult result = parser.parse_file(argv[i]);

        if (result.total_messages == 0) {
            std::fprintf(stderr, "Failed to parse: %s\n", argv[i]);
            continue;
        }

        print_result(argv[i], result);

        total_messages += result.total_messages;
        total_bytes += result.total_bytes;
        total_orphaned += result.orphaned_refs;
        total_elapsed += result.elapsed_seconds;
        files_parsed++;
    }

    if (files_parsed > 1) {
        double mb = total_bytes / (1024.0 * 1024.0);
        double msg_per_sec = total_messages / total_elapsed;

        std::printf("=== Summary (%d sessions) ===\n\n", files_parsed);
        std::printf("Total messages:          %llu\n", total_messages);
        std::printf("Total size:              %.2f MB\n", mb);
        std::printf("Total time:              %.2f sec\n", total_elapsed);
        std::printf("Average throughput:      %.2fM msg/sec\n", msg_per_sec / 1e6);
        std::printf("Total orphaned refs:     %llu\n", total_orphaned);
    }

    return 0;
}
