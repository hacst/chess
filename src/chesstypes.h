#ifndef CHESSTYPES_H
#define CHESSTYPES_H

#include <ostream>
#include <string>
#include <cassert>
#include <sstream>
#include <stdint.h>

struct Position {
	int x,y;

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

	std::string toString() const;
};

struct State {
	struct Board {
		enum class Field : uint8_t {
			Empty = 0,

			WhiteInitialKing,
			WhiteKing,
			WhiteQueen,
			WhiteInitialRook,
			WhiteRook,
			WhiteBishop,
			WhiteKnight,
			WhiteInitialPawn,
			WhitePawn,

			BlackInitialKing = 0xA,
			BlackKing,
			BlackQueen,
			BlackInitialRook,
			BlackRook,
			BlackBishop,
			BlackKnight,
			BlackInitialPawn,
			BlackPawn
		};

		Field fields[64];

		inline Field operator()(int x, int y) {
			return fields[x + 8 * y];
		}

		Board() : fields {
			Field::BlackInitialRook, Field::BlackKnight, Field::BlackBishop, Field::BlackInitialKing, Field::BlackBishop, Field::BlackKnight, Field::BlackInitialRook,
			Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn,
			Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty,
			Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty,
			Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty,
			Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty,
			Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn,
			Field::WhiteInitialRook, Field::WhiteKnight, Field::WhiteBishop, Field::WhiteQueen, Field::BlackInitialKing, Field::WhiteBishop, Field::WhiteKnight, Field::WhiteInitialRook} {
		}
	} board;

	// Board, ...

	bool apply(const Turn& turn) {
		board(turn.from.x, turn.from.y);
		//TODO: Implement this
		return true;
	}

	std::string toString() const {
		return "A state";
	}
};

enum PlayerColor {
	None,
	White,
	Black
};

std::ostream& operator <<(std::ostream& stream, const PlayerColor c);


#endif // CHESSTYPES_H
