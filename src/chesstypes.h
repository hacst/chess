#ifndef CHESSTYPES_H
#define CHESSTYPES_H

#include <ostream>
#include <string>
#include <cassert>

struct Turn {
	enum Action {
		Move,
		Forfeit
		// ...
	} action;

	int x,y;

	std::string toString() const {
		return "A turn";
	}
};

struct State {
	// Board, ...

	std::string toString() const {
		return "A state";
	}
};

enum PlayerColor {
	None,
	White,
	Black
};

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


#endif // CHESSTYPES_H
