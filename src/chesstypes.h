#ifndef CHESSTYPES_H
#define CHESSTYPES_H

#include <ostream>
#include <array>
#include <string>
#include <cassert>
#include <sstream>
#include <stdint.h>

#include "helper.h"


enum PlayerColor {
	None,
	White,
	Black
};

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

			// Use the lower 4bit for white and the upper for black
			BlackInitialKing = 0x11, 
			BlackKing,
			BlackQueen,
			BlackInitialRook,
			BlackRook,
			BlackBishop,
			BlackKnight,
			BlackInitialPawn,
			BlackPawn
		};

		std::array<std::array<Field,8>,8> fields;

		inline Field& operator()(int x, int y) {
			return fields[x][y];
		}

		Board() : fields({{
			{{ Field::BlackInitialRook, Field::BlackKnight, Field::BlackBishop, Field::BlackInitialKing, Field::BlackQueen, Field::BlackBishop, Field::BlackKnight, Field::BlackInitialRook }},
			{{ Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn, Field::BlackInitialPawn }},
			{{ Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty }},
			{{ Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty }},
			{{ Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty }},
			{{ Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty, Field::Empty }},
			{{ Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn, Field::WhiteInitialPawn }},
			{{ Field::WhiteInitialRook, Field::WhiteKnight, Field::WhiteBishop, Field::WhiteQueen, Field::BlackInitialKing, Field::WhiteBishop, Field::WhiteKnight, Field::WhiteInitialRook }} }}) {
		}
		
		bool operator==(const Board& other) const {
			return fields == other.fields;
		}
		
		std::string toString() const;
		
	} board;

	PlayerColor nextTurn;

	State()
	    : board(),
	      nextTurn(PlayerColor::White) {}
	
	bool apply(const Turn& turn) {
		//TODO: Implement this properly
		if (turn.action == Turn::Move) {
			auto& from = board(turn.from.x, turn.from.y);
			auto& to = board(turn.to.x, turn.to.y);
			
			if (to != Board::Field::Empty) {
				LOG(trace) << from << " strikes " << to;
			}
			
			to = from;
			from = Board::Field::Empty;
		} else if (turn.action == Turn::Forfeit) {
			//TODO: Do something
		} else {
			// Assume passed
		}
		
		nextTurn = (nextTurn == PlayerColor::White)
		        ? PlayerColor::Black
		        : PlayerColor::White;
		
		return true;
	}

	bool operator==(const State& other) const {
		return nextTurn == other.nextTurn
		        && board == other.board;
	}
	
	std::string toString() const;
};

namespace std {
std::ostream& operator <<(std::ostream& stream, const PlayerColor c);
std::ostream& operator <<(std::ostream& stream, const State::Board::Field f);
}


#endif // CHESSTYPES_H
