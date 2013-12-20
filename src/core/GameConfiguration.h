#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include "misc/helper.h"

#include <ostream>
#include <chrono>
#include <memory>

#include <boost/optional.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>

/**
 * @brief Class for holding game configuration parameters.
 * @note Can be stored and read from disc using save/load.
 */
struct GameConfiguration {
public:
    GameConfiguration();

    //! Minimum time between turns for display purposes.
    int timeBetweenTurnsInSeconds;
    //! Maximum time between turns after which to time out a move.
    int maximumTurnTimeInSeconds;

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
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar & BOOST_SERIALIZATION_NVP(timeBetweenTurnsInSeconds);
        ar & BOOST_SERIALIZATION_NVP(maximumTurnTimeInSeconds);
    }
};

using GameConfigurationPtr = std::shared_ptr<GameConfiguration>;

#endif // GAMECONFIGURATION_H
