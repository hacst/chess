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
#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "gui/states/GamePlay.h"

#include <ostream>
#include <string>

#include <boost/optional.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>

/**
* @brief SaveGame class for a single savegame
*/
class SaveGame {
public:
    SaveGame();
    SaveGame(std::string fen_, GamePlay::GameMode gameMode_, PlayerColor humanPlayerColor_);

    //! Saved state in fen notation
    std::string fen;
    //! Game mode (ai vs human etc.)
    GamePlay::GameMode gameMode;
    //! Color of the human player if human vs ai
    PlayerColor humanPlayerColor;

    /**
    * @brief Loads a savegame from disk.
    * @param path Path to file.
    * @return SaveGame on success. boost::none on failure.
    */
    static boost::optional<SaveGame> load(const std::string& path);

    /**
    * @brief Saves a given save game to a file.
    * @param saveGame Save game to save.
    * @param path Path to save file to.
    * @return True on success.
    */
    static bool save(const SaveGame& saveGame, const std::string& path);

    /**
    * @brief Saves this game to the given path.
    * @param path Path to file to save to.
    * @return True on success.
    */
    bool save(const std::string& path) const;

private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar & BOOST_SERIALIZATION_NVP(fen);
        ar & BOOST_SERIALIZATION_NVP(gameMode);
        ar & BOOST_SERIALIZATION_NVP(humanPlayerColor);
    }
};


BOOST_CLASS_VERSION(SaveGame, 1)

using SaveGamePtr = std::shared_ptr<SaveGame>;

#endif // SAVE_GAME
