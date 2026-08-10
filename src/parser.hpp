#pragma once

#include <cstdint>
#include <string>

struct ParseResult {
    uint64_t message_counts[256] = {};
    uint64_t total_messages = 0;
    uint64_t total_bytes = 0;
    uint64_t first_timestamp = 0;
    uint64_t last_timestamp = 0;
    double elapsed_seconds = 0.0;
};

class Parser {
   public:
    ParseResult parse_file(const std::string& path);
};
