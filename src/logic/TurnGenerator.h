#ifndef TURNGENERATOR_H
#define TURNGENERATOR_H

#include "ChessTypes.h"
#include "ChessBoard.h"

/**
 * @brief Turn generation (based on bitboards) and gameover detection.
 */
class TurnGenerator {
public:
    //! Sets the kingInCheck-Flag, based on the given chessboard.
    void initFlags(ChessBoard &cb);
    /**
     * @brief Returns the generated turns.
     * @warning The generateTurns-function needs to be called previously.
     */
    std::vector<Turn> getTurnList() const;
    //! Generates turns for the given player color, based on the given chessboard.
    void generateTurns(PlayerColor player, ChessBoard& cb);

//private: /* to access gtest functions */

    //! Contains the generated turns.
    std::vector<Turn> turnList;
    //! Creates turn objects from bitboards adds it to turnsOut list
    void bitBoardToTurns(Piece piece,
                         Field from,
                         BitBoard bbTurns,
                         BitBoard bbAllOppTurns,
                         ChessBoard& cb,
                         Turns& turnsOut);

    BitBoard calcMoveTurns(Piece piece,
                           BitBoard bbPiece,
                           BitBoard bbAllOppTurns,
                           const ChessBoard& cb);
    BitBoard calcUnCheckFields(PlayerColor opp,
                               const ChessBoard& cb);
    BitBoard calcAllOppTurns(PlayerColor opp,
                             const ChessBoard& cb);
    BitBoard calcShortCastleTurns(PlayerColor player,
                                  BitBoard bbAllPieces,
                                  BitBoard bbAllOppTurns);
    BitBoard calcLongCastleTurns(PlayerColor player,
                                 BitBoard bbAllPieces,
                                 BitBoard bbAllOppTurns);

    // nonsliding pieces
    BitBoard calcKingTurns      (BitBoard king,
                                 BitBoard allOwnPieces,
                                 BitBoard allOppTurns) const;
    BitBoard calcKnightTurns    (BitBoard knights,
                                 BitBoard allOwnPieces) const;
    BitBoard calcPawnTurns      (BitBoard pawns,
                                 BitBoard allPieces,
                                 BitBoard allOppPieces,
                                 PlayerColor player,
                                 Field enPassantSquare) const;
    BitBoard calcPawnMoveTurns  (BitBoard pawns,
                                 BitBoard allPieces,
                                 PlayerColor player) const;
    BitBoard calcPawnAttackTurns(BitBoard pawns,
                                 BitBoard allOppPieces,
                                 PlayerColor player,
                                 Field enPassantSquare) const;
    // sliding pieces
    BitBoard calcQueenTurns (BitBoard queens,
                             BitBoard allOppPieces,
                             BitBoard allPieces) const;
    BitBoard calcBishopTurns(BitBoard bishops,
                             BitBoard allOppPieces,
                             BitBoard allPieces) const;
    BitBoard calcRookTurns  (BitBoard rooks,
                             BitBoard allOppPieces,
                             BitBoard allPieces) const;

    // some helper functions
    BitBoard maskRank (Rank rank) const;
    BitBoard clearRank(Rank rank) const;
    BitBoard maskFile (File file) const;
    BitBoard clearFile(File file) const;

    BitBoard getBitsE(BitBoard bbPiece) const;
    BitBoard getBitsW(BitBoard bbPiece) const;
    BitBoard getBitsN(BitBoard bbPiece) const;
    BitBoard getBitsS(BitBoard bbPiece) const;

    BitBoard getBitsNE(BitBoard bbPiece) const;
    BitBoard getBitsNW(BitBoard bbPiece) const;
    BitBoard getBitsSE(BitBoard bbPiece) const;
    BitBoard getBitsSW(BitBoard bbPiece) const;
};


#endif // TURNGENERATOR_H
