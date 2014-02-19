/*
    Copyright (c) 2013-2014, Stefan Hacker <dd0t@users.sourceforge.net>

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
#include <fstream>
#include <sstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <limits>

#include "GameConfiguration.h"
#include "misc/helper.h"

using namespace std;
using namespace chrono;

string AIConfiguration::toString() const {
    stringstream ss;
    ss << "AIConfiguration:" << endl
        << "  Name              : " << name << endl
        << "  Opening book      : " << openingBook << endl
        << "  Max. turn time    : " << maximumTimeForTurnInSeconds << "s" << endl
        << "  Pondering         : " << ponderDuringOpposingPly << endl
        << "  Max. search depth : " << maximumDepth << endl;

    return ss.str();
}

AIConfiguration AIConfiguration::defaults() {
    return { "Default", "resources/Book.bin", 30, true, 10000 };
}

GameConfiguration::GameConfiguration()
    : timeBetweenTurnsInSeconds(2)
    , maximumTurnTimeInSeconds(std::numeric_limits<int>::max())
    , initialGameStateFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
    , aiSelected(2)
    , ai({
        AIConfiguration { "Simplistic Simon", "", 6, false, 3 },
        AIConfiguration { "Bookish Bert", "resources/Book.bin", 8, false, 10000 },
        AIConfiguration { "Pondering Paula", "resources/Book.bin", 10, true, 10000 }
        }) {
    // Empty
}

boost::optional<GameConfiguration> GameConfiguration::load(const std::string &path) {
    try {
        ifstream ifs(path);
        boost::archive::xml_iarchive xmli(ifs);

        GameConfiguration config;
        xmli >> boost::serialization::make_nvp("configuration", config);
        return config;
    } catch (...) {
        return boost::optional<GameConfiguration>();
    }
}

bool GameConfiguration::save(const GameConfiguration &config, const std::string &path) {
    try {
        ofstream ofs(path);
        boost::archive::xml_oarchive xmlo(ofs);
        xmlo << boost::serialization::make_nvp("configuration", config);
        return true;
    } catch (...) {
        return false;
    }
}

bool GameConfiguration::save(const std::string &path) const {
    return save(*this, path);
}

bool GameConfiguration::operator==(const GameConfiguration& other) const {
    return maximumTurnTimeInSeconds == other.maximumTurnTimeInSeconds;
}

string GameConfiguration::toString() const {
    stringstream ss;
    ss << "GameConfiguration:" << endl
        << "  Maximum turn time: " << maximumTurnTimeInSeconds << "s" << endl
        << "  Minimum turn time: " << timeBetweenTurnsInSeconds << "s" << endl
        << "  Initial FEN: " << initialGameStateFEN << endl
        << "  AI selected: " << aiSelected << endl;
    for (const AIConfiguration& aicfg : ai) {
        ss << aicfg << endl;
    }
    
    
    return ss.str();
}

