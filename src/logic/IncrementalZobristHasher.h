#ifndef INCREMENTALZOBRISTHASHER_H
#define INCREMENTALZOBRISTHASHER_H

#include <array>
#include "logic/ChessTypes.h"

class ChessBoard;

/**
 * @brief Incremental polyglot constants based Zobrist-Hash implementation.
 */
class IncrementalZobristHasher {
public:
    IncrementalZobristHasher();
    IncrementalZobristHasher(const ChessBoard& board);
    using Hash = uint64_t;
    
    //! Gives a full estimate for the given board
    static Hash estimateFullBoard(const ChessBoard& board);
    
    Hash getHash() const;
private:
    Hash m_hash;
    
    class HashConstants {
    public:
        HashConstants();

        
        //! Typesafe accessor for constants
        inline Hash forPieceSquare(PieceType pieceType, Field field, PlayerColor playerColor) const {
            return pieceSquares[pieceType][playerColor][field];
        }

        inline Hash forCastlingRightsShort(PlayerColor player) const {
            return castlingRightsShort[player];
        }
        
        inline Hash forCastlingRightsLong(PlayerColor player) const {
            return castlingRightsLong[player];
        }
        
        inline Hash forEnPassantRights(File file) const {
            return enPassantRights[file];
        }
        
        inline Hash forSideToMove(PlayerColor player) const {
            return player == White ? sideToMove : 0ULL;
        }

    private:
        std::array<std::array<std::array<Hash, NUM_FIELDS>, NUM_PLAYERS>, NUM_PIECETYPES> pieceSquares;
        
        Hash sideToMove;
        std::array<Hash, NUM_PLAYERS> castlingRightsShort;
        std::array<Hash, NUM_PLAYERS> castlingRightsLong;
        
        std::array<Hash, NUM_FILES> enPassantRights;
    };
    
    const static HashConstants m_hashConstants;
};


#endif //INCREMENTALZOBRISTHASHER_H
