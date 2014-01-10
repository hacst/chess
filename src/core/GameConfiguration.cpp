#include <fstream>

#include <sstream>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <limits>

#include "GameConfiguration.h"

using namespace std;
using namespace chrono;

GameConfiguration::GameConfiguration()
    : timeBetweenTurnsInSeconds(2)
    , maximumTurnTimeInSeconds(std::numeric_limits<int>::max())
    , openingBook("resources/Book.bin")
    , initialGameStateFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
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
        << "  Opening book: " << openingBook << endl;
    return ss.str();
}

