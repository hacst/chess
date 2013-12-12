#ifndef TURNGENERATOR_H
#define TURNGENERATOR_H

//#include "interface/AbstractTurnGenerator.h"
//#include "GameState.h"


#include "ChessTypes.h"
#include "ChessBoard.h"


enum File {
    A, B, C, D, E, F, G, H
};

enum Rank {
    One, Two, Three, Four, Five, Six, Seven, Eight
};






//class BitBoardTurnGen
class TurnGenerator { //: public AbstractTurnGenerator {
public:
    virtual std::vector<Turn> generateTurns(PlayerColor player, const ChessBoard& cb); // override;

//private:


    //virtual std::array<BitBoard, 6> calcTurns(PlayerColor player, const ChessBoard& cb);
    virtual BitBoard          calcTurns      (Piece piece, BitBoard bbPiece, const ChessBoard& cb);
    virtual std::vector<Turn> bitBoardToTurns(Piece piece, BitBoard bbPiece, BitBoard bbTurns);

    // nonsliding pieces
    virtual BitBoard calcKingTurns  (BitBoard king,    BitBoard allOwnPieces);
    virtual BitBoard calcKnightTurns(BitBoard knights, BitBoard allOwnPieces);
    virtual BitBoard calcPawnTurns  (BitBoard pawns,   BitBoard allOppPieces,
                                     BitBoard allPieces, PlayerColor player);
    // sliding pieces
    virtual BitBoard calcQueenTurns (BitBoard queens,  BitBoard allOwnPieces);
    virtual BitBoard calcBishopTurns(BitBoard bishops, BitBoard allOwnPieces);
    virtual BitBoard calcRookTurns  (BitBoard rooks,   BitBoard allOwnPieces);

    virtual BitBoard maskRank (Rank rank);
    virtual BitBoard clearRank(Rank rank);
    virtual BitBoard maskFile (File file);
    virtual BitBoard clearFile(File file);
};

using TurnGeneratorPtr = std::shared_ptr<TurnGenerator>;


#endif // TURNGENERATOR_H
