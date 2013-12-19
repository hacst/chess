#include "Turn.h"

using namespace std;

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
    ss << actions[action] << " " << piece << " " << from << " -> " << to;
    return ss.str();
}



bool turnVecCompare(const std::vector<Turn>& left, const std::vector<Turn>& right) {

    if (left.size() != right.size()) return false;


    //for (auto )



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
    for (Turn& turn: v) {
        ss << turn << endl;
    }

    return ss.str();
}
