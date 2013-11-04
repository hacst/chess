#include "chesstypes.h"
#include "helper.h"

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
