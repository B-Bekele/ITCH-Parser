#include <gtest/gtest.h>

#include <cstring>

#include "endian.hpp"
#include "messages.hpp"

// Helpers to write big-endian values into a byte buffer
static void write_be16(uint8_t* buf, uint16_t val) {
    buf[0] = (val >> 8) & 0xFF;
    buf[1] = val & 0xFF;
}

static void write_be32(uint8_t* buf, uint32_t val) {
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

static void write_be64(uint8_t* buf, uint64_t val) {
    for (int i = 7; i >= 0; --i) {
        buf[i] = val & 0xFF;
        val >>= 8;
    }
}

// --- Endian utilities ---

TEST(EndianTest, swaps_16_bit) {
    uint8_t bytes[] = {0x00, 0x0C};
    uint16_t raw;
    std::memcpy(&raw, bytes, 2);
    EXPECT_EQ(to_host(raw), 12);
}

TEST(EndianTest, swaps_32_bit) {
    uint8_t bytes[] = {0x00, 0x16, 0xE3, 0x60};
    uint32_t raw;
    std::memcpy(&raw, bytes, 4);
    EXPECT_EQ(to_host(raw), 1500000u);
}

TEST(EndianTest, swaps_64_bit) {
    uint8_t bytes[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x39};
    uint64_t raw;
    std::memcpy(&raw, bytes, 8);
    EXPECT_EQ(to_host(raw), 12345u);
}

TEST(EndianTest, reads_6_byte_timestamp) {
    uint8_t ts[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    uint64_t result = read_timestamp(ts);
    EXPECT_EQ(result, 0x000102030405ULL);
}

TEST(EndianTest, timestamp_zero) {
    uint8_t ts[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(read_timestamp(ts), 0u);
}

TEST(EndianTest, timestamp_max) {
    uint8_t ts[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    EXPECT_EQ(read_timestamp(ts), 0xFFFFFFFFFFFFULL);
}

// --- System Event ---

TEST(MessageTest, decodes_system_event) {
    uint8_t raw[12] = {};
    raw[0] = 'S';
    write_be16(raw + 1, 0);
    write_be16(raw + 3, 0);
    raw[5] = 0x00;
    raw[6] = 0x00;
    raw[7] = 0x0A;
    raw[8] = 0x53;
    raw[9] = 0xA2;
    raw[10] = 0x88;
    raw[11] = 'O';

    auto* msg = reinterpret_cast<const SystemEvent*>(raw);
    EXPECT_EQ(msg->type, 'S');
    EXPECT_EQ(to_host(msg->stock_locate), 0);
    EXPECT_EQ(to_host(msg->tracking_number), 0);
    EXPECT_EQ(read_timestamp(msg->timestamp), 0x00000A53A288ULL);
    EXPECT_EQ(msg->event_code, 'O');
}

// --- Add Order ---

TEST(MessageTest, decodes_add_order) {
    uint8_t raw[36] = {};
    raw[0] = 'A';
    write_be16(raw + 1, 10);      // stock_locate
    write_be16(raw + 3, 0);       // tracking_number
    std::memset(raw + 5, 0, 6);   // timestamp
    write_be64(raw + 11, 12345);  // order_ref
    raw[19] = 'B';                // side
    write_be32(raw + 20, 100);    // shares
    std::memcpy(raw + 24, "AAPL    ", 8);
    write_be32(raw + 32, 1500000);  // price ($150.0000)

    auto* msg = reinterpret_cast<const AddOrder*>(raw);
    EXPECT_EQ(msg->type, 'A');
    EXPECT_EQ(to_host(msg->stock_locate), 10);
    EXPECT_EQ(to_host(msg->order_ref), 12345u);
    EXPECT_EQ(msg->side, 'B');
    EXPECT_EQ(to_host(msg->shares), 100u);
    EXPECT_EQ(std::string(msg->stock, 8), "AAPL    ");
    EXPECT_EQ(to_host(msg->price), 1500000u);
}

// --- Add Order with MPID ---

TEST(MessageTest, decodes_add_order_mpid) {
    uint8_t raw[40] = {};
    raw[0] = 'F';
    write_be16(raw + 1, 10);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    write_be64(raw + 11, 55555);
    raw[19] = 'S';
    write_be32(raw + 20, 200);
    std::memcpy(raw + 24, "MSFT    ", 8);
    write_be32(raw + 32, 2500000);  // $250.0000
    std::memcpy(raw + 36, "GSCO", 4);

    auto* msg = reinterpret_cast<const AddOrderMPID*>(raw);
    EXPECT_EQ(msg->type, 'F');
    EXPECT_EQ(to_host(msg->order_ref), 55555u);
    EXPECT_EQ(msg->side, 'S');
    EXPECT_EQ(to_host(msg->shares), 200u);
    EXPECT_EQ(std::string(msg->stock, 8), "MSFT    ");
    EXPECT_EQ(to_host(msg->price), 2500000u);
    EXPECT_EQ(std::string(msg->attribution, 4), "GSCO");
}

// --- Order Executed ---

TEST(MessageTest, decodes_order_executed) {
    uint8_t raw[31] = {};
    raw[0] = 'E';
    write_be16(raw + 1, 10);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    write_be64(raw + 11, 12345);  // order_ref
    write_be32(raw + 19, 50);     // executed_shares
    write_be64(raw + 23, 99999);  // match_number

    auto* msg = reinterpret_cast<const OrderExecuted*>(raw);
    EXPECT_EQ(msg->type, 'E');
    EXPECT_EQ(to_host(msg->order_ref), 12345u);
    EXPECT_EQ(to_host(msg->executed_shares), 50u);
    EXPECT_EQ(to_host(msg->match_number), 99999u);
}

// --- Order Executed with Price ---

TEST(MessageTest, decodes_order_executed_with_price) {
    uint8_t raw[36] = {};
    raw[0] = 'C';
    write_be16(raw + 1, 10);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    write_be64(raw + 11, 12345);
    write_be32(raw + 19, 50);
    write_be64(raw + 23, 99999);
    raw[31] = 'Y';                  // printable
    write_be32(raw + 32, 1505000);  // execution_price ($150.5000)

    auto* msg = reinterpret_cast<const OrderExecutedWithPrice*>(raw);
    EXPECT_EQ(msg->type, 'C');
    EXPECT_EQ(to_host(msg->order_ref), 12345u);
    EXPECT_EQ(to_host(msg->executed_shares), 50u);
    EXPECT_EQ(to_host(msg->match_number), 99999u);
    EXPECT_EQ(msg->printable, 'Y');
    EXPECT_EQ(to_host(msg->execution_price), 1505000u);
}

// --- Order Cancel ---

TEST(MessageTest, decodes_order_cancel) {
    uint8_t raw[23] = {};
    raw[0] = 'X';
    write_be16(raw + 1, 10);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    write_be64(raw + 11, 12345);
    write_be32(raw + 19, 25);  // cancelled_shares

    auto* msg = reinterpret_cast<const OrderCancel*>(raw);
    EXPECT_EQ(msg->type, 'X');
    EXPECT_EQ(to_host(msg->order_ref), 12345u);
    EXPECT_EQ(to_host(msg->cancelled_shares), 25u);
}

// --- Order Delete ---

TEST(MessageTest, decodes_order_delete) {
    uint8_t raw[19] = {};
    raw[0] = 'D';
    write_be16(raw + 1, 10);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    write_be64(raw + 11, 12345);

    auto* msg = reinterpret_cast<const OrderDelete*>(raw);
    EXPECT_EQ(msg->type, 'D');
    EXPECT_EQ(to_host(msg->stock_locate), 10);
    EXPECT_EQ(to_host(msg->order_ref), 12345u);
}

// --- Order Replace ---

TEST(MessageTest, decodes_order_replace) {
    uint8_t raw[35] = {};
    raw[0] = 'U';
    write_be16(raw + 1, 10);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    write_be64(raw + 11, 12345);    // original_order_ref
    write_be64(raw + 19, 12346);    // new_order_ref
    write_be32(raw + 27, 200);      // shares
    write_be32(raw + 31, 1510000);  // price ($151.0000)

    auto* msg = reinterpret_cast<const OrderReplace*>(raw);
    EXPECT_EQ(msg->type, 'U');
    EXPECT_EQ(to_host(msg->original_order_ref), 12345u);
    EXPECT_EQ(to_host(msg->new_order_ref), 12346u);
    EXPECT_EQ(to_host(msg->shares), 200u);
    EXPECT_EQ(to_host(msg->price), 1510000u);
}

// --- Trade ---

TEST(MessageTest, decodes_trade) {
    uint8_t raw[44] = {};
    raw[0] = 'P';
    write_be16(raw + 1, 10);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    write_be64(raw + 11, 12345);
    raw[19] = 'B';
    write_be32(raw + 20, 100);
    std::memcpy(raw + 24, "TSLA    ", 8);
    write_be32(raw + 32, 4200000);  // $420.0000
    write_be64(raw + 36, 77777);

    auto* msg = reinterpret_cast<const Trade*>(raw);
    EXPECT_EQ(msg->type, 'P');
    EXPECT_EQ(to_host(msg->order_ref), 12345u);
    EXPECT_EQ(msg->side, 'B');
    EXPECT_EQ(to_host(msg->shares), 100u);
    EXPECT_EQ(std::string(msg->stock, 8), "TSLA    ");
    EXPECT_EQ(to_host(msg->price), 4200000u);
    EXPECT_EQ(to_host(msg->match_number), 77777u);
}

// --- Stock Directory ---

TEST(MessageTest, decodes_stock_directory) {
    uint8_t raw[39] = {};
    raw[0] = 'R';
    write_be16(raw + 1, 1);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    std::memcpy(raw + 11, "GOOG    ", 8);
    raw[19] = 'Q';              // market_category (Nasdaq Global Select)
    raw[20] = 'N';              // financial_status (Normal)
    write_be32(raw + 21, 100);  // round_lot_size
    raw[25] = 'N';              // round_lots_only
    raw[26] = 'A';              // issue_classification
    raw[27] = 'Z';
    raw[28] = ' ';            // issue_sub_type
    raw[29] = 'P';            // authenticity
    raw[30] = 'N';            // short_sale_threshold
    raw[31] = 'N';            // ipo_flag
    raw[32] = ' ';            // luld_ref_price_tier
    raw[33] = 'N';            // etp_flag
    write_be32(raw + 34, 0);  // etp_leverage_factor
    raw[38] = 'N';            // inverse_indicator

    auto* msg = reinterpret_cast<const StockDirectory*>(raw);
    EXPECT_EQ(msg->type, 'R');
    EXPECT_EQ(to_host(msg->stock_locate), 1);
    EXPECT_EQ(std::string(msg->stock, 8), "GOOG    ");
    EXPECT_EQ(msg->market_category, 'Q');
    EXPECT_EQ(msg->financial_status, 'N');
    EXPECT_EQ(to_host(msg->round_lot_size), 100u);
    EXPECT_EQ(msg->authenticity, 'P');
}

// --- Broken Trade ---

TEST(MessageTest, decodes_broken_trade) {
    uint8_t raw[19] = {};
    raw[0] = 'B';
    write_be16(raw + 1, 0);
    write_be16(raw + 3, 0);
    std::memset(raw + 5, 0, 6);
    write_be64(raw + 11, 88888);

    auto* msg = reinterpret_cast<const BrokenTrade*>(raw);
    EXPECT_EQ(msg->type, 'B');
    EXPECT_EQ(to_host(msg->match_number), 88888u);
}
