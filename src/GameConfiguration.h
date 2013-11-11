#ifndef GAMECONFIGURATION_H
#define GAMECONFIGURATION_H

#include <ostream>
#include <chrono>

#include <boost/optional.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>

struct GameConfiguration {
public:
	GameConfiguration();

	int maximumTurnTimeInSeconds;

	static boost::optional<GameConfiguration> load(const std::string& path);
	static bool save(const GameConfiguration& config, const std::string& path);

	bool save(const std::string& path) const;
	
	bool operator==(const GameConfiguration& other) const;

private:
	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/) {
		ar & BOOST_SERIALIZATION_NVP(maximumTurnTimeInSeconds);
	}
};

namespace std {
ostream& operator <<(ostream& stream, const GameConfiguration& config);
}

#endif // GAMECONFIGURATION_H
