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
