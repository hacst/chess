#ifndef TURN_H
#define TURN_H

#include "ChessTypes.h"

class Turn {
public:
    Piece piece;
    Field from, to;
    enum Action {
        Move, Castle, Forfeit, Pass,
        PromotionQueen, PromotionBishop,
        PromotionKnight, PromotionRook
    } action;

    Turn()
        : piece(Piece(White, NoType)), from(), to(), action(Pass) {}
    Turn(Piece piece, Field from, Field to, Action action)
        : piece(piece), from(from), to(to), action(action) {}

    bool isMove() const { return action == Move; }
    bool isCastling() const { return action == Castle; }
    bool isPromotion() const { return action >= PromotionQueen && action <= PromotionRook; }
    bool isForfeit() const { return action == Forfeit; }
    bool isPass() const { return action == Pass; }
    PieceType getPromotionPieceType() const { return static_cast<PieceType>(action - PromotionQueen + 1); }

    static Turn move(Piece piece, Field from, Field to) {
        return Turn(piece, from, to, Move);
    }

    static Turn castle(Piece piece, Field from, Field to) {
        assert(piece.type == King);

        return Turn(piece, from, to, Castle);
    }

    /*
    static Turn shortCastle(PlayerColor player, PieceType pieceType) {
        if (player == White) {
            if (pieceType == King) {
                return Turn(Piece(White, King), E1, G1);
            } else if (pieceType == Rook) {
                return Turn(Piece(White, Rook), H1, F1);
            }
        } else {
            if (pieceType == King) {
                return Turn(Piece(Black, King), E8, G8);
            } else if (pieceType == Rook) {
                return Turn(Piece(Black, Rook), G8, F8);
            }
        }
    }

    static Turn longCastle(PlayerColor player, PieceType pieceType) {
        if (player == White) {
            if (pieceType == King) {
                return Turn(Piece(White, King), E1, C1);
            } else if (pieceType == Rook) {
                return Turn(Piece(White, Rook), A1, D1);
            }
        } else {
            if (pieceType == King) {
                return Turn(Piece(Black, King), E8, C8);
            } else if (pieceType == Rook) {
                return Turn(Piece(Black, Rook), A8, D8);
            }
        }
    }
    */

    static Turn promotionQueen(Piece piece, Field field) {
        assert((piece.player == White && rankFor(field) == Eight)
            || (piece.player == Black && rankFor(field) == One));

        assert(piece.type == Pawn);

        return Turn(piece, field, field, PromotionQueen);
    }

    static Turn promotionBishop(Piece piece, Field field) {
        assert((piece.player == White && rankFor(field) == Eight)
            || (piece.player == Black && rankFor(field) == One));

        assert(piece.type == Pawn);

        return Turn(piece, field, field, PromotionBishop);
    }

    static Turn promotionRook(Piece piece, Field field) {
        assert((piece.player == White && rankFor(field) == Eight)
            || (piece.player == Black && rankFor(field) == One));

        assert(piece.type == Pawn);

        return Turn(piece, field, field, PromotionRook);
    }

    static Turn promotionKnight(Piece piece, Field field) {
        assert((piece.player == White && rankFor(field) == Eight)
            || (piece.player == Black && rankFor(field) == One));

        assert(piece.type == Pawn);

        return Turn(piece, field, field, PromotionKnight);
    }


    /*
    static Turn forfeit() {
        return Turn(Position(), Position(), Forfeit);
    }
    */
    static Turn pass(PlayerColor player) {
        return Turn(Piece(player, NoType), ERR, ERR, Pass);
    }

    bool operator==(const Turn& other) const;
    bool operator!=(const Turn& other) const;

    std::string toString() const;
};


/* function for comparing (unsorted) turn vectors */
bool        turnVecCompare(const std::vector<Turn>& left, const std::vector<Turn>& right);
/* function for printing all turns in the vector */
std::string turnVecToString(std::vector<Turn> v);
/* function determindes wheater the given vector is part of the other */
bool        turnVecContains(std::vector<Turn> v, std::vector<Turn> other);
/* function determines wheater the given vector contains the given turn or not */
bool        turnVecContains(std::vector<Turn> v, Turn t);

#endif // TURN_H
