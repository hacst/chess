#ifndef TURNGENERATOR_H
#define TURNGENERATOR_H

#include "interface/AbstractTurnGenerator.h"



#define BIT_SET(bitBoard, field) (bitBoard |=   (BitBoard)1 << (field))
#define BIT_CLEAR(bitBoard, field) (bitBoard &= ~((BitBoard)1 << (field)))
#define BIT_TOGGLE(bitBoard, field) (bitBoard ^=   (BitBoard)1 << (field))
#define BIT_ISSET(bitBoard, field) (bitBoard & (  (BitBoard)1 << (field)))

using BitBoard = uint64_t; // unsigned long;
std::string bitBoardToString(BitBoard b);



//class BitBoardTurnGen
class TurnGenerator : public AbstractTurnGenerator {
public:
    virtual std::vector<Turn> generateTurns(
                PlayerColor player,
                const ChessBoard& chessBoard
            ) override;

    //TODO: method for updating interal board representation
    // on GameState.applyTurn?
    //virtual void applyTurn() or update()

//private:
    // for internal turn calculation bit boards are used
    // at least 12 bit boards are needed for a complete board
    // representation
    BitBoard m_pawns[2];
    BitBoard m_rooks[2];
    BitBoard m_knights[2];
    BitBoard m_bishops[2];
    BitBoard m_queens[2];
    BitBoard m_king[2];
    // some additional bit boards for turn calculation
    BitBoard m_allPieces[3];

    //virtual void initBitBoards();
    //virtual void updateBitBoards();
    void generateBitBoards(const ChessBoard& chessBoard);

    static BitBoard maskRank(Rank rank);
    static BitBoard maskFile(File file);
    static BitBoard clearRank(Rank rank);
    static BitBoard clearFile(File file);


    virtual std::array<BitBoard, 6> calcTurns(PlayerColor player);
    // sliding pieces
    //virtual BitBoard calcQueenTurns(BitBoard queen, BitBoard allPieces);
    //virtual BitBoard calcBishopTurns(BitBoard bishop, BitBoard allPieces);
    //virtual BitBoard calcRookTurns(BitBoard rook, BitBoard allPieces);
    // nonsliding pieces
    virtual BitBoard calcKingTurns(BitBoard king, BitBoard allOwnPieces);
    //virtual BitBoard calcKnightTurns(BitBoard knight, BitBoard allPieces);
    //virtual BitBoard calcPawnTurns(BitBoard pawn, BitBoard allPieces);

    virtual std::vector<Turn> bitBoardToTurns(BitBoard b);
};

using TurnGeneratorPtr = std::shared_ptr<TurnGenerator>;


#endif // TURNGENERATOR_H
