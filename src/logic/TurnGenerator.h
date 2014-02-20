#ifndef TURNGENERATOR_H
#define TURNGENERATOR_H

#include "ChessTypes.h"
#include "ChessBoard.h"

class TurnGenerator {
public:
    virtual std::vector<Turn> getTurnList() const;
    virtual void              generateTurns(PlayerColor player,
                                            ChessBoard& cb);
//private:
    std::vector<Turn> turnList;

    virtual std::vector<Turn> bitBoardToTurns(Piece piece,
                                              Field from,
                                              BitBoard bbTurns);

    virtual BitBoard calcMoveTurns(Piece piece,
                                   BitBoard bbPiece,
                                   BitBoard bbAllOppTurns,
                                   const ChessBoard& cb);
    virtual BitBoard calcUnCheckFields(PlayerColor opp,
                                       const ChessBoard& cb);
    virtual BitBoard calcAllOppTurns(PlayerColor opp,
                                     const ChessBoard& cb);
    virtual BitBoard calcShortCastleTurns(PlayerColor player,
                                          BitBoard bbAllPieces,
                                          BitBoard bbAllOppTurns);
    virtual BitBoard calcLongCastleTurns(PlayerColor player,
                                         BitBoard bbAllPieces,
                                         BitBoard bbAllOppTurns);

    // nonsliding pieces
    virtual BitBoard calcKingTurns      (BitBoard king,
                                         BitBoard allOwnPieces,
                                         BitBoard allOppTurns) const;
    virtual BitBoard calcKnightTurns    (BitBoard knights,
                                         BitBoard allOwnPieces) const;
    virtual BitBoard calcPawnTurns      (BitBoard pawns,
                                         BitBoard allPieces,
                                         BitBoard allOppPieces,
                                         PlayerColor player,
                                         Field enPassantSquare) const;
    virtual BitBoard calcPawnMoveTurns  (BitBoard pawns,
                                         BitBoard allPieces,
                                         PlayerColor player) const;
    virtual BitBoard calcPawnAttackTurns(BitBoard pawns,
                                         BitBoard allOppPieces,
                                         PlayerColor player,
                                         Field enPassantSquare) const;
    // sliding pieces
    virtual BitBoard calcQueenTurns (BitBoard queens,
                                     BitBoard allOppPieces,
                                     BitBoard allPieces) const;
    virtual BitBoard calcBishopTurns(BitBoard bishops,
                                     BitBoard allOppPieces,
                                     BitBoard allPieces) const;
    virtual BitBoard calcRookTurns  (BitBoard rooks,
                                     BitBoard allOppPieces,
                                     BitBoard allPieces) const;

    // some helper functions
    virtual BitBoard maskRank (Rank rank) const;
    virtual BitBoard clearRank(Rank rank) const;
    virtual BitBoard maskFile (File file) const;
    virtual BitBoard clearFile(File file) const;

    virtual BitBoard getBitsE(BitBoard bbPiece) const;
    virtual BitBoard getBitsW(BitBoard bbPiece) const;
    virtual BitBoard getBitsN(BitBoard bbPiece) const;
    virtual BitBoard getBitsS(BitBoard bbPiece) const;

    virtual BitBoard getBitsNE(BitBoard bbPiece) const;
    virtual BitBoard getBitsNW(BitBoard bbPiece) const;
    virtual BitBoard getBitsSE(BitBoard bbPiece) const;
    virtual BitBoard getBitsSW(BitBoard bbPiece) const;
};

using TurnGeneratorPtr = std::shared_ptr<TurnGenerator>;


#endif // TURNGENERATOR_H
