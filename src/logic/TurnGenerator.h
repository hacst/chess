#ifndef TURNGENERATOR_H
#define TURNGENERATOR_H

//#include "interface/AbstractTurnGenerator.h"
//#include "GameState.h"


#include "ChessTypes.h"
#include "ChessBoard.h"

//class BitBoardTurnGen
class TurnGenerator { //: public AbstractTurnGenerator {
public:
    virtual std::vector<Turn> generateTurns(PlayerColor player, const ChessBoard& cb) const; // override;

    //private:


    //virtual std::array<BitBoard, 6> calcTurns(PlayerColor player, const ChessBoard& cb);
    virtual BitBoard          calcTurns(Piece piece, BitBoard bbPiece, const ChessBoard& cb) const;
    virtual std::vector<Turn> bitBoardToTurns(Piece piece, BitBoard bbPiece, BitBoard bbTurns) const;

    // nonsliding pieces
    virtual BitBoard calcKingTurns(BitBoard king, BitBoard allOwnPieces,
                                   bool longCastleRights, bool shortCastleRights) const;
    virtual BitBoard calcKnightTurns(BitBoard knights, BitBoard allOwnPieces) const;
    virtual BitBoard calcPawnTurns(BitBoard pawns, BitBoard allOppPieces,
        BitBoard allPieces, PlayerColor player) const;
    // sliding pieces
    virtual BitBoard calcQueenTurns(BitBoard queens, BitBoard allOppPieces,
        BitBoard allPieces) const;
    virtual BitBoard calcBishopTurns(BitBoard bishops, BitBoard allOppPieces,
        BitBoard allPieces) const;
    virtual BitBoard calcRookTurns(BitBoard rooks, BitBoard allOppPieces,
        BitBoard allPieces) const;

    virtual BitBoard maskRank(Rank rank) const;
    virtual BitBoard clearRank(Rank rank) const;
    virtual BitBoard maskFile(File file) const;
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
