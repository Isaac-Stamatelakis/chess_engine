//
// Created by Isaac on 2026-01-10.
//

#ifndef CHESSENGINE_DEBUG_H
#define CHESSENGINE_DEBUG_H
#include <cstdint>
#include <string>

enum DebugFlag : uint8_t {
    DebugNone       = 0,
    Occupancy  = 1 << 0,
    Attacks    = 1 << 1,
    Pinned     = 1 << 2,
    FreeMove    = 1 << 3,
};

struct DebugOptions {
    uint8_t flags = DebugNone;

    void ParseArg(const std::string& arg) {
        if (arg == "--debug-occupancy") {
            flags |= Occupancy;
        } else if (arg == "--debug-attacks") {
            flags |= Attacks;
        } else if (arg == "--debug-pinned") {
            flags |= Pinned;
        } else if (arg == "--debug-free-move") {
            flags |= FreeMove;
        }
    }
};


#endif //CHESSENGINE_DEBUG_H