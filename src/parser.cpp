#include "parser.hpp"

#include <chrono>
#include <cstdio>
#include <cstring>

#include "endian.hpp"
#include "messages.hpp"

ParseResult Parser::parse_file(const std::string& path) {
    ParseResult result = {};

    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return result;

    auto start = std::chrono::high_resolution_clock::now();

    uint16_t raw_len;
    uint8_t body[256];

    while (std::fread(&raw_len, sizeof(raw_len), 1, f) == 1) {
        uint16_t len = to_host(raw_len);
        if (std::fread(body, 1, len, f) != len) break;

        char msg_type = static_cast<char>(body[0]);
        result.message_counts[static_cast<uint8_t>(msg_type)]++;
        result.total_messages++;
        result.total_bytes += 2 + len;

        switch (msg_type) {
            case 'S': /* SystemEvent */              break;
            case 'R': /* StockDirectory */           break;
            case 'H': /* StockTradingAction */       break;
            case 'Y': /* RegSHO */                   break;
            case 'L': /* MarketParticipantPosition */ break;
            case 'V': /* MWCBDeclineLevel */         break;
            case 'W': /* MWCBStatus */               break;
            case 'K': /* IPOQuotingPeriod */         break;
            case 'J': /* LULDAuctionCollar */        break;
            case 'h': /* OperationalHalt */          break;
            case 'A': /* AddOrder */                 break;
            case 'F': /* AddOrderMPID */             break;
            case 'E': /* OrderExecuted */            break;
            case 'C': /* OrderExecutedWithPrice */   break;
            case 'X': /* OrderCancel */              break;
            case 'D': /* OrderDelete */              break;
            case 'U': /* OrderReplace */             break;
            case 'P': /* Trade */                    break;
            case 'Q': /* CrossTrade */               break;
            case 'B': /* BrokenTrade */              break;
            case 'I': /* NOII */                     break;
            case 'N': /* RetailPriceImprovement */   break;
            default: break;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsed_seconds =
        std::chrono::duration<double>(end - start).count();

    std::fclose(f);
    return result;
}
