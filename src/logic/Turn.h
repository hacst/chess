#ifndef TURN_H
#define TURN_H

#include "GameState.h"

class Turn {
public:
    Piece piece;
    Field from, to;
    enum Action {
        Move, Forfeit, Castle, Pass
    } action;

    Turn()
        : piece(), from(), to(), action(Pass) {}
    Turn(Piece piece, Field from, Field to, Action action)
        : piece(piece), from(from), to(to), action(action) {}


    /*
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
    */

    std::string toString() const;
};

#endif // TURN_H
