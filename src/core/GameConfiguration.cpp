#include <fstream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "GameConfiguration.h"

using namespace std;
using namespace chrono;

GameConfiguration::GameConfiguration()
	: maximumTurnTimeInSeconds(60) {
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

namespace std {
ostream& operator<<(ostream& stream, const GameConfiguration &config) {
	stream << "GameConfiguration:" << endl
		   << "  Maximum turn time: " << config.maximumTurnTimeInSeconds << "s";

	return stream;
}
}
