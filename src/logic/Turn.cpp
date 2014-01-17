#include "Turn.h"

using namespace std;

bool Turn::operator==(const Turn& other) const {
    return piece == other.piece
            && from == other.from
            && to == other.to
            && action == other.action;
}

bool Turn::operator!=(const Turn& other) const {
    return !(*this == other);
}

std::string Turn::toString() const {
    static const char actions[][20] = {
        "Move",
        "Castle",
        "Forfeit",
        "Pass",
        "PromotionQueen",
        "PromotionBishop",
        "PromotionRook",
        "PromotionKnight"
    };

    std::stringstream ss;
    ss << actions[action] << " " << piece << " " << from << " -> " << to;
    return ss.str();
}




bool turnVecCompare(const std::vector<Turn>& left, const std::vector<Turn>& right) {
    if (left.size() != right.size()) return false;

    auto leftIter  = left.begin();
    auto rightIter = right.begin();
    bool found = false;

    while (rightIter != right.end()) {
        while (leftIter != left.end()) {

            if (*rightIter == *leftIter) {
                found = true;
                break;
            }
            leftIter++;
        }

        if (!found) {
            return false;
        } else {
            found = false;
        }

        leftIter = left.begin();
        rightIter++;
    }

    return true;
}

std::string turnVecToString(std::vector<Turn> v) {
    std::stringstream ss;

    ss << endl << endl;

    if (v.empty()) {
        ss << "empty turn list" << endl;
    } else {
        for (Turn& turn: v) {
            ss << turn << endl;
        }
    }

    return ss.str();
}
