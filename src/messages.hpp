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

#pragma pack(pop)

static_assert(sizeof(SystemEvent) == 12, "SystemEvent must be 12 bytes");
