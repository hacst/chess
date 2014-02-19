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
#include <iostream>
#include <cmath>
#include <fstream>
#include <random>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "gui/GuiWindow.h"

#include "misc/helper.h"
#include "core/GameConfiguration.h"
#include "core/Globals.h"
#include "core/Logging.h"

namespace po = boost::program_options;

using namespace std;
using namespace Logging;

int main(int argn, char **argv) {
	// Boost program options for commandline parsing
    po::options_description desc("Test");
    desc.add_options()
            ("help", "Print help message")
            ("width", po::value<int>()->default_value(1024), "Horizontal resolution")
            ("height", po::value<int>()->default_value(768), "Vertical resolution")
            ("config", po::value<string>()->default_value("config.xml"), "Config file")
            ("FEN", po::value<string>(), "Fen string to override configuration")
            ("seed", po::value<int>(), "Seed override for random numbers")
            ("loglvl", po::value<string>()->default_value("debug"), "Set loglevel (trace|debug|info|warning|error|fatal)")
            ("fullscreen", "If set program runs in fullscreen")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argn, argv, desc), vm);

    if (vm.count("help")) {
        cerr << desc << endl;
        return 1;
    }
    
    auto severity = severityFrom(vm["loglvl"].as<string>());
    if (!severity) {
        cerr << "Invalid severity" << endl;
        cerr << desc << endl;
        return 1;
    }
    
    initializeLogging();

    addLoggingConsoleSink(severity.get());
    addLoggingFileSink("3dchess.log", severity.get());
    
    Logger log = initLogger("main");

	std::vector<string> resourceFiles;
	resourceFiles.push_back("resources/3dmodels/bishop.3DS");
	resourceFiles.push_back("resources/3dmodels/chessboard.3DS");
	resourceFiles.push_back("resources/3dmodels/king.3DS");
	resourceFiles.push_back("resources/3dmodels/knight.3DS");
	resourceFiles.push_back("resources/3dmodels/pawn.3DS");
	resourceFiles.push_back("resources/3dmodels/queen.3DS");
	resourceFiles.push_back("resources/3dmodels/rook.3DS");
	resourceFiles.push_back("resources/Book.bin");
	resourceFiles.push_back("resources/Lato-Bla.ttf");
	resourceFiles.push_back("resources/Lato-BlaIta.ttf");
	resourceFiles.push_back("resources/Signika.ttf");

	int errCount = 0;
	for (auto& f : resourceFiles) {
		if (!boost::filesystem::exists(f)) {
			GLOG(error) << "Could not find file '" << f << "'.";
			++errCount;
		}
	}

	if (errCount > 0) {
		GLOG(error) << "Could not find " << errCount << " file(s).";

		return 0;
	}

    auto configFromFile = GameConfiguration::load(vm["config"].as<string>());
    if (!configFromFile) {
        GLOG(error) << "Could not find configuration file '" << vm["config"].as<string>() << "'. Using defaults.";
    } else {
        global_config = *configFromFile;
    }

    string previousInitialGameStateFEN = global_config.initialGameStateFEN;
    if (vm.count("FEN") != 0) {
        global_config.initialGameStateFEN = vm["FEN"].as<string>();
    }
    
    GLOG(info) << global_config;
    
    global_seed = random_device()();
    if (vm.count("seed") != 0) {
        global_seed = vm["seed"].as<int>();
    }
    GLOG(info) << "Game seed: " << global_seed;

    // SDL2/OpenGL for graphics
    const int width = vm["width"].as<int>();
    const int height = vm["height"].as<int>();
    const bool fullscreen = (vm.count("fullscreen") != 0);

    GLOG(info) << "Width:         " << width;
    GLOG(info) << "Height:        " << height;
    GLOG(info) << "Fullscreen:    " << fullscreen;

    GuiWindow window("3D Chess", fullscreen, width, height);
    window.exec();
    
    global_config.initialGameStateFEN = previousInitialGameStateFEN;

    if (!global_config.save(vm["config"].as<string>())) {
        GLOG(error) << "Could not save configuration to '" << vm["config"].as<string>() << "'";
    }

    return 0;
}
