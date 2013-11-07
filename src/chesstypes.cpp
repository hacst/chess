#include <sstream>

#include "chesstypes.h"
#include "helper.h"

using namespace std;

namespace std {
std::ostream& operator <<(std::ostream& stream, const PlayerColor c) {
	switch(c) {
	case None:
		stream << "None";
		break;
	case White:
		stream << "White";
		break;
	case Black:
		stream << "Black";
		break;
	default:
		assert(false);
		stream << "Unknown color";
		break;
	}
	
	return stream;
}


std::ostream& operator <<(std::ostream& stream, const State::Board::Field f) {
	typedef State::Board::Field F;
	
	auto r = [&](std::string s) -> std::ostream& {
		stream << s;
		return stream;
	};

	//TODO: Change this to proper chess notation
	switch(f) {
	case F::Empty: return r(" E ");
	case F::WhiteInitialKing: return r("wiK");
	case F::WhiteKing: return r("wK ");
	case F::WhiteQueen: return r("wQ ");
	case F::WhiteInitialRook: return r("wiR");
	case F::WhiteRook: return r("wR ");
	case F::WhiteBishop: return r("wB ");
	case F::WhiteKnight: return r("wKN");
	case F::WhiteInitialPawn: return r("wiP");
	case F::WhitePawn: return r("wP ");

	case F::BlackInitialKing: return r("biK");
	case F::BlackKing: return r("bK ");
	case F::BlackQueen: return r("bQ ");
	case F::BlackInitialRook: return r("biR");
	case F::BlackRook: return r("bR ");
	case F::BlackBishop: return r("bB ");
	case F::BlackKnight: return r("bKN");
	case F::BlackInitialPawn: return r("biP");
	case F::BlackPawn: return r("bP ");
	default: return r("???");
	}
}

}

std::string State::toString() const {
	stringstream ss;
	ss << board << endl;
	ss << "Next turn: " << nextTurn << endl;
	return ss.str();
}

std::string State::Board::toString() const {
	stringstream ss;
	for(auto& row: fields) {
		for (auto& field: row) {
			ss << field << ' ';
		}
		ss << endl;
	}
	
	return ss.str();
}

std::string Position::toString() const {
	std::stringstream ss;
	ss << "(" << x << "," << y << ")";
	return ss.str();
}


std::string Turn::toString() const {
	static const char actions[][10] = {
		"Move",
		"Forfeit",
		"Castle",
		"Pass"
	};

	std::stringstream ss;
	ss << from << " -> " << to << " " << actions[action];
	return ss.str();
}
