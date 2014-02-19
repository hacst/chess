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
#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include "misc/helper.h"

#include <ostream>
#include <chrono>
#include <memory>
#include <string>

#include <boost/optional.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

/**
 * @brief AI configuration class
 */
class AIConfiguration {
public:
    //! Name of this configuration
    std::string name;
    //! Relative path to opening book. Empty for none
    std::string openingBook;
    //! Time allowed to take for turn
    int maximumTimeForTurnInSeconds;
    //! Ponder when not playing
    bool ponderDuringOpposingPly;
    //! Hard depth limit
    size_t maximumDepth;

    static AIConfiguration defaults();

    std::string toString() const;

private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar & BOOST_SERIALIZATION_NVP(name);
        ar & BOOST_SERIALIZATION_NVP(openingBook);
        ar & BOOST_SERIALIZATION_NVP(maximumTimeForTurnInSeconds);
        ar & BOOST_SERIALIZATION_NVP(ponderDuringOpposingPly);
        ar & BOOST_SERIALIZATION_NVP(maximumDepth);
    }
};

/**
 * @brief Class for holding game configuration parameters.
 * @note Can be stored and read from disc using save/load.
 */
class GameConfiguration {
public:
    GameConfiguration();

    //! Minimum time between turns for display purposes.
    int timeBetweenTurnsInSeconds;
    //! Maximum time between turns after which to time out a move.
    int maximumTurnTimeInSeconds;

    //! Initial game state as FEN string
    std::string initialGameStateFEN;

    //! Selected ai configuration (must match entry in ai)
    int aiSelected;

    //! List of ai configurations ordered by difficulty (simplest first)
    std::vector<AIConfiguration> ai;

    /**
     * @brief Loads a game configuration from disk.
     * @param path Path to file.
     * @return GameConfiguration on success. boost::none on failure.
     */
    static boost::optional<GameConfiguration> load(const std::string& path);
    
    /**
     * @brief Saves a given game configuration to a file.
     * @param config Configuration to save.
     * @param path Path to save configuration to.
     * @return True on success.
     */
    static bool save(const GameConfiguration& config, const std::string& path);

    /**
     * @brief Saves this configuration to the given path.
     * @param path Path to file to save to.
     * @return True on success.
     */
    bool save(const std::string& path) const;

    bool operator==(const GameConfiguration& other) const;

    std::string toString() const;

private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_NVP(timeBetweenTurnsInSeconds);
        ar & BOOST_SERIALIZATION_NVP(maximumTurnTimeInSeconds);
        ar & BOOST_SERIALIZATION_NVP(initialGameStateFEN);
        if (version > 1) {
            ar & BOOST_SERIALIZATION_NVP(aiSelected);
            ar & BOOST_SERIALIZATION_NVP(ai);
        }
    }
};

BOOST_CLASS_VERSION(GameConfiguration, 2)
BOOST_CLASS_VERSION(AIConfiguration, 1)

using GameConfigurationPtr = std::shared_ptr<GameConfiguration>;

#endif // GAMECONFIGURATION_H
