#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct SystemEvent {
    char type;                // 'S'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char event_code;
};

struct StockDirectory {
    char type;                // 'R'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char market_category;
    char financial_status;
    uint32_t round_lot_size;
    char round_lots_only;
    char issue_classification;
    char issue_sub_type[2];
    char authenticity;
    char short_sale_threshold;
    char ipo_flag;
    char luld_ref_price_tier;
    char etp_flag;
    uint32_t etp_leverage_factor;
    char inverse_indicator;
};

struct StockTradingAction {
    char type;                // 'H'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char trading_state;
    char reserved;
    char reason[4];
};

struct RegSHO {
    char type;                // 'Y'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char reg_sho_action;
};

struct MarketParticipantPosition {
    char type;                // 'L'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char mpid[4];
    char stock[8];
    char primary_market_maker;
    char market_maker_mode;
    char market_participant_state;
};

struct MWCBDeclineLevel {
    char type;                // 'V'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t level1;
    uint64_t level2;
    uint64_t level3;
};

struct MWCBStatus {
    char type;                // 'W'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char breached_level;
};

struct IPOQuotingPeriod {
    char type;                // 'K'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    uint32_t ipo_release_time;
    char ipo_release_qualifier;
    uint32_t ipo_price;
};

struct LULDAuctionCollar {
    char type;                // 'J'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    uint32_t ref_price;
    uint32_t upper_price;
    uint32_t lower_price;
    uint32_t extension;
};

struct OperationalHalt {
    char type;                // 'h'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char market_code;
    char halt_action;
};

struct AddOrder {
    char type;                // 'A'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref;
    char side;
    uint32_t shares;
    char stock[8];
    uint32_t price;
};

struct AddOrderMPID {
    char type;                // 'F'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref;
    char side;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    char attribution[4];
};

struct OrderExecuted {
    char type;                // 'E'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref;
    uint32_t executed_shares;
    uint64_t match_number;
};

struct OrderExecutedWithPrice {
    char type;                // 'C'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref;
    uint32_t executed_shares;
    uint64_t match_number;
    char printable;
    uint32_t execution_price;
};

struct OrderCancel {
    char type;                // 'X'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref;
    uint32_t cancelled_shares;
};

struct OrderDelete {
    char type;                // 'D'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref;
};

struct OrderReplace {
    char type;                // 'U'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t original_order_ref;
    uint64_t new_order_ref;
    uint32_t shares;
    uint32_t price;
};

struct Trade {
    char type;                // 'P'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref;
    char side;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    uint64_t match_number;
};

struct CrossTrade {
    char type;                // 'Q'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t shares;
    char stock[8];
    uint32_t cross_price;
    uint64_t match_number;
    char cross_type;
};

struct BrokenTrade {
    char type;                // 'B'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t match_number;
};

struct NOII {
    char type;                // 'I'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t paired_shares;
    uint64_t imbalance_shares;
    char imbalance_direction;
    char stock[8];
    uint32_t far_price;
    uint32_t near_price;
    uint32_t current_ref_price;
    char cross_type;
    char price_variation;
};

struct RetailPriceImprovement {
    char type;                // 'N'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char interest_flag;
};

struct DirectListing {
    char type;                // 'O'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char open_eligibility_status;
    uint32_t min_price;
    uint32_t max_price;
    uint32_t near_execution_price;
    uint64_t near_execution_time;
    uint32_t lower_collar;
    uint32_t upper_collar;
};

#pragma pack(pop)

static_assert(sizeof(SystemEvent) == 12, "SystemEvent must be 12 bytes");
static_assert(sizeof(StockDirectory) == 39, "StockDirectory must be 39 bytes");
static_assert(sizeof(StockTradingAction) == 25, "StockTradingAction must be 25 bytes");
static_assert(sizeof(RegSHO) == 20, "RegSHO must be 20 bytes");
static_assert(sizeof(MarketParticipantPosition) == 26, "MarketParticipantPosition must be 26 bytes");
static_assert(sizeof(MWCBDeclineLevel) == 35, "MWCBDeclineLevel must be 35 bytes");
static_assert(sizeof(MWCBStatus) == 12, "MWCBStatus must be 12 bytes");
static_assert(sizeof(IPOQuotingPeriod) == 28, "IPOQuotingPeriod must be 28 bytes");
static_assert(sizeof(LULDAuctionCollar) == 35, "LULDAuctionCollar must be 35 bytes");
static_assert(sizeof(OperationalHalt) == 21, "OperationalHalt must be 21 bytes");
static_assert(sizeof(AddOrder) == 36, "AddOrder must be 36 bytes");
static_assert(sizeof(AddOrderMPID) == 40, "AddOrderMPID must be 40 bytes");
static_assert(sizeof(OrderExecuted) == 31, "OrderExecuted must be 31 bytes");
static_assert(sizeof(OrderExecutedWithPrice) == 36, "OrderExecutedWithPrice must be 36 bytes");
static_assert(sizeof(OrderCancel) == 23, "OrderCancel must be 23 bytes");
static_assert(sizeof(OrderDelete) == 19, "OrderDelete must be 19 bytes");
static_assert(sizeof(OrderReplace) == 35, "OrderReplace must be 35 bytes");
static_assert(sizeof(Trade) == 44, "Trade must be 44 bytes");
static_assert(sizeof(CrossTrade) == 40, "CrossTrade must be 40 bytes");
static_assert(sizeof(BrokenTrade) == 19, "BrokenTrade must be 19 bytes");
static_assert(sizeof(NOII) == 50, "NOII must be 50 bytes");
static_assert(sizeof(RetailPriceImprovement) == 20, "RetailPriceImprovement must be 20 bytes");
static_assert(sizeof(DirectListing) == 48, "DirectListing must be 48 bytes");
