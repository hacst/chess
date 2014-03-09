/*
    Copyright (c) 2013-2014, Max Stark <max.stark88@googlemail.com>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#include "Turn.h"
#include <algorithm>

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

bool turnVecContains(std::vector<Turn> v, std::vector<Turn> other) {
    for (Turn& turn: other) {
        if (!turnVecContains(v, turn)) {
            return false;
        }
    }

    return true;
}

bool turnVecContains(std::vector<Turn> v, Turn t) {
    if(std::find(v.begin(), v.end(), t) != v.end()) {
        return true;
    } else {
        return false;
    }
}
