#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

struct ParseResult {
    uint64_t message_counts[256] = {};
    uint64_t total_messages = 0;
    uint64_t total_bytes = 0;
    uint64_t first_timestamp = 0;
    uint64_t last_timestamp = 0;
    uint64_t orphaned_refs = 0;
    double elapsed_seconds = 0.0;
};

class Parser {
   public:
    ParseResult parse_file(const std::string& path);

   private:
    std::unordered_set<uint64_t> active_orders_;
};
