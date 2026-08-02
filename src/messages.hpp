#pragma once

#include <cstdint>

#pragma pack(push, 1)

struct SystemEvent {
    char type;                // 'S'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char event_code;          // O, S, Q, M, E, C
};

struct AddOrder {
    char type;                // 'A'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref;
    char side;                // 'B' or 'S'
    uint32_t shares;
    char stock[8];
    uint32_t price;
};

#pragma pack(pop)

static_assert(sizeof(SystemEvent) == 12, "SystemEvent must be 12 bytes");
static_assert(sizeof(AddOrder) == 36, "AddOrder must be 36 bytes");
