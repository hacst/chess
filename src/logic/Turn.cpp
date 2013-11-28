#include "Turn.h"




bool Turn::operator==(const Turn& other) const {
    return piece == other.piece
            && from == other.from
            && to == other.to
            && action == other.action;
}

std::string Turn::toString() const {
    static const char actions[][10] = {
        "Move",
        "Forfeit",
        "Castle",
        "Pass"
    };

    std::stringstream ss;
    ss << piece << " " << from << " -> " << to << " " << actions[action];
    return ss.str();
}
