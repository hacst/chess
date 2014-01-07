#ifndef INCREMENTALZOBRISTHASHER_H
#define INCREMENTALZOBRISTHASHER_H

#include <array>
#include "logic/ChessTypes.h"

/**
 * @brief Zobrist-Hash implementation.
 * Uses mt19937 to initialize from a fixed seed 452134
 */
template <typename THASH, uint64_t SEED = 46170>
class IncrementalZobristHasher {
public:
    IncrementalZobristHasher();
private:
    static const struct Constants {
        Constants();

        std::array<std::array<THASH, NUM_FIELDS>, NUM_PIECETYPES> pieceSquares;
        THASH sideToMove;
        std::array<THASH, 4> castlingRights;
        std::array<THASH, NUM_FILES> enPassantRights;
    } m_constants;
};

using IncrementalZobristHasher64 = IncrementalZobristHasher<uint64_t>;


#endif INCREMENTALZOBRISTHASHER_H