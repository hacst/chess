#ifndef INCREMENTALZOBRISTHASHER_H
#define INCREMENTALZOBRISTHASHER_H

#include <array>
#include "logic/ChessTypes.h"

class ChessBoard;
class Turn;

/**
 * @brief Incremental polyglot constants based Zobrist-Hash implementation.
 */
class IncrementalZobristHasher {
public:
    IncrementalZobristHasher();
    IncrementalZobristHasher(const ChessBoard& board);
    using Hash = uint64_t;
    
    //! Gives a full estimate for the given board
    static Hash hashFullBoard(const ChessBoard& board);
    
    //! Returns the current zobrist hash
    Hash getHash() const;

    //! Called when the en passant field is cleared.
    void clearedEnPassantSquare(Field enPassantSquare);
    //! Called for a move update.
    void moveIncrement(const Turn& turn);
    //! Called when a piece is captured
    void captureIncrement(Field field, const Piece& capturedPiece);
    //! Updates hash for now active player
    void turnAppliedIncrement();
    //! Called when turn might give the enemy an en passant possibility
    void newEnPassantPossibility(const Turn& turn, BitBoard opposingPawns);
    //! Called to potentially update castling rights.
    void updateCastlingRights(
        const std::array<bool, NUM_PLAYERS>& prevShortCastleLeft,
        const std::array<bool, NUM_PLAYERS>& prevLongCastleRight,
        const std::array<bool, NUM_PLAYERS>& shortCastleRight,
        const std::array<bool, NUM_PLAYERS>& longCastleRight
    );

private:
    /**
     * @brief Return true if en passant according to polyglot rules.
     * Polyglot uses a en passant definition that differs from FEN.
     * It only integrates the file if there are pawns neighbouring the
     * pawn that opened the en passant possibility.
     */
    static bool isPolyglotEnPassant(const ChessBoard& board);

    Hash m_hash;
    bool m_isEnPassantApplied;
    
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
