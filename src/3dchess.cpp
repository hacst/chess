#include <iostream>
#include <cmath>
#include <fstream>

#include <boost/program_options.hpp>

#include "helper.h"
#include "GameLogic.h"
#include "ConsolePlayer.h"
#include "ConsoleObserver.h"
#include "DummyPlayer.h"
#include "GameConfiguration.h"
#include "ObserverDispatcherProxy.h"
#include "GuiWindow.h"

namespace po = boost::program_options;

using namespace std;

int main(int argn, char **argv) {
	// Boost program options for commandline parsing

	po::options_description desc("Test");
	desc.add_options()
			("help", "Print help message")
			("width", po::value<int>()->default_value(1024), "Horizontal resolution")
			("height", po::value<int>()->default_value(768), "Vertical resolution")
			("fullscreen", "If set program runs in fullscreen")
			;

	po::variables_map vm;
	po::store(po::parse_command_line(argn, argv, desc), vm);

	if (vm.count("help")) {
		cerr << desc << endl;
		return 1;
	}
	
	auto firstPlayer = make_shared<DummyPlayer>();
	firstPlayer->start();
	auto secondPlayer = make_shared<DummyPlayer>();
	secondPlayer->start();

	auto observer = make_shared<ConsoleObserver>();

	AbstractGameLogicPtr gameLogic = make_shared<GameLogic>(firstPlayer, secondPlayer);
	gameLogic->addObserver(observer);
	auto observerProxy = make_shared<ObserverDispatcherProxy>(observer);
	gameLogic->addObserver(observerProxy);
	
	gameLogic->start();
	
	GameConfiguration config;
	config.save("config.xml");

	auto configl = GameConfiguration::load("config.xml");
	if (!configl) {
		cout << "Failed to load" << endl;
	} else {
		cout << "Dur: " << (configl->maximumTurnTimeInSeconds) << "s" << endl;
	}

	// SDL2/OpenGL for graphics
	const int width = vm["width"].as<int>();
	const int height = vm["height"].as<int>();
	const bool fullscreen = (vm.count("fullscreen") != 0);

	cout << "Width:         " << width << endl;
	cout << "Height:        " << height << endl;
	cout << "Fullscreen:    " << fullscreen << endl;

	GuiWindow *window = new GuiWindow("3D Chess", fullscreen, width, height);
	window->exec();

	return 0;
}
