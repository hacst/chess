#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include "misc/helper.h"

#include <ostream>
#include <chrono>
#include <memory>

#include <boost/optional.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>

struct GameConfiguration {
public:
    GameConfiguration();

    int minimumTurnTimeInSeconds; // Minimum time between turns for AI
    int maximumTurnTimeInSeconds;

    static boost::optional<GameConfiguration> load(const std::string& path);
    static bool save(const GameConfiguration& config, const std::string& path);

    bool save(const std::string& path) const;

    bool operator==(const GameConfiguration& other) const;

    std::string toString() const;

private:
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const unsigned int /*version*/) {
        ar & BOOST_SERIALIZATION_NVP(minimumTurnTimeInSeconds);
        ar & BOOST_SERIALIZATION_NVP(maximumTurnTimeInSeconds);
    }
};

using GameConfigurationPtr = std::shared_ptr<GameConfiguration>;

#endif // GAMECONFIGURATION_H
