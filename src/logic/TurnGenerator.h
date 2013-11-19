#ifndef TURNGENERATOR_H
#define TURNGENERATOR_H

#include <vector>
#include "GameState.h"
#include "ChessBoard.h"


struct Position {
    int x,y;

    bool operator==(const Position& other) const {
        return x == other.x
                && y == other.y;
    }

    std::string toString() const;
};

struct Turn {
    Position from, to;

    enum Action {
        Move,
        Forfeit,
        Castle,
        Pass
    } action;

    Turn()
        : from()
        , to()
        , action(Pass) {}

    Turn(Position from, Position to, Action action)
        : from(from)
        , to(to)
        , action(action) {}

    static Turn move(Position from, Position to) {
        return Turn(from, to, Move);
    }

    static Turn forfeit() {
        return Turn(Position(), Position(), Forfeit);
    }

    static Turn castle(Position king, Position rook) {
        const int castleOffset = (king.x < rook.x) ? -2 : 2;
        return Turn(king, Position {king.x + castleOffset, king.y}, Castle);
    }

    static Turn pass() {
        return Turn(Position(), Position(), Pass);
    }

    bool operator==(const Turn& other) const {
        return from == other.from
                && to == other.to
                && action == other.action;
    }

    std::string toString() const;
};



class TurnGenerator {
public:
    virtual std::vector<Turn> generateTurns(GameStatePtr state);

private:
    virtual std::array<BitBoard, 6> calcTurnBitBoards(PlayerColor player, ChessBoardPtr board);
    // sliding pieces
    virtual BitBoard calcQueenTurns(BitBoard queen, BitBoard allPieces);
    virtual BitBoard calcBishopTurns(BitBoard bishop, BitBoard allPieces);
    virtual BitBoard calcRookTurns(BitBoard rook, BitBoard allPieces);
    // nonsliding pieces
    virtual BitBoard calcKingTurns(BitBoard king, BitBoard allPieces);
    virtual BitBoard calcKnightTurns(BitBoard knight, BitBoard allPieces);
    virtual BitBoard calcPawnTurns(BitBoard pawn, BitBoard allPieces);
};

using TurnGeneratorPtr = std::shared_ptr<TurnGenerator>;

#endif // TURNGENERATOR_H
