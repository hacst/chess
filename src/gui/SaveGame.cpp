/*
    Copyright (c) 2014, Stefan Hacker <dd0t@users.sourceforge.net>

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
#include "SaveGame.h"

#include <fstream>
#include <sstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

using namespace std;

SaveGame::SaveGame()
    : fen()
    , gameMode()
    , humanPlayerColor(NoPlayer)  {
    // Empty
}

SaveGame::SaveGame(
    std::string fen_,
    GamePlay::GameMode gameMode_,
    PlayerColor humanPlayerColor_)
    : fen(fen_)
    , gameMode(gameMode_)
    , humanPlayerColor(humanPlayerColor_) {
    // Empty
}

boost::optional<SaveGame> SaveGame::load(const std::string& path) {
    try {
        ifstream ifs(path);
        boost::archive::xml_iarchive xmli(ifs);

        SaveGame saveGame;
        xmli >> boost::serialization::make_nvp("saveGame", saveGame);
        return saveGame;
    }
    catch (...) {
        return boost::optional<SaveGame>();
    }
}

boost::optional<SaveGame> SaveGame::loadFromSlot(int slot) {
    return load(pathForSlot(slot));
}

bool SaveGame::save(const SaveGame& saveGame, const std::string& path) {
    try {
        ofstream ofs(path);
        boost::archive::xml_oarchive xmlo(ofs);
        xmlo << boost::serialization::make_nvp("saveGame", saveGame);
        return true;
    }
    catch (...) {
        return false;
    }
}


bool SaveGame::save(const std::string& path) const {
    return save(*this, path);
}

bool SaveGame::saveToSlot(int slot) {
    return save(pathForSlot(slot));
}

std::string SaveGame::pathForSlot(int slot) {
    stringstream ss;
    ss << "saveGame-" << slot << ".xml";
    return ss.str();
}
