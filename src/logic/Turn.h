#ifndef TURN_H
#define TURN_H

#include "ChessTypes.h"


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



    bool operator==(const Turn& other) const;
    std::string toString() const;


    static Turn move(Piece piece, Field from, Field to) {
        return Turn(piece, from, to, Move);
    }

    /*
    static Turn forfeit() {
        return Turn(Position(), Position(), Forfeit);
    }

    static Turn castle(Position king, Position rook) {
        const int castleOffset = (king.x < rook.x) ? -2 : 2;
        return Turn(king, Position {king.x + castleOffset, king.y}, Castle);
    }

    static Turn pass() {
        return Turn(Position(), Position(), Pass);
    }*/

};

#endif // TURN_H
