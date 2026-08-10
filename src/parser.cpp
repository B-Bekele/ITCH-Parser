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

            uint8_t* body = buffer.get() + pos + 2;
            char msg_type = static_cast<char>(body[0]);
            result.message_counts[static_cast<uint8_t>(msg_type)]++;
            result.total_messages++;
            result.total_bytes += 2 + len;

            uint64_t ts = 0;

            switch (msg_type) {
                case 'S': {
                    auto* m = reinterpret_cast<const SystemEvent*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'R': {
                    auto* m = reinterpret_cast<const StockDirectory*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'H': {
                    auto* m = reinterpret_cast<const StockTradingAction*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'Y': {
                    auto* m = reinterpret_cast<const RegSHO*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'L': {
                    auto* m = reinterpret_cast<const MarketParticipantPosition*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'V': {
                    auto* m = reinterpret_cast<const MWCBDeclineLevel*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'W': {
                    auto* m = reinterpret_cast<const MWCBStatus*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'K': {
                    auto* m = reinterpret_cast<const IPOQuotingPeriod*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'J': {
                    auto* m = reinterpret_cast<const LULDAuctionCollar*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'h': {
                    auto* m = reinterpret_cast<const OperationalHalt*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'A': {
                    auto* m = reinterpret_cast<const AddOrder*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'F': {
                    auto* m = reinterpret_cast<const AddOrderMPID*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'E': {
                    auto* m = reinterpret_cast<const OrderExecuted*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'C': {
                    auto* m = reinterpret_cast<const OrderExecutedWithPrice*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'X': {
                    auto* m = reinterpret_cast<const OrderCancel*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'D': {
                    auto* m = reinterpret_cast<const OrderDelete*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'U': {
                    auto* m = reinterpret_cast<const OrderReplace*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'P': {
                    auto* m = reinterpret_cast<const Trade*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'Q': {
                    auto* m = reinterpret_cast<const CrossTrade*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'B': {
                    auto* m = reinterpret_cast<const BrokenTrade*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'I': {
                    auto* m = reinterpret_cast<const NOII*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                case 'N': {
                    auto* m = reinterpret_cast<const RetailPriceImprovement*>(body);
                    ts = read_timestamp(m->timestamp);
                    break;
                }
                default:
                    break;
            }

            if (result.first_timestamp == 0 && ts != 0) {
                result.first_timestamp = ts;
            }
            result.last_timestamp = ts;

            pos += 2 + len;
        }

        if (bytes_read == 0) break;
    }

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsed_seconds = std::chrono::duration<double>(end - start).count();

    std::fclose(f);
    return result;
}
